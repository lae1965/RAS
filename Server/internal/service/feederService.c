#include "device.h"
#include "http.h"

FeederService feederService;

static bool isNamesEqual(void *content1, void *content2) {
  return (strcmp(((Feeder *)(content1))->name, (char *)(content2)) == 0);
}

static bool add(Feeder *feeder, HttpError *httpError) {
  if (feederService.list->getByCondition(feederService.list->head, feeder->name, isNamesEqual)) {
    httpError->statusCode = 409;
    snprintf(httpError->message, 64, "feeder named \"%s\" already exist", feeder->name);
    return false;
  }

  feederService.list->push(&feederService.list->head, feeder);
  saveFeederList();

  return true;
}

static Feeder *get(char *feederName, HttpError *httpError) {
  Node *node = feederService.list->getByCondition(feederService.list->head, feederName, isNamesEqual);
  if (!node) {
    httpError->statusCode = 404;
    snprintf(httpError->message, 64, "feeder named \"%s\" not exist", feederName);
    return NULL;
  }

  return (Feeder *)(node->content);
}

static bool change(Feeder *feederWithNewValues, char *changingFeederName, HttpError *httpError) {
  Node *node = feederService.list->getByCondition(feederService.list->head, changingFeederName, isNamesEqual);
  if (!node) {
    httpError->statusCode = 404;
    snprintf(httpError->message, 64, "feeder named \"%s\" not exist", changingFeederName);
    return false;
  }
  Feeder *changingFeeder = (Feeder *)(node->content);

  // NOLINTNEXTLINE
  strcpy(changingFeeder->name, feederWithNewValues->name);
  changingFeeder->timeBetweenFeedings = feederWithNewValues->timeBetweenFeedings;
  changingFeeder->timeOfFeeding       = feederWithNewValues->timeOfFeeding;

  saveFeederList();
  return true;
}

static bool delete(char *feederName, HttpError *httpError) {
  Node *node = feederService.list->getByCondition(feederService.list->head, feederName, isNamesEqual);
  if (!node) {
    httpError->statusCode = 404;
    snprintf(httpError->message, 64, "feeder named \"%s\" not exist", feederName);
    return false;
  }

  if (!feederService.list->erase(&feederService.list->head, node, true)) {
    httpError->statusCode = 500;
    snprintf(httpError->message, 64, "falure erasing feeder \"%s\"", feederName);
    return false;
  }
  saveFeederList();
  return true;
}

static bool changePower(char *feederName, HttpError *httpError) {
  Node *node = feederService.list->getByCondition(feederService.list->head, feederName, isNamesEqual);
  if (!node) {
    httpError->statusCode = 404;
    snprintf(httpError->message, 64, "feeder named \"%s\" not exist", feederName);
    return false;
  }

  Feeder *feeder = (Feeder *)(node->content);

  feeder->isPowerOn = !feeder->isPowerOn;
  saveFeederList();
  return feeder->isPowerOn;
}

void newFeederService(void) {
  feederService.add         = add;
  feederService.get         = get;
  feederService.change      = change;
  feederService.changePower = changePower;
  feederService.delete      = delete;
}
