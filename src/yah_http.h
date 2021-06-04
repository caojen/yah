#ifndef YAH_HTTP_H
#define YAH_HTTP_H

#define YAH_REQUEST_MAX_METHOD 8
#define YAH_REQUEST_MAX_URL 129
#define YAH_REQUEST_MAX_HEADER_COUNT 5
#define YAH_REQUEST_MAX_HEADER_LENGTH 257
#define YAH_REQUEST_MAX_BODY_LENGTH 4097
#define YAH_RESPONSE_MAX_LENGTH 4097

struct yah_request {

    char ip[20];
    int port;

    char method[YAH_REQUEST_MAX_METHOD];
    char url[YAH_REQUEST_MAX_URL];

    unsigned header_count;
    char header_keys[YAH_REQUEST_MAX_HEADER_COUNT][YAH_REQUEST_MAX_HEADER_LENGTH];
    char header_values[YAH_REQUEST_MAX_HEADER_COUNT][YAH_REQUEST_MAX_HEADER_LENGTH];

    char body[YAH_REQUEST_MAX_BODY_LENGTH];

};

typedef struct yah_request Request;

Request* yah_request_init();
int yah_request_destory(Request* request);

/**
 * All these functions return 0 if success
 * if failed, request will not change, return 1
 */
int yah_request_set_remote(Request* request, const char* ip, int port);
int yah_request_set_method(Request* request, int method);
int yah_request_set_url(Request* request, const char* url);
// the "content-length" will be auto generated when sending.
// but you need to set "content-type"
int yah_request_add_header(Request* request, const char* key, const char* value);
int yah_request_set_body(Request* request, const char* body);

/**
 * send the request
 * after sending, check the http response. if the status is in [200, 399], return REQUEST_OK
 * if socket.connect() error, return REQUEST_TIMEOUT
 * otherwise return other error code
 */
int yah_request_send(Request* request);

#define REQUEST_OK 0
#define REQUEST_TIMEOUT 1
#define REQUEST_BAD_REQUEST 400
#define REQUEST_UNAUTHORIZED 401
#define REQUEST_FORBIDDEN 403
#define REQUEST_NOTFOUND 404
#define REQUEST_NOTACCETABLE 406
#define REQUEST_ERROR 2
#define REQUEST_SEND_ERROR 3
#define REQUEST_READ_ERROR 4

#define REQUEST_METHOD_GET 0
#define REQUEST_METHOD_POST 1
#define REQUEST_METHOD_PUT 2
#define REQUEST_METHOD_DELETE 3
#define REQUEST_METHOD_HEAD 4
#define REQUEST_METHOD_PATCH 5

#define SOCKET_CONNECT_RETRY 20

// dealing with base socket functions
// create socket and try to connect
// based on ip and port
// return the file descriptor of this socket
int yah_tcp_scoket_connect(const char* ip, int port);

#endif
