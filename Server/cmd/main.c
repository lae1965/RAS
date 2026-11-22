#include <signal.h>
#include <sys/time.h>

#include "device.h"

int PORT = 8080;

void timerHandler(int);
bool syncSystemTime(void);

int main() {
  if (!http_init(PORT)) return EXIT_FAILURE;

  newFilterService();
  newFeederService();
  getFilterList();
  getFeederList();
  filterListWorkingInit();
  feederListWorkingInit();
  printf("Server has been started on %d port...\n", PORT);

  syncSystemTime();

  struct sigaction sa;
  sa.sa_handler = timerHandler;
  sa.sa_flags   = 0;
  sigemptyset(&sa.sa_mask);

  if (sigaction(SIGALRM, &sa, NULL) == -1) {
    perror("sigaction");
    return EXIT_FAILURE;
  }

  struct itimerval timer;
  timer.it_value.tv_sec     = 1;
  timer.it_value.tv_usec    = 0;
  timer.it_interval.tv_sec  = 1;
  timer.it_interval.tv_usec = 0;

  setitimer(ITIMER_REAL, &timer, NULL);

  while (1) {
    http_listener();
  }

  http_destroy();
  return EXIT_SUCCESS;
}
