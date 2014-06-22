# HttpClient for Spark Core | Arduino

This is a work in progress Http Client Library for the Spark Core. It is not ready for use other than for people who have very basic needs, or are willing to help with the development. Because of this, it is currently very verbose and makes heavy use of the serial connection so you can see what's going on. That said, if you are reasonably familiar with Arduino or embedded development you might find it useful. I am publishing it in this early stage mostly because I am myself just starting out with C++ and could use all the help I can get. If you find errors or bad code just let me know and I'll work on fixing it!

There are a couple other options that are probably better suited if you are using a vanilla Arduino and not a Spark Core. First is [Arduino HTTP library](https://github.com/amcewen/HttpClient) from Adrian McEwen. It depends on the Arduino Ethernet API Library though, which may or may not make sense in your implementation. Second there is [HTTPClient](https://github.com/interactive-matter/HTTPClient) from Interactive Matter but it also depends on the same Arduino Ethernet Library. Both of these libraries are orders of magnitude more mature than this one. In the future, it might very well make more sense to reuse a lot of code from these other libraries but to get rid of the dependencies rather than reimplementing things again.

### Example usage

```cpp
#include "application.h"

/**
* Declaring the variables.
*/
unsigned int nextTime = 0;    // Next time to contact the server
HttpClient http;

// Headers currently need to be set at init, useful for API keys etc.
http_header_t headers[] = {
    //  { "Content-Type", "application/json" },
    //  { "Accept" , "application/json" },
    { "Accept" , "*/*"},
    { NULL, NULL } // NOTE: Always terminate headers will NULL
};

http_request_t request;
http_response_t response;

void setup() {
    Serial.begin(9600);
}

void loop() {
    if (nextTime > millis()) {
        return;
    }

    Serial.println();
    Serial.println("Application>\tStart of Loop.");
    // Request path and body can be set at runtime or at setup.
    request.hostname = "www.timeapi.org";
    request.port = 80;
    request.path = "/utc/now";

    // The library also supports sending a body with your request:
    //request.body = "{\"key\":\"value\"}";

    // Get request
    http.get(request, response, headers);
    Serial.print("Application>\tResponse status: ");
    Serial.println(response.status);

    Serial.print("Application>\tHTTP Response Body: ");
    Serial.println(response.body);

    nextTime = millis() + 10000;
}


```
