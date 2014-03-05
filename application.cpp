#include "application.h"

/**
 * Declaring the variables.
 */
unsigned int nextTime = 0;    // Next time to contact the server
char const* hostname = "www.timeapi.org";
int port = 80;
HttpClient http(hostname, port);

http_request_t request;
http_response_t* response;

void setup() {
	Serial.begin(9600);
  request.path = "/utc/now";
}

void loop() {
	if (nextTime > millis()) {
		return;
	}

  response = http.get(&request);
  Serial.print("Application: HTTP Response Status: ");
  Serial.println(response->status);
  Serial.print("Application: HTTP Response Body: ");
  Serial.println(response->body);

	nextTime = millis() + 10000;

}
