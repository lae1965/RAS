#include "filter.h"
#include "mongoose.h"

static const char *URL = "http://localhost:8000";
extern void ev_handler(struct mg_connection *c, int ev, void *ev_data);

int main(void) {
  struct mg_mgr mgr;
  mg_mgr_init(&mgr);

  mg_http_listen(&mgr, URL, ev_handler, NULL);
  printf("Server has been started on %s...\n", URL);

  while (1) mg_mgr_poll(&mgr, 1000);

  mg_mgr_free(&mgr);
  return 0;
}