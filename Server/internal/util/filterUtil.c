#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "device.h"
#include "linked_list.h"

Filter *parseFilterObject(const char *jsonFilter, HttpError *httpError) {
  Filter *filter = calloc(1, sizeof(Filter));

  if (!parseProperty(jsonFilter, "\"name\":", &filter->name, string, httpError) ||
      !parseProperty(jsonFilter, "\"isPowerOn\":", &filter->isPowerOn, boolean, httpError) ||
      !parseProperty(jsonFilter, "\"isWithDryRotation\":", &filter->isWithDryRotation, boolean, httpError) ||
      !parseProperty(jsonFilter, "\"isWithTimerWashing\":", &filter->isWithTimerWashing, boolean, httpError) ||
      !parseProperty(jsonFilter, "\"isWithLevelWashing\":", &filter->isWithLevelWashing, boolean, httpError) ||
      !parseProperty(jsonFilter, "\"timeBetweenRotations\":", &filter->timeBetweenRotations, integer, httpError) ||
      !parseProperty(jsonFilter, "\"timeOfRotation\":", &filter->timeOfRotation, integer, httpError) ||
      !parseProperty(jsonFilter, "\"timeBetweenWashings\":", &filter->timeBetweenWashings, integer, httpError) ||
      !parseProperty(jsonFilter, "\"timeOfWashing\":", &filter->timeOfWashing, integer, httpError) ||
      !parseProperty(jsonFilter, "\"levelOfBeginWashing\":", &filter->levelOfBeginWashing, integer, httpError)) {
    perror("invalid json");
    free(filter);
    return NULL;
  }

  return filter;
}

static char *stringifyFilter(char *jsonFilter, Filter *filter) {
  char *p = jsonFilter;

  *p++ = '{';
  *p++ = '\n';

  p = stringifyProperty(p, "name", &filter->name, string, false);
  p = stringifyProperty(p, "isPowerOn", &filter->isPowerOn, boolean, false);
  p = stringifyProperty(p, "isWithDryRotation", &filter->isWithDryRotation, boolean, false);
  p = stringifyProperty(p, "isWithTimerWashing", &filter->isWithTimerWashing, boolean, false);
  p = stringifyProperty(p, "isWithLevelWashing", &filter->isWithLevelWashing, boolean, false);
  p = stringifyProperty(p, "timeBetweenRotations", &filter->timeBetweenRotations, integer, false);
  p = stringifyProperty(p, "timeOfRotation", &filter->timeOfRotation, integer, false);
  p = stringifyProperty(p, "timeBetweenWashings", &filter->timeBetweenWashings, integer, false);
  p = stringifyProperty(p, "timeOfWashing", &filter->timeOfWashing, integer, false);
  p = stringifyProperty(p, "levelOfBeginWashing", &filter->levelOfBeginWashing, integer, true);

  *p++ = '}';

  return p;
}

char *stringifyFilterObject(Filter *filter) {
  char *jsonFilter = malloc(350);

  char *p = stringifyFilter(jsonFilter, filter);
  *p++    = '\n';
  *p      = '\0';

  return jsonFilter;
}

char *stringifyFilterList(void) {
  Node *node         = filterService.list->head;
  int   filtersCount = filterService.list->size(node);
  char *jsonFilter   = malloc(filtersCount * 350 + 5);

  char *p = jsonFilter;
  *p++    = '[';
  *p++    = '\n';

  while (node) {
    p = stringifyFilter(p, (Filter *)((Device *)node->content)->properties);
    if (node->next) *p++ = ',';
    *p++ = '\n';
    node = node->next;
  }
  *p++ = ']';
  *p++ = '\n';
  *p   = '\0';

  return jsonFilter;
}

void filterWorkingInit(Device *device) {
  Filter *filter = (Filter *)device->properties;

  device->da[0].action = device->da[1].action = DISABLED;

  if (!filter->isPowerOn) return;

  if (filter->isWithDryRotation) {
    device->da[0].action        = AWAITING_ROTATION;
    device->da[0].remainingTime = filter->timeBetweenRotations;
  }
  if (filter->isWithTimerWashing) {
    device->da[1].action        = AWAITING_WASHING;
    device->da[1].remainingTime = filter->timeBetweenWashings;
  }
}

void filterWorkingCorrect(Device *device) {
  Filter *filter = (Filter *)device->properties;

  if (!filter->isPowerOn) return;

  DeviceAction *rotate = &device->da[0];
  DeviceAction *wash   = &device->da[1];

  if (rotate->action == DISABLED && wash->action == DISABLED) return;

  if (rotate->action == DISABLED && filter->isWithDryRotation && wash->remainingTime == 1 && wash->action == WASHING) {
    filterWorkingInit(device);
    return;
  }

  if (wash->action == DISABLED && filter->isWithTimerWashing && rotate->remainingTime == 1 && rotate->action == ROTATING) {
    filterWorkingInit(device);
    return;
  }

  if (rotate->action != DISABLED) {
    if (rotate->remainingTime > 1) rotate->remainingTime--;
    else {
      if (rotate->action == AWAITING_ROTATION) {
        rotate->action        = ROTATING;
        rotate->remainingTime = filter->timeOfRotation;
      } else {
        if (wash->action == AWAITING_WASHING && wash->remainingTime < filter->timeBetweenRotations) {
          rotate->action = DISABLED;
        } else if (!filter->isWithDryRotation) filterWorkingInit(device);
        else {
          rotate->action        = AWAITING_ROTATION;
          rotate->remainingTime = filter->timeBetweenRotations;
        }
      }
    }
  }

  if (wash->action != DISABLED) {
    if (wash->remainingTime > 1) wash->remainingTime--;
    else {
      if (wash->action == AWAITING_WASHING) {
        rotate->action      = DISABLED;
        wash->action        = WASHING;
        wash->remainingTime = filter->timeOfWashing;
      } else filterWorkingInit(device);
    }
  }
}

void filterListWorkingInit(void) {
  Node *node = filterService.list->head;

  while (node) {
    filterWorkingInit((Device *)node->content);
    node = node->next;
  }
}

void filterListWorkingCorrect(void) {
  Node *node = filterService.list->head;

  while (node) {
    filterWorkingCorrect((Device *)node->content);
    node = node->next;
  }
}
