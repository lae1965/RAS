#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "device.h"
#include "linked_list.h"

Feeder *parseFeederObject(const char *jsonFeeder, HttpError *httpError) {
  Feeder *feeder = calloc(1, sizeof(Feeder));

  if (!parseProperty(jsonFeeder, "\"name\":", &feeder->name, string, httpError) ||
      !parseProperty(jsonFeeder, "\"isPowerOn\":", &feeder->isPowerOn, boolean, httpError) ||
      !parseProperty(jsonFeeder, "\"timeBetweenFeedings\":", &feeder->timeBetweenFeedings, integer, httpError) ||
      !parseProperty(jsonFeeder, "\"timeOfFeeding\":", &feeder->timeOfFeeding, integer, httpError)) {
    perror("invalid json");
    free(feeder);
    return NULL;
  }

  return feeder;
}

static char *stringifyFeeder(char *jsonFeeder, Feeder *feeder) {
  char *p = jsonFeeder;

  *p++ = '{';
  *p++ = '\n';

  p = stringifyProperty(p, "name", &feeder->name, string, false);
  p = stringifyProperty(p, "isPowerOn", &feeder->isPowerOn, integer, false);
  p = stringifyProperty(p, "timeBetweenFeedings", &feeder->timeBetweenFeedings, integer, false);
  p = stringifyProperty(p, "timeOfFeeding", &feeder->timeOfFeeding, integer, true);

  *p++ = '}';

  return p;
}

char *stringifyFeederObject(Feeder *feeder) {
  char *jsonFeeder = malloc(350);

  char *p = stringifyFeeder(jsonFeeder, feeder);
  *p++    = '\n';
  *p      = '\0';

  return jsonFeeder;
}

char *stringifyFeederList(void) {
  Node *node         = feederService.list->head;
  int   feedersCount = feederService.list->size(node);
  char *jsonFeeder   = malloc(feedersCount * 350 + 5);

  char *p = jsonFeeder;
  *p++    = '[';
  *p++    = '\n';

  while (node) {
    p = stringifyFeeder(p, (Feeder *)((Device *)node->content)->properties);
    if (node->next) *p++ = ',';
    *p++ = '\n';
    node = node->next;
  }
  *p++ = ']';
  *p++ = '\n';
  *p   = '\0';

  return jsonFeeder;
}

void feederWorkingInit(Device *device) {
  Feeder *feeder = (Feeder *)device->properties;

  device->da[0].action = DISABLED;

  if (!feeder->isPowerOn) return;

  device->da->action        = AWAITING_FEEDING;
  device->da->remainingTime = feeder->timeBetweenFeedings;
}

void feederWorkingCorrect(Device *device) {
  Feeder *feeder = (Feeder *)device->properties;

  if (device->da->action == DISABLED) return;
  if (device->da->remainingTime > 1) device->da->remainingTime--;
  else {
    switch (device->da->action) {
      case AWAITING_FEEDING:
        device->da->action        = FEEDING;
        device->da->remainingTime = feeder->timeOfFeeding;
        break;
      case FEEDING:
        feederWorkingInit(device);
        break;
      default:
        break;
    }
  }
}

void feederListWorkingInit(void) {
  Node *node = feederService.list->head;

  while (node) {
    feederWorkingInit((Device *)node->content);
    node = node->next;
  }
}

void feederListWorkingCorrect(void) {
  Node *node = feederService.list->head;

  while (node) {
    feederWorkingCorrect((Device *)node->content);
    node = node->next;
  }
}
