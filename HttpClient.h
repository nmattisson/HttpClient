#ifndef __HTTP_CLIENT_H_
#define __HTTP_CLIENT_H_

#include "spark_wiring_string.h"
#include "spark_wiring_tcpclient.h"
#include "spark_wiring_usbserial.h"

/**
 * Defines for the HTTP methods.
 */
#define HTTP_METHOD_GET    "GET"
#define HTTP_METHOD_POST   "POST"
#define HTTP_METHOD_PUT    "PUT"
#define HTTP_METHOD_DELETE "DELETE"

/**
 * This struct is used to pass additional HTTP headers such as API-keys.
 * Normally you pass this as an array. The last entry must have NULL as key.
 */
typedef struct
{
  char* key;
  char* value;
} http_header_t;

/**
 * HTTP Request struct.
 * @param		method	HTTP verb for request.
 * @param		path	request path
 * @param		headers	pointer to array of headers
 * @param		body	request body
 */
typedef struct
{
  http_header_t headers[];
  String hostname;
  String path;
  int port;
	String body;
} http_request_t;

/**
 * HTTP Response struct.
 * @param		method	HTTP verb for request.
 * @param		path	request path
 * @param		headers	pointer to array of headers
 * @param		body	request body
 */
typedef struct
{
  int status;
  http_header_t headers[];
  String body;
} http_response_t;

class HttpClient {
public:
  /**
   * Public references
   */
  //IPAddress ip;
  uint16_t port;
  const char *host;
  http_response_t response;
  TCPClient client;
  char buffer[512];

  /**
   * Constructor.
   */
  //HttpClient(IPAddress ip, uint16_t port);
  HttpClient(const char *host, uint16_t port);

  /**
   * HTTP request methods.
   */
  http_response_t* get(http_request_t *aRequest)
  {
    return request(aRequest, HTTP_METHOD_GET);
  }

  http_response_t* post(http_request_t *aRequest)
  {
    return request(aRequest, HTTP_METHOD_POST);
  }

  http_response_t* put(http_request_t *aRequest)
  {
    return request(aRequest, HTTP_METHOD_PUT);
  }

  // Can't use 'delete' as name since it's a C++ keyword.
  http_response_t* del(http_request_t *aRequest)
  {
    return request(aRequest, HTTP_METHOD_DELETE);
  }

private:
  /**
   * Underlying HTTP method.
   */
  http_response_t* request(http_request_t *aRequest,  const char* aHttpMethod);
};

#endif /* __HTTP_CLIENT_H_ */
