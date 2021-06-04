#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "yah_http.h"
#include "yah_mem.h"
#include "yah_log.h"
#include "yah_string.h"

char remote_ip[20] = { 0 };
int remote_port = 0;

Request*
yah_request_init() {
    Request* request = 
        (Request*) yah_mem_alloc (sizeof(Request));

    memset(request, 0, sizeof(Request));

    return request;
}

int
yah_request_destory(Request* request) {
    if(request != NULL) {
        yah_mem_free(request);
    }

    return 0;
}

int
yah_request_set_remote(Request* request, const char* ip, int port) {
    if(request == NULL) {
        return 1;
    }
    strcpy(request->ip, ip);
    request->port = port;
    return 0;
}

int
yah_request_set_method(Request* request, int method) {
    if(request == NULL) {
        return 1;
    }
    switch(method) {
        case REQUEST_METHOD_GET:
            strcpy(request->method, "GET");
            break;
        case REQUEST_METHOD_POST:
            strcpy(request->method, "POST");
            break;
        case REQUEST_METHOD_PUT:
            strcpy(request->method, "PUT");
            break;
        case REQUEST_METHOD_DELETE:
            strcpy(request->method, "DELETE");
            break;
        case REQUEST_METHOD_HEAD:
            strcpy(request->method, "HEAD");
            break;
        case REQUEST_METHOD_PATCH:
            strcpy(request->method, "PATCH");
            break;
        default:
            return 1;
    }
    return 0;
}

int
yah_request_set_url(Request* request, const char* url) {
    if(request == NULL) {
        return 1;
    }
    strcpy(request->url, url);
    return 0;
}

int
yah_request_add_header(Request* request, const char* key, const char* value) {
    if(request == NULL) {
        return 1;
    } else if(request->header_count >= YAH_REQUEST_MAX_HEADER_COUNT) {
        return 2;
    }
    strcpy(request->header_keys[request->header_count], key);
    strcpy(request->header_values[request->header_count], value);
    request->header_count++;
    return 0;
}

int
yah_request_set_body(Request* request, const char* body) {
    if(request == NULL) {
        return 1;
    }
    strcpy(request->body, body);
    return 0;
}

int
yah_tcp_scoket_connect(const char* ip, int port) {
    // socket setting
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = PF_INET;
    if(inet_aton(ip, &serv_addr.sin_addr) == 0) {
        yah_error("cannot format %s into binary format.", ip);
    }
    serv_addr.sin_port = htons(port);

    // try connect
    // when connect failed, it will try to sleep 2 * (n+1)s
    // and retry connecting
    // until the max-retry exceed SOCKET_CONNECT_RETRY
    for(int i = 0; i < SOCKET_CONNECT_RETRY; ++i) {
        if(connect(fd, (struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0) {
            yah_log("socket: connect return error. sleep %ds and retry", 2 * (i + 1));
            sleep(2 * (i + 1));
        } else {
            return fd;
        }
    }
    return -1;
}

int
yah_request_send(Request* request) {
    if(request == NULL) {
        return REQUEST_ERROR;
    }

    // generate message here
    char msg[2 * sizeof(Request)] = { 0 };
    yah_http_generate(request, msg);

    // connect to the socket
    int sock = yah_tcp_scoket_connect(request->ip, request->port);
    if(sock < 0) {
        yah_log("request: timeout.");
        return REQUEST_TIMEOUT;
    }

    // send to socket
    if(write(sock, msg, strlen(msg)) == -1) {
        yah_log("request: write socket error, return -1");
        return REQUEST_SEND_ERROR;
    }

    // receive from the socket
    char response[YAH_RESPONSE_MAX_LENGTH] = { 0 };
    if(read(sock, response, YAH_RESPONSE_MAX_LENGTH) == -1) {
        yah_log("request: read socket error, return -1");
        return REQUEST_READ_ERROR;
    }
    yah_log("request: read from socket: %s", response);

    // close socket
    close(sock);
    // get http code from response
    char* begin = response;
    char* end = begin;
    yah_string_get_next_part(response, &begin, &end);
    yah_string_get_next_part(end, &begin, &end);
    *end = 0;
    int code = atoi(begin);
    *begin = ' ';
    if(code >= 200 && code <= 399) {
        return REQUEST_OK;
    } else if(code == 400) {
        return REQUEST_BAD_REQUEST;
    } else if(code == 401) {
        return REQUEST_UNAUTHORIZED;
    } else if(code == 403) {
        return REQUEST_FORBIDDEN;
    } else if(code == 406) {
        return REQUEST_NOTACCETABLE;
    } else if(code == 404) {
        return REQUEST_NOTFOUND;
    } else {
        return REQUEST_ERROR;
    }
}

int
yah_http_generate(Request* request, char* msg) {
    char part[2 * sizeof(Request)] = { 0 };
    sprintf(part, "%s %s HTTP/1.0\r\n", request->method, request->url);
    strcat(msg, part);
    /// headers:
    for(int i = 0; i < request->header_count; i++) {
        sprintf(part, "%s: %s\r\n", request->header_keys[i], request->header_values[i]);
        strcat(msg, part);
    }
    /// generate content-length when request->body exists
    if(request->body[0] != 0) {
        sprintf(part, "Content-Length: %d\r\n", strlen(request->body));
        strcat(msg, part);
    } else {
        sprintf(part, "Content-Length: 0\r\n");
        strcat(msg, part);
    }
    strcat(msg, "\r\n");
    if(request->body[0] != 0) {
        sprintf(part, "%s", request->body);
        strcat(msg, part);
        strcat(msg, "\r\n");
    }
    yah_log("http generated: %s", msg);
    return 0;
}
