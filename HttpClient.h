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
  const char* header;
  const char* value;
} http_header_t;

/**
 * HTTP Request struct.
 * hostname request host
 * path	 request path
 * port     request port
 * body	 request body
 */
typedef struct
{
  String hostname;
  String path;
  // TODO: Look at setting the port by default.
  //int port = 80;
  int port;
  String body;
} http_request_t;

/**
 * HTTP Response struct.
 * status  response status code.
 * body	response body
 */
typedef struct
{
  int status;
  String body;
} http_response_t;

class HttpClient {
public:
    /**
    * Public references to variables.
    */
    //uint16_t port;
    //const char *host;
    //IPAddress ip;
    TCPClient client;
    char buffer[1024];

    /**
    * Constructor.
    */
    HttpClient(void);
    //HttpClient(const char *host, uint16_t port);
    //HttpClient(IPAddress ip, uint16_t port);

    /**
    * HTTP request methods.
    * Can't use 'delete' as name since it's a C++ keyword.
    */
    void get(http_response_t &aResponse, http_request_t &aRequest)
    {
        request(aResponse, aRequest, NULL, HTTP_METHOD_GET);
    }

    void post(http_response_t &aResponse, http_request_t &aRequest)
    {
        request(aResponse, aRequest, NULL, HTTP_METHOD_POST);
    }

    void put(http_response_t &aResponse, http_request_t &aRequest)
    {
        request(aResponse, aRequest, NULL, HTTP_METHOD_PUT);
    }

    void del(http_response_t &aResponse, http_request_t &aRequest)
    {
        request(aResponse, aRequest, NULL, HTTP_METHOD_DELETE);
    }

    void get(http_response_t &aResponse, http_request_t &aRequest, http_header_t headers[])
    {
        request(aResponse, aRequest, headers, HTTP_METHOD_GET);
    }

    void post(http_response_t &aResponse, http_request_t &aRequest, http_header_t headers[])
    {
        request(aResponse, aRequest, headers, HTTP_METHOD_POST);
    }

    void put(http_response_t &aResponse, http_request_t &aRequest, http_header_t headers[])
    {
        request(aResponse, aRequest, headers, HTTP_METHOD_PUT);
    }

    void del(http_response_t &aResponse, http_request_t &aRequest, http_header_t headers[])
    {
        request(aResponse, aRequest, headers, HTTP_METHOD_DELETE);
    }

private:
    /**
    * Underlying HTTP methods.
    */
    void request(http_response_t &aResponse, http_request_t &aRequest, http_header_t headers[], const char* aHttpMethod);
    void sendHeader(const char* aHeaderName, const char* aHeaderValue);
    void sendHeader(const char* aHeaderName, const int aHeaderValue);
    void sendHeader(const char* aHeaderName);
};

#endif /* __HTTP_CLIENT_H_ */
