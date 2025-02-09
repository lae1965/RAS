#ifndef HTTP_H
#define HTTP_H

#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

#define MAX_EVENTS         10
#define BUFFER_SIZE        1024
#define QUERY_KEY_LENGTH   16
#define QUERY_VALUE_LENGTH 32
#define URI_LENGTH         128
#define QUERY_COUNT        16
#define JSON_BODY_LENGTH   512

#define IF_ENDPOINT(endpoint) if (isEndpointEqual(request, endpoint))

typedef enum {
  GET,
  POST,
  PUT,
  DELETE,
  OPTIONS
} httpMethod;

typedef struct {
  char key[QUERY_KEY_LENGTH];
  char value[QUERY_VALUE_LENGTH];
} Query;

typedef struct {
  httpMethod method;
  char       uri[URI_LENGTH];
  Query      query[QUERY_COUNT];
  char       jsonBody[JSON_BODY_LENGTH];
  char       failureMethod[16];
  void      *body;
} RequestParts;

typedef struct http {
  int                epoll_fd;
  struct epoll_event events[MAX_EVENTS];
  int                subscriber_sockets[MAX_EVENTS];
  int                server_fd;
  struct sockaddr_in address;
} Http;

typedef struct {
  int  code;
  char message[32];
} HttpStatus;

typedef struct {
  int  statusCode;
  char message[64];
} HttpError;

extern char      *methodList[];
extern HttpStatus httpStatuses[];
extern Http       http;

bool          http_init(uint16_t port);
void          http_listener(void);
void          http_destroy(void);
RequestParts *parseRequest(char *p_http_request);
bool          isEndpointEqual(RequestParts *request, char *endpoint);
char         *createHttpResponse(int status, char **headers, char *body, bool disableContentLength);
bool          sendResponse(int client_fd, int status, char **headers, char *body, bool disableContentLength);
void          sendSSE(char *message);

#endif