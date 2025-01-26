#include <stdbool.h>
#include <time.h>

bool syncSystemTime(void);

#define NO_ACTION 0
#define ROTATION  1
#define FEEDING   1
#define WASHING   2

typedef struct deviceAction {
  char message[25];
  int  type;
  int  value;
} DeviceAction;

void timerHandler(__attribute__((unused)) int type) {
  time_t     rawTime  = time(NULL);
  struct tm *timeInfo = localtime(&rawTime);

  if (timeInfo->tm_hour == 0 && timeInfo->tm_min == 0 && timeInfo->tm_sec == 0) syncSystemTime();

  return;
}