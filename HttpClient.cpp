#include "HttpClient.h"

#define LOGGING

/**
* Constructor.
*/
HttpClient::HttpClient()
{

}

/**
* Method to send a header, should only be called from within the class.
*/
void HttpClient::sendHeader(const char* aHeaderName, const char* aHeaderValue)
{
    client.print(aHeaderName);
    client.print(": ");
    client.println(aHeaderValue);

    Serial.print(aHeaderName);
    Serial.print(": ");
    Serial.println(aHeaderValue);
}

void HttpClient::sendHeader(const char* aHeaderName, const int aHeaderValue)
{
    client.print(aHeaderName);
    client.print(": ");
    client.println(aHeaderValue);

    Serial.print(aHeaderName);
    Serial.print(": ");
    Serial.println(aHeaderValue);
}

void HttpClient::sendHeader(const char* aHeaderName)
{
    client.println(aHeaderName);
    Serial.println(aHeaderName);
}

/**
* Method to send an HTTP Request. Allocate variables in your application code
* in the aResponse struct and set the headers and the options in the aRequest
* struct.
*/
void HttpClient::request(http_request_t &aRequest, http_response_t &aResponse, http_header_t headers[], const char* aHttpMethod)
{
    // If a proper response code isn't received it will be set to -1.
    aResponse.status = -1;

    if (client.connect(aRequest.hostname.c_str(), aRequest.port)) {
        Serial.print("HttpClient>\tConnecting to: ");
        Serial.print(aRequest.hostname);
        Serial.print(":");
        Serial.println(aRequest.port);
    } else {
        Serial.println("HttpClient>\tConnection failed.");
        client.stop();
        return;
    }

    // Send HTTP Request
    Serial.println("HttpClient>\tStart of HTTP Request.");

    // Send initial headers (only HTTP 1.0 is supported for now).
    client.print(aHttpMethod);
    client.print(" ");
    client.print(aRequest.path);
    client.print(" HTTP/1.0\r\n");

    Serial.print(aHttpMethod);
    Serial.print(" ");
    Serial.print(aRequest.path);
    Serial.print(" HTTP/1.0\r\n");

    // Send General and Request Headers.
    sendHeader("Connection", "close"); // Not supporting keep-alive for now.
    sendHeader("HOST", aRequest.hostname.c_str());

    // TODO: Support for connecting with IP address instead of URL
    // if (aRequest.hostname == NULL) {
    //     //sendHeader("HOST", ip);
    // } else {
    //     sendHeader("HOST", aRequest.hostname);
    // }


    //Send Entity Headers
    // TODO: Check the standard, currently sending Content-Length : 0 for empty
    // POST requests, and no content-length for other types.
    if (aRequest.body != NULL) {
        sendHeader("Content-Length", (aRequest.body).length());
    } else if (strcmp(aHttpMethod, HTTP_METHOD_POST) == 0) { //Check to see if its a Post method.
        sendHeader("Content-Length", 0);
    }

    if (headers != NULL)
    {
        int i = 0;
        while (headers[i].header != NULL)
        {
            if (headers[i].value != NULL) {
                sendHeader(headers[i].header, headers[i].value);
            } else {
                sendHeader(headers[i].header);
            }
            i++;
        }
    }

    // Empty line to finish headers
    client.println();
    client.flush();

    // Send HTTP Request body if applicable.
    if (aRequest.body != NULL) {
        Serial.println(aRequest.body);
        client.println(aRequest.body);
    }
    Serial.println("HttpClient>\tEnd of HTTP Request.");


    // Keep waiting for new data to arrive within 5 seconds. The first value of client.available() might not
    // represent the whole response.
    unsigned int i = 0;
    unsigned long lastRead = millis();
    bool keepWaiting = true;
    while ( keepWaiting) {
        if(!client.connected()) {
            Serial.println("HttpClient>\tClient disconnected/end of stream");
            break;
        }

        if((millis() - lastRead > 5000)) {
            Serial.println("HttpClient>\tError: Timeout while reading response");
            break;
        }

        int bytes = bytes = client.available();

        if(bytes) {
            #ifdef LOGGING
            Serial.print("\r\nHttpClient>\tStart of HTTP Response (chunk) of ");
            Serial.print(bytes);
            Serial.println(" bytes.");
            #endif
        }

        while (client.available()) {
            char c = client.read();
            lastRead = millis();

            if (c == -1) {
                Serial.println("HttpClient>\tError: No data available.");
                keepWaiting = false;
                break;
            }

            // Check that received character fits in buffer before storing.
            if (i < sizeof(buffer)-1) {
                buffer[i] = c;
            } else if ((i == sizeof(buffer)-1)) {
                Serial.println("HttpClient>\tError: Response body larger than buffer.");
                buffer[i] = '\0'; // Null terminate buffer
                client.stop();
                keepWaiting = false;
                break;
            }
            i++;
        }
    }

    #ifdef LOGGING
    Serial.println(buffer);
    Serial.println("HttpClient>\tEnd of HTTP Response.");
    #endif

    client.stop();

    String raw_response(buffer);

    // Not super elegant way of finding the status code, but it works.
    String statusCode = raw_response.substring(9,12);
    Serial.print("HttpClient>\tStatus Code: ");
    Serial.println(statusCode);

    int bodyPos = raw_response.indexOf("\r\n\r\n");
    if (bodyPos == -1) {
        Serial.println("HttpClient>\tError: Can't find HTTP response body.");
        return;
    }
    // Return the entire message body from bodyPos+4 till end.
    aResponse.body = raw_response.substring(bodyPos+4);
    aResponse.status = atoi(statusCode.c_str());
}
