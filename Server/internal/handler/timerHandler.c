#include <stdbool.h>
#include <time.h>

#include "device.h"

bool syncSystemTime(void);

void timerHandler(__attribute__((unused)) int type) {
  static char buf[2048];
  time_t      rawTime  = time(NULL);
  struct tm  *timeInfo = localtime(&rawTime);

  if (timeInfo->tm_hour == 0 && timeInfo->tm_min == 0 && timeInfo->tm_sec == 0) syncSystemTime();
  filterListWorkingCorrect();
  feederListWorkingCorrect();

  createReportJSON(buf);
  sendSSE(buf);
  return;
}