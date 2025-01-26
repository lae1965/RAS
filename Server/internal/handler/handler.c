#include "device.h"

static char *contentType   = "Content-Type: application/json";
static char *headersList[] = {
    "Access-Control-Allow-Origin: http://localhost:5173",
    "Access-Control-Allow-Headers: origin, x-requested-with, Content-Type, Content-Length, Cache-Control, Connection",
    "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS",
    "Access-Control-Allow-Credentials: true",
    NULL,
    NULL
};
static char *sseHeaders[] = {
    "Access-Control-Allow-Origin: http://localhost:5173",
    "Access-Control-Allow-Headers: origin, x-requested-with, Content-Type, Content-Length, Cache-Control, Connection",
    "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS",
    "Access-Control-Allow-Credentials: true",
    "Content-Type: text/event-stream",
    "Cache-Control: no-cache",
    "Connection: keep-alive",
    NULL
};

void handler(Http *phttp, int client_fd) {
  char *buffer = malloc(BUFFER_SIZE);
  if (buffer == NULL) {
    sendResponse(client_fd, 500, (char *[]){contentType, NULL}, "failed allocating memory for read request", false);
    return;
  }

  ssize_t read_bytes = read(client_fd, buffer, BUFFER_SIZE - 1);
  buffer[read_bytes] = '\0';

  RequestParts *request = parseRequest(buffer);

  if (request == NULL) {
    free(buffer);
    sendResponse(client_fd, 500, (char *[]){contentType, NULL}, "failed reading request", false);
    return;
  }

  headersList[4] = request->method == OPTIONS ? NULL : contentType;

  HttpError httpError;
  char     *body                 = buffer;
  char     *jsonString           = NULL;
  int       status               = 200;
  bool      needClose_fd         = true;
  bool      disableContentLength = false;
  char    **headers              = headersList;
  switch (request->method) {
    case OPTIONS:
      status  = 204;
      body[0] = '\0';
      break;
    case GET:
      IF_ENDPOINT("/api/filter") {
        jsonString = stringifyFilterList();
        body       = jsonString;
        break;
      }
      IF_ENDPOINT("/api/filter/:filtername") {
        Filter *filter = filterService.get(request->query[0].value, &httpError);
        if (filter) {
          jsonString = stringifyFilterObject(filter);
          body       = jsonString;
        } else {
          status = httpError.statusCode;
          snprintf(body, BUFFER_SIZE, "%s", httpError.message);
        }
        break;
      }
      IF_ENDPOINT("/api/feeder") {
        jsonString = stringifyFeederList();
        body       = jsonString;
        break;
      }
      IF_ENDPOINT("/api/feeder/:feedername") {
        Feeder *feeder = feederService.get(request->query[0].value, &httpError);
        if (feeder) {
          jsonString = stringifyFeederObject(feeder);
          body       = jsonString;
        } else {
          status = httpError.statusCode;
          snprintf(body, BUFFER_SIZE, "%s", httpError.message);
        }
        break;
      }
      IF_ENDPOINT("/sse/subscribe") {
        headers              = sseHeaders;
        needClose_fd         = false;
        disableContentLength = true;
        *body                = '\0';
        // Добавляем клиентский сокет в массив подписчиков
        int i;
        for (i = 0; i < MAX_EVENTS; i++) {
          if (phttp->subscriber_sockets[i] == 0) {
            phttp->subscriber_sockets[i] = client_fd;  // сохраняем сокет
            break;
          }
        }
        if (i == MAX_EVENTS) {
          status = 403;
        }
        break;
      }
      IF_ENDPOINT("/api/filter/change_power/:filtername") {
        bool isPowerOn = filterService.changePower(request->query[0].value, &httpError);
        if (httpError.statusCode != 404) {
          status = 204;
          snprintf(body, BUFFER_SIZE, "data: {\"type\": \"changeFilterPower\", \"name\": \"%s\", \"isPowerOn\": %s}\n\n", request->query[0].value, isPowerOn ? "true" : "false");
          sendSSE(phttp->subscriber_sockets, body);
          *body = '\0';
        } else {
          status = httpError.statusCode;
          snprintf(body, BUFFER_SIZE, "%s", httpError.message);
        }
        break;
      }
      IF_ENDPOINT("/api/feeder/change_power/:feedername") {
        bool isPowerOn = feederService.changePower(request->query[0].value, &httpError);
        if (httpError.statusCode != 404) {
          status = 204;
          snprintf(body, BUFFER_SIZE, "data: {\"type\": \"changeFeederPower\", \"name\": \"%s\", \"isPowerOn\": %s}\n\n", request->query[0].value, isPowerOn ? "true" : "false");
          sendSSE(phttp->subscriber_sockets, body);
          *body = '\0';
        } else {
          status = httpError.statusCode;
          snprintf(body, BUFFER_SIZE, "%s", httpError.message);
        }
        break;
      }
      snprintf(body, BUFFER_SIZE, "Method: %s, uri: %s not found", methodList[request->method], request->uri);
      status = 404;
      break;
    case POST:
      IF_ENDPOINT("/api/filter") {
        Filter *filter = parseFilterObject(request->jsonBody, &httpError);
        if (!filter) {
          snprintf(body, BUFFER_SIZE, "%s", httpError.message);
          status = httpError.statusCode;
          break;
        }

        if (filterService.add(filter, &httpError)) {
          snprintf(body, BUFFER_SIZE, "filter %s created successfully", filter->name);
          status = 201;
        } else {
          snprintf(body, BUFFER_SIZE, "%s", httpError.message);
          status = httpError.statusCode;
        }
        break;
      }
      IF_ENDPOINT("/api/feeder") {
        Feeder *feeder = parseFeederObject(request->jsonBody, &httpError);
        if (!feeder) {
          snprintf(body, BUFFER_SIZE, "%s", httpError.message);
          status = httpError.statusCode;
          break;
        }

        if (feederService.add(feeder, &httpError)) {
          snprintf(body, BUFFER_SIZE, "feeder %s created successfully", feeder->name);
          status = 201;
        } else {
          snprintf(body, BUFFER_SIZE, "%s", httpError.message);
          status = httpError.statusCode;
        }
        break;
      }
      snprintf(body, BUFFER_SIZE, "Method: %s, uri: %s not found", methodList[request->method], request->uri);
      status = 404;
      break;
    case PUT:
      IF_ENDPOINT("/api/filter/:filtername") {
        Filter *filter = parseFilterObject(request->jsonBody, &httpError);
        if (!filter) {
          snprintf(body, BUFFER_SIZE, "%s", httpError.message);
          status = httpError.statusCode;
          break;
        }

        if (filterService.change(filter, request->query[0].value, &httpError))
          snprintf(body, BUFFER_SIZE, "filter %s changed successfully", request->query[0].value);
        else {
          snprintf(body, BUFFER_SIZE, "%s", httpError.message);
          status = httpError.statusCode;
        }

        free(filter);
        break;
      }
      IF_ENDPOINT("/api/feeder/:feedername") {
        Feeder *feeder = parseFeederObject(request->jsonBody, &httpError);
        if (!feeder) {
          snprintf(body, BUFFER_SIZE, "%s", httpError.message);
          status = httpError.statusCode;
          break;
        }

        if (feederService.change(feeder, request->query[0].value, &httpError))
          snprintf(body, BUFFER_SIZE, "feeder %s changed successfully", request->query[0].value);
        else {
          snprintf(body, BUFFER_SIZE, "%s", httpError.message);
          status = httpError.statusCode;
        }

        free(feeder);
        break;
      }
      snprintf(body, BUFFER_SIZE, "Method: %s, uri: %s not found", methodList[request->method], request->uri);
      status = 404;
      break;
    case DELETE:
      IF_ENDPOINT("/api/filter/:filtername") {
        if (filterService.delete(request->query[0].value, &httpError)) {
          snprintf(body, BUFFER_SIZE, "filter %s deleted successfully", request->query[0].value);
          status = 204;
        } else {
          snprintf(body, BUFFER_SIZE, "%s", httpError.message);
          status = httpError.statusCode;
        }
        break;
      }
      IF_ENDPOINT("/api/feeder/:feedername") {
        if (feederService.delete(request->query[0].value, &httpError)) {
          snprintf(body, BUFFER_SIZE, "feeder %s deleted successfully", request->query[0].value);
          status = 204;
        } else {
          snprintf(body, BUFFER_SIZE, "%s", httpError.message);
          status = httpError.statusCode;
        }
        break;
      }
      snprintf(body, BUFFER_SIZE, "Method: %s, uri: %s not found", methodList[request->method], request->uri);
      status = 404;
      break;
    default:
      snprintf(body, BUFFER_SIZE, "Method: %s not exist for this application", request->failureMethod);
      status = 404;
      break;
  }

  sendResponse(client_fd, status, headers, body, disableContentLength);
  if (needClose_fd) close(client_fd);
  if (jsonString) free(jsonString);
  free(request);
  free(buffer);
}
