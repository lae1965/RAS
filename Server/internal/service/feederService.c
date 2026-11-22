#include "device.h"
#include "http.h"

FeederService feederService;

static bool isNamesEqual(void *content1, void *content2) {
  return (strcmp(((Feeder *)(((Device *)content1)->properties))->name, (char *)(content2)) == 0);
}

static void clearContent(void *content) {
  Device *device = (Device *)content;
  if (!device) return;
  Feeder *feeder = (Feeder *)device->properties;
  if (feeder) free(feeder);
  free(device);
}

static bool add(Feeder *feeder, HttpError *httpError) {
  if (feederService.list->getByCondition(feederService.list->head, feeder->name, isNamesEqual)) {
    httpError->statusCode = 409;
    snprintf(httpError->message, 64, "feeder named \"%s\" already exist", feeder->name);
    return false;
  }

  Device *device = calloc(1, sizeof(Device));
  if (!device) {
    httpError->statusCode = 500;
    snprintf(httpError->message, 64, "not enough memory to create new feeder");
    return false;
  }
  device->properties = feeder;
  device->type       = FEEDER;
  feederService.list->push(&feederService.list->head, device);
  feederWorkingInit(device);
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

  return (Feeder *)((Device *)node->content)->properties;
}

static bool change(Feeder *feederWithNewValues, char *changingFeederName, HttpError *httpError) {
  Node *node = feederService.list->getByCondition(feederService.list->head, changingFeederName, isNamesEqual);
  if (!node) {
    httpError->statusCode = 404;
    snprintf(httpError->message, 64, "feeder named \"%s\" not exist", changingFeederName);
    return false;
  }
  Device *device         = (Device *)node->content;
  Feeder *changingFeeder = (Feeder *)device->properties;

  // NOLINTNEXTLINE
  strcpy(changingFeeder->name, feederWithNewValues->name);
  changingFeeder->timeBetweenFeedings = feederWithNewValues->timeBetweenFeedings;
  changingFeeder->timeOfFeeding       = feederWithNewValues->timeOfFeeding;

  feederWorkingInit(device);
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

  if (!feederService.list->erase(&feederService.list->head, node, clearContent)) {
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

  Device *device = (Device *)node->content;
  Feeder *feeder = (Feeder *)device->properties;

  feeder->isPowerOn = !feeder->isPowerOn;
  feederWorkingInit(device);
  saveFeederList();
  return feeder->isPowerOn;
}

static bool forcedFeeding(char *feederName, HttpError *httpError) {
  Node *node = feederService.list->getByCondition(feederService.list->head, feederName, isNamesEqual);
  if (!node) {
    httpError->statusCode = 404;
    snprintf(httpError->message, 64, "feeder named \"%s\" not exist", feederName);
    return false;
  }

  Device *device = (Device *)node->content;

  device->da[0].action        = FEEDING;
  device->da[0].remainingTime = ((Feeder *)device->properties)->timeOfFeeding;

  return true;
}

void newFeederService(void) {
  feederService.list          = newSinglyLinkedList();
  feederService.add           = add;
  feederService.get           = get;
  feederService.change        = change;
  feederService.changePower   = changePower;
  feederService.delete        = delete;
  feederService.forcedFeeding = forcedFeeding;
}
