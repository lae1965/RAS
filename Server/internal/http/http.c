#include "http.h"

char      *methodList[]   = {"GET", "POST", "PUT", "DELETE", "OPTIONS"};
HttpStatus httpStatuses[] = {
    {200, "OK"},
    {201, "Created"},
    {204, "No Content"},
    {400, "Bad Request"},
    {401, "Unauthorized"},
    {403, "Forbidden"},
    {404, "Not Found"},
    {409, "Conflict"},
    {500, "Internal Server Error"}
};

Http http;

void handler(int client_fd);

static void formatUriParam(char *decodedString, char *src, int srcLength) {
  char *p = decodedString;

  for (int i = 0; i < srcLength; i++, p++) {
    switch (src[i]) {
      case '%':
        sscanf(&src[i + 1], "%2x", (unsigned int *)p);
        i += 2;
        break;
      case '+':
        *p = ' ';
        break;
      default:
        *p = src[i];
        break;
    }
  }
  *p = '\0';
}

bool http_init(uint16_t port) {
  int opt = 1;

  http.epoll_fd = -1;  // Для корректной отработки http_destroy

  // Создаем сокет
  if ((http.server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    return false;
  }

  // Установить опции сокета
  setsockopt(http.server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  // Подготовка структуры адреса
  http.address.sin_family      = AF_INET;
  http.address.sin_addr.s_addr = INADDR_ANY;
  http.address.sin_port        = htons(port);

  // Привязка сокета к порту
  if (bind(http.server_fd, (struct sockaddr *)&http.address, sizeof(struct sockaddr)) < 0) {
    perror("bind failed");
    http_destroy();
    return false;
  }

  // Подготовка к прослушиванию
  if (listen(http.server_fd, 5) < 0) {
    perror("listen failed");
    http_destroy();
    return false;
  }

  // Создаем epoll экземпляр
  http.epoll_fd = epoll_create1(0);
  if (http.epoll_fd == -1) {
    http_destroy();
    perror("epoll_create1");
    return false;
  }

  // Регистрация серверного сокета
  struct epoll_event ev;
  ev.events  = EPOLLIN;
  ev.data.fd = http.server_fd;
  if (epoll_ctl(http.epoll_fd, EPOLL_CTL_ADD, http.server_fd, &ev) == -1) {
    http_destroy();
    perror("epoll_ctl: server_fd");
    return NULL;
  }

  return true;
}

void http_destroy(void) {
  if (http.server_fd > 0) {
    close(http.server_fd);
    http.server_fd = 0;
  }
  if (http.epoll_fd >= 0) {
    close(http.epoll_fd);
    http.epoll_fd = -1;
  }
}

void http_listener(void) {
  int addrlen = sizeof(struct sockaddr_in);

  int n = epoll_wait(http.epoll_fd, http.events, MAX_EVENTS, 1000);  // тайм-аут 1000 мс

  for (int i = 0; i < n; i++) {
    if (http.events[i].data.fd == http.server_fd) {  // Новый клиент SSE или запрос по REST API
      int client_fd = accept(http.server_fd, (struct sockaddr *)&http.address, (socklen_t *)&addrlen);
      if (client_fd < 0) {
        perror("accept");
        continue;
      }

      handler(client_fd);
    }
  }
}

RequestParts *parseRequest(char *p_http_request) {
  if (p_http_request == NULL) return NULL;
  RequestParts *request = calloc(1, sizeof(RequestParts));

  char *p = p_http_request;
  char *q = p;

  // Получаем метод запроса
  while (*q != ' ') q++;
  int len;
  int methodsCount = sizeof(methodList) / sizeof(methodList[0]), i;
  for (i = 0; i < methodsCount; i++) {
    len = q - p;
    if (strncmp(methodList[i], p, len) == 0) break;
  }

  if (i == methodsCount) {
    strncpy(request->failureMethod, p, len);
    request->failureMethod[len] = '\0';
  }
  request->method = i;
  if (request->method >= OPTIONS) return request;

  // Получаем uri
  q++;
  p = q;
  while (*q != ' ' && *q != '?') q++;
  len = (int)(q - p);
  strncpy(request->uri, p, len);
  request->uri[len] = '\0';

  if (*q == '?') {  // В запросе присутствует Query
    for (int i = 0; i < QUERY_COUNT; i++) {
      // Получаем key
      q++;
      p = q;
      while (*q != '=') q++;
      len = q - p;
      strncpy(request->query[i].key, p, len);
      request->query[i].key[len] = '\0';

      // Получаем value
      q++;
      p = q;
      while (*q != ' ' && *q != '&') q++;
      len = q - p;
      formatUriParam(request->query[i].value, p, len);

      if (*q == ' ') break;
    }
  }

  if (request->method == GET || request->method == DELETE) return request;

  int content_length = 0;
  while (1) {
    while (*q != '\r' && *(q + 1) != '\n') q++;
    if (*(q + 2) == '\r' && *(q + 3) == '\n') break;
    q += 2;
    if (strncmp(q, "Content-Length", 14) == 0) {
      q += 16;
      p              = q;
      content_length = (int)strtol(p, &q, 10);
      break;
    }
  }

  if (content_length == 0) return request;
  // Ищем body
  while (!(q[0] == '\r' && q[1] == '\n' && q[2] == '\r' && q[3] == '\n')) q++;
  q += 4;
  strncpy(request->jsonBody, q, content_length + 1);  // +1 для записи '\0'
  request->jsonBody[content_length] = '\0';

  return request;
}

bool isEndpointEqual(RequestParts *request, char *endpoint) {
  char *uri          = request->uri;
  int   uri_len      = strlen(uri);
  int   endpoint_len = strlen(endpoint);
  bool  isParamExist = false;

  char *q = strchr(endpoint, ':');  // Обрабатываем вариант /endpoint/:param
  char *p;
  if (q != NULL) {
    p = strrchr(uri, '/');
    if (p == NULL) return false;
    isParamExist = true;
    endpoint_len = q - endpoint;
    uri_len      = p - uri;
  }

  if (endpoint_len > 1 && endpoint[endpoint_len - 1] == '/') endpoint_len--;
  if (uri_len > 1 && uri[uri_len - 1] == '/') uri_len--;

  if (endpoint_len != uri_len || strncmp(endpoint, uri, uri_len) != 0) return false;
  if (isParamExist) {
    // NOLINTNEXTLINE
    strcpy(request->query[0].key, q + 1);
    formatUriParam(request->query[0].value, p + 1, strlen(p + 1));
  }

  return true;
}

// массив headers должен завершаться элементом NULL
char *createHttpResponse(int status, char **headers, char *body, bool disableContentLength) {
  char *statusMessage = NULL;

  for (int i = 0, len = sizeof(httpStatuses) / sizeof(httpStatuses[0]); i < len; i++) {
    if (status == httpStatuses[i].code) {
      statusMessage = httpStatuses[i].message;
      break;
    }
  }
  if (statusMessage == NULL) {
    perror("invalid status");
    return NULL;
  }

  if (body == NULL) body = "";
  int contentLength = strlen(body);
  int headersLength = 0;
  for (int i = 0; headers[i] != NULL; i++) headersLength += strlen(headers[i]) + 2;

  int responseLen = strlen(statusMessage) + headersLength + snprintf(NULL, 0, "%d", contentLength) + contentLength + 35;

  char *response = malloc(responseLen + 1);
  if (response == NULL) {
    perror("allocatin memory failed");
    return NULL;
  }

  char *pr = response;
  pr += sprintf(pr, "HTTP/1.1 %d %s\r\n", status, statusMessage);
  for (int i = 0; headers[i] != NULL; i++) pr += sprintf(pr, "%s\r\n", headers[i]);
  if (!disableContentLength) pr += sprintf(pr, "Content-Length: %d\r\n", contentLength);
  *pr++ = '\r';
  *pr++ = '\n';
  // NOLINTNEXTLINE
  strcpy(pr, body);

  return response;
}

// массив headers должен завершаться элементом NULL
bool sendResponse(int client_fd, int status, char **headers, char *body, bool disableContentLength) {
  char *response = createHttpResponse(status, headers, body, disableContentLength);
  if (response == NULL) return false;

  send(client_fd, response, strlen(response), 0);

  free(response);
  return true;
}

void sendSSE(char *message) {
  for (int i = 0; i < MAX_EVENTS; i++) {
    if (http.subscriber_sockets[i] != 0) {
      int       err  = 0;
      socklen_t len  = sizeof(err);
      int       test = getsockopt(http.subscriber_sockets[i], SOL_SOCKET, SO_ERROR, &err, &len);
      if (test == -1 || err) {
        close(http.subscriber_sockets[i]);
        http.subscriber_sockets[i] = 0;
      } else send(http.subscriber_sockets[i], message, strlen(message), 0);
    }
  }
}
