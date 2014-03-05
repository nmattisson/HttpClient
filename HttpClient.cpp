#include "HttpClient.h"

// Connecting to an ip instead of a host is implemented, but untested and
// therefore commented out for now.
// HttpClient::HttpClient(IPAddress _ip, uint16_t _port)
// {
//   ip = _ip;
//   port = _port;
//   host = NULL;
// }

HttpClient::HttpClient(const char *_host, uint16_t _port)
{
  host = _host;
  port = _port;
  //ip = INADDR_NONE;
}

http_response_t* HttpClient::request(http_request_t *aRequest, const char* aHttpMethod)
{

  bool connected;
  if (client.connected()) {
    Serial.println("HttpClient: Connection active.");
    connected = true;
  // }  else if (host == NULL) {
  //   Serial.println("HttpClient: Connecting with IP.");
  //   connected = client.connect(ip, port);
  } else {
    Serial.println("HttpClient: Connecting with Hostname.");
    connected = client.connect(host, port);
  }

  if(connected) {
    Serial.println("HttpClient: Starting HTTP Request.");
  } else {
    Serial.println("HttpClient: Connection failed.");
    client.stop();
    return NULL;
  }

  // Send HTTP Request
  client.print(aHttpMethod);
  client.print(" ");
  client.print(aRequest->path);
  client.print(" HTTP/1.0\r\n");
  client.print("HOST: ");
  client.println(host);
  client.println("Connection: close");
  client.println();
  client.flush();

  Serial.println("\r\nHttpClient: Start of HTTP Request.");
  Serial.print(aHttpMethod);
  Serial.print(" ");
  Serial.print(aRequest->path);
  Serial.print(" HTTP/1.0\r\n");
  Serial.print("HOST: ");
  Serial.println(host);
  Serial.println("Connection: close");
  Serial.println("HttpClient: End of HTTP Request.");

  int bytes = 0;
  while(!bytes && client.connected()) {
    bytes = client.available();
    delay(200);
  }

  Serial.println("\r\nHttpClient: Start of HTTP Response.");
  for (int i = 0;  i  < bytes; i++) {
    char c = client.read();
    Serial.print(c);
    if (c == -1) {
      Serial.println("HttpClient Error: No data available.");
      break;
    }

    buffer[i] = c;
  }

  Serial.println("\r\nHttpClient: End of HTTP Response.\n");
  client.stop();

  String raw_response(buffer);
  String statusCode = raw_response.substring(9,12);
  Serial.print("HttpClient Status Code: ");
  Serial.println(statusCode);

  int bodyPos = raw_response.indexOf("\r\n\r\n");
  if (bodyPos == -1) {
    Serial.println("HttpClient Error: Can not find HTTP response body.");
    return NULL;
  }
  // Return the entire message body from bodyPos+4 till end.
  response.body = raw_response.substring(bodyPos+4);
  response.status = atoi(raw_response.substring(9,12).c_str());
  return &response;
}
