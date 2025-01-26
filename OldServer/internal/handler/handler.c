#include "mongoose.h"

typedef enum {
  GET,
  POST,
  PATCH,
  DELETE,
  OPTIONS,
  UNKNOWN
} httpMethod;

static char *methodList[] = {"GET", "POST", "PATCH", "DELETE", "OPTIONS"};

static httpMethod getRequestMethod(struct mg_str method) {
  char *buf = method.buf;
  size_t len = method.len;
  int methodsCount = sizeof(methodList) / sizeof(methodList[0]);

  for (int i = 0; i < methodsCount; i++) {
    if (strncmp(buf, methodList[i], len) == 0) return i;
  }

  return UNKNOWN;
}

static bool checkEndpoint(struct mg_str uri, char *endpoint) {
  return (strncmp(uri.buf, endpoint, uri.len) == 0);
}

void ev_handler(struct mg_connection *c, int ev, void *ev_data) {
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *)ev_data;
    struct mg_str uri = hm->uri;
    httpMethod method = getRequestMethod(hm->method);

    switch (method) {
      case OPTIONS:
        mg_http_reply(
            c, 200,
            "Access-Control-Allow-Origin: http://localhost:5173\r\n"
            "Access-Control-Allow-Headers: origin, x-requested-with, content-type\r\n"
            "Access-Control-Allow-Methods: GET, POST, PATCH, DELETE, OPTIONS\r\n"
            "Access-Control-Allow-Credentials: true\r\n"
            "\r\n",
            "");
        return;
      case GET:
        if (checkEndpoint(uri, "/") == true) {
          mg_http_reply(c, 200, "Content-Type: text/plain\r\n", "Method %s endpoint %c\n", methodList[method], '/');
        }
        return;
      case POST:
      case PATCH:
      case DELETE:
        mg_http_reply(c, 200, "Content-Type: text/plain\r\n", "Method %s\n", methodList[method]);
        return;
      default:
        mg_http_reply(c, 404, "Content-Type: text/plain\r\n", "Unknown method\n");
        return;
    }
  }
}
