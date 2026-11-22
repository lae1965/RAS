#include <stdbool.h>
#include <time.h>

#include "device.h"

Info info;

bool syncSystemTime(void);

void timerHandler(__attribute__((unused)) int type) {
  static char buf[2048];
  time_t      rawTime  = time(NULL);
  struct tm  *timeInfo = localtime(&rawTime);
  int         hour     = timeInfo->tm_hour;
  int         min      = timeInfo->tm_min;

  if (hour == 0 && min == 0 && timeInfo->tm_sec == 0) syncSystemTime();

  strftime(info.date, sizeof(info.date), "%d.%m.%Y", timeInfo);
  strftime(info.time, sizeof(info.time), "%T", timeInfo);
  info.airTemperature   = 25;  // TODO
  info.humidity         = 60;  // TODO
  info.waterTemperature = 22;  // TODO

  filterListWorkingCorrect();
  feederListWorkingCorrect();

  createReportJSON(buf);
  sendSSE(buf);
  return;
}