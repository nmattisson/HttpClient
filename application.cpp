#include "application.h"

/**
* Declaring the variables.
*/
unsigned int nextTime = 0;    // Next time to contact the server

//char const* hostname = "www.timeapi.org";
char const* hostname = "validate.jsontest.com";
int port = 80;
HttpClient http(hostname, port);

// Headers currently need to be set at init, useful for API keys etc.
http_header_t headers[] = {
    { "Content-Type", "application/json" },
    //  { "Accept" , "application/json"},
    { "Accept" , "*/*"},
    //  { "HeaderWithoutKey", NULL },
    { NULL, NULL } // NOTE: Always terminate headers will NULL
};

http_request_t request;
http_response_t* response;

void setup() {
    Serial.begin(9600);

    // Request path and body can of course be set at runtime.
    request.path = "/?json=";
    request.body = "{\"key\":\"value\"}";
}

void loop() {
    if (nextTime > millis()) {
        return;
    }

    response = http.post(&request, headers);
    Serial.print("Application>\tHTTP Response Status: ");
    Serial.println(response->status);
    Serial.print("Application>\tHTTP Response Body: ");
    Serial.println(response->body);

    nextTime = millis() + 10000;

}
