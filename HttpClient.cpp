#include "HttpClient.h"

#define LOGGING

/**
* Constructor, variables will initialise the TCPClient.
*/
HttpClient::HttpClient(const char *_host, uint16_t _port)
{
    host = _host;
    port = _port;
}

void HttpClient::init(const char *_host, uint16_t _port) {
	host = _host;
	port = _port;
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
* Method to send an HTTP Request. Set the headers and the options in the
* aRequest struct.
*/
http_response_t HttpClient::request(http_request_t& aRequest, http_header_t headers[], const char* aHttpMethod)
{
	http_response_t response;
    if (client.connected()) {
        Serial.println("HttpClient>\tConnection active.");
    } else if (client.connect(host, port)) {
        Serial.print("HttpClient>\tConnecting with Hostname: ");
        Serial.println(host);
    } else {
        Serial.println("HttpClient>\tConnection failed.");
        client.stop();
        return response;
    }

    // Send initial headers (only HTTP 1.0 is supported for now).
    client.print(aHttpMethod);
    client.print(" ");
    client.print(aRequest.path);
    client.print(" HTTP/1.0\r\n");

    #ifdef LOGGING
    Serial.println("HttpClient>\tStart of HTTP Request.");
    Serial.print(aHttpMethod);
    Serial.print(" ");
    Serial.print(aRequest->path);
    Serial.print(" HTTP/1.0\r\n");
    #endif

    // Send General and Request Headers.
    sendHeader("Connection", "close");
    sendHeader("HOST", host);

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

    // Send HTTP Request body.
    client.println(aRequest.body);

    #ifdef LOGGING
    Serial.println();
    Serial.println(aRequest->body);
    Serial.println("HttpClient>\tEnd of HTTP Request.");
    #endif

    unsigned int bytes = 0;
    while(!bytes && client.connected()) {
        bytes = client.available();
        delay(200);
    }

    #ifdef LOGGING
    Serial.print("\r\nHttpClient>\tStart of HTTP Response of ");
    Serial.print(bytes);
    Serial.println(" bytes.");
    Serial.print("HttpClient>\tBuffer Size: ");
    Serial.print(sizeof(buffer));
    Serial.print("\r\n");
    #endif

    for (unsigned int i = 0;  i  < bytes; i++) {
        char c = client.read();
        if (c == -1) {
            Serial.println("HttpClient>\tError: No data available.");
            break;
        }
        // Check that received character fits in buffer before storing.
        if (i < sizeof(buffer)-1) {
            buffer[i] = c;
        } else if ((i == sizeof(buffer)-1)) {
            buffer[i] = '\0'; // Null terminate buffer
        }
    }

    #ifdef LOGGING
    Serial.print(buffer);
    Serial.println("\r\nHttpClient>\tEnd of HTTP Response.\n");
    #endif

    client.stop();

    String raw_response(buffer);
    // Not super elegant way of finding the status code, but it works.
    String statusCode = raw_response.substring(9,12);
    Serial.print("HttpClient>\tStatus Code: ");
    Serial.println(statusCode);

    int bodyPos = raw_response.indexOf("\r\n\r\n");
    if (bodyPos == -1) {
        Serial.println("HttpClient>\tError: Can not find HTTP response body.");
        return response;
    }
    // Return the entire message body from bodyPos+4 till end.
    response.body = raw_response.substring(bodyPos+4);
    response.status = atoi(raw_response.substring(9,12).c_str());
    return response;
}
