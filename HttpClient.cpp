#include "HttpClient.h"

/**
* Constructor, variables will initialise the TCPClient.
*/
HttpClient::HttpClient(const char *_host, uint16_t _port)
{
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
http_response_t* HttpClient::request(http_request_t *aRequest, http_header_t headers[], const char* aHttpMethod)
{

    if (client.connected()) {
        Serial.println("HttpClient>\tConnection active.");
    } else if (client.connect(host, port)) {
        Serial.print("HttpClient>\tConnecting with Hostname: ");
        Serial.println(host);
    } else {
        Serial.println("HttpClient>\tConnection failed.");
        client.stop();
        return NULL;
    }

    // Send HTTP Request
    Serial.println("\r\nHttpClient>\tStart of HTTP Request.");

    // Send initial headers (only HTTP 1.0 is supported for now).
    client.print(aHttpMethod);
    client.print(" ");
    client.print(aRequest->path);
    client.print(" HTTP/1.0\r\n");

    Serial.print(aHttpMethod);
    Serial.print(" ");
    Serial.print(aRequest->path);
    Serial.print(" HTTP/1.0\r\n");

    // Send General and Request Headers.
    sendHeader("Connection", "close");
    sendHeader("HOST", host);

    //Send Entity Headers
    // TODO: Check the standard, currently sending Content-Length : 0 for empty
    // POST requests, and no content-length for other types.
    if (aRequest->body != NULL) {
        sendHeader("Content-Length", (aRequest->body).length());
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
    Serial.println();
    // TODO: Flush seems to do nothing at the moment, causing an extra character
    // to be appended to the body and resulting in technically invalid JSON. Most
    // parsers will probably handle it just fine though.
    client.flush();

    // Send HTTP Request body.
    Serial.println(aRequest->body);
    client.println(aRequest->body);
    Serial.println("HttpClient>\tEnd of HTTP Request.");

    int bytes = 0;
    while(!bytes && client.connected()) {
        bytes = client.available();
        delay(200);
    }

    Serial.println("\r\nHttpClient>\tStart of HTTP Response.");
    for (int i = 0;  i  < bytes; i++) {
        char c = client.read();
        Serial.print(c);
        if (c == -1) {
            Serial.println("HttpClient>\tError: No data available.");
            break;
        }

        buffer[i] = c;
    }

    Serial.println("\r\nHttpClient>\tEnd of HTTP Response.\n");
    client.stop();

    String raw_response(buffer);
    // Not super elegant way of finding the status code, but it works.
    String statusCode = raw_response.substring(9,12);
    Serial.print("HttpClient>\tStatus Code: ");
    Serial.println(statusCode);

    int bodyPos = raw_response.indexOf("\r\n\r\n");
    if (bodyPos == -1) {
        Serial.println("HttpClient>\tError: Can not find HTTP response body.");
        return NULL;
    }
    // Return the entire message body from bodyPos+4 till end.
    response.body = raw_response.substring(bodyPos+4);
    response.status = atoi(raw_response.substring(9,12).c_str());
    return &response;
}
