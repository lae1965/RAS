#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "device.h"

Feeder *topFeeder = NULL;

Feeder *findFeederByName(char *feederName) {
  Feeder *feeder = topFeeder;

  while (feeder) {
    if (strcmp(feeder->name, feederName) == 0) return feeder;
    feeder = feeder->next;
  }

  return NULL;
}

void deleteAllFeeders() {
  if (!topFeeder) return;

  Feeder *feeder = topFeeder, *prevFeeder;

  do {
    prevFeeder = feeder;
    feeder     = feeder->next;
    free(prevFeeder);
  } while (feeder);

  topFeeder = NULL;
}

bool deleteOneFeeder(char *feederName) {
  Feeder *feeder = topFeeder, *prevFeeder;

  while (feeder) {
    if (strcmp(feederName, feeder->name) == 0) break;
    prevFeeder = feeder;
    feeder     = feeder->next;
  }
  if (!feeder) return false;

  if (feeder == topFeeder) topFeeder = feeder->next;
  else prevFeeder->next = feeder->next;

  free(feeder);

  return true;
}

void insertNewFeeder(Feeder *newFeeder) {
  if (!topFeeder) topFeeder = newFeeder;
  else {
    Feeder *feeder = topFeeder;
    while (feeder->next) feeder = feeder->next;
    feeder->next = newFeeder;
  }
}

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
  Feeder *feeder = topFeeder;

  int feedersCount = 0;
  while (feeder) {
    feedersCount++;
    feeder = feeder->next;
  }

  char *jsonFeeder = malloc(feedersCount * 350 + 5);

  char *p = jsonFeeder;
  *p++    = '[';
  *p++    = '\n';

  feeder = topFeeder;
  while (feeder) {
    p = stringifyFeeder(p, feeder);
    if (feeder->next) *p++ = ',';
    *p++   = '\n';
    feeder = feeder->next;
  }
  *p++ = ']';
  *p++ = '\n';
  *p   = '\0';

  return jsonFeeder;
}
