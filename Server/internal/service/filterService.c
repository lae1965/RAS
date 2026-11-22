#include "device.h"
#include "http.h"
#include "linked_list.h"

FilterService filterService;

static bool isNamesEqual(void *content1, void *content2) {
  return (strcmp(((Filter *)(((Device *)content1)->properties))->name, (char *)(content2)) == 0);
}

static void clearContent(void *content) {
  Device *device = (Device *)content;
  if (!device) return;
  Filter *filter = (Filter *)device->properties;
  if (filter) free(filter);
  free(device);
}

static bool add(Filter *filter, HttpError *httpError) {
  if (filterService.list->getByCondition(filterService.list->head, filter->name, isNamesEqual)) {
    httpError->statusCode = 409;
    snprintf(httpError->message, 64, "filter named \"%s\" already exist", filter->name);
    return false;
  }

  Device *device = calloc(1, sizeof(Device));
  if (!device) {
    httpError->statusCode = 500;
    snprintf(httpError->message, 64, "not enough memory to create new filter");
    return false;
  }

  device->properties = filter;
  device->type       = FILTER;
  filterService.list->push(&filterService.list->head, device);
  filterWorkingInit(device);
  saveFilterList();

  return true;
}

static Filter *get(char *filterName, HttpError *httpError) {
  Node *node = filterService.list->getByCondition(filterService.list->head, filterName, isNamesEqual);
  if (!node) {
    httpError->statusCode = 404;
    snprintf(httpError->message, 64, "filter named \"%s\" not exist", filterName);
    return NULL;
  }

  return (Filter *)((Device *)node->content)->properties;
}

static bool change(Filter *filterWithNewValues, char *changingFilterName, HttpError *httpError) {
  Node *node = filterService.list->getByCondition(filterService.list->head, changingFilterName, isNamesEqual);
  if (!node) {
    httpError->statusCode = 404;
    snprintf(httpError->message, 64, "filter named \"%s\" not exist", changingFilterName);
    return false;
  }
  Device *device         = (Device *)node->content;
  Filter *changingFilter = (Filter *)device->properties;

  // NOLINTNEXTLINE
  strcpy(changingFilter->name, filterWithNewValues->name);
  changingFilter->isWithDryRotation    = filterWithNewValues->isWithDryRotation;
  changingFilter->isWithTimerWashing   = filterWithNewValues->isWithTimerWashing;
  changingFilter->isWithLevelWashing   = filterWithNewValues->isWithLevelWashing;
  changingFilter->timeBetweenRotations = filterWithNewValues->timeBetweenRotations;
  changingFilter->timeOfRotation       = filterWithNewValues->timeOfRotation;
  changingFilter->timeBetweenWashings  = filterWithNewValues->timeBetweenWashings;
  changingFilter->timeOfWashing        = filterWithNewValues->timeOfWashing;
  changingFilter->levelOfBeginWashing  = filterWithNewValues->levelOfBeginWashing;

  filterWorkingInit(device);
  saveFilterList();
  return true;
}

static bool delete(char *filterName, HttpError *httpError) {
  Node *node = filterService.list->getByCondition(filterService.list->head, filterName, isNamesEqual);
  if (!node) {
    httpError->statusCode = 404;
    snprintf(httpError->message, 64, "filter named \"%s\" not exist", filterName);
    return false;
  }

  if (!filterService.list->erase(&filterService.list->head, node, clearContent)) {
    httpError->statusCode = 500;
    snprintf(httpError->message, 64, "falure erasing filter \"%s\"", filterName);
    return false;
  }
  saveFilterList();
  return true;
}

static bool changePower(char *filterName, HttpError *httpError) {
  Node *node = filterService.list->getByCondition(filterService.list->head, filterName, isNamesEqual);
  if (!node) {
    httpError->statusCode = 404;
    snprintf(httpError->message, 64, "filter named \"%s\" not exist", filterName);
    return false;
  }

  Device *device = (Device *)node->content;
  Filter *filter = (Filter *)device->properties;

  filter->isPowerOn = !filter->isPowerOn;
  filterWorkingInit(device);
  saveFilterList();
  return filter->isPowerOn;
}

static bool forcedRotation(char *filterName, HttpError *httpError) {
  Node *node = filterService.list->getByCondition(filterService.list->head, filterName, isNamesEqual);
  if (!node) {
    httpError->statusCode = 404;
    snprintf(httpError->message, 64, "filter named \"%s\" not exist", filterName);
    return false;
  }

  Device *device = (Device *)node->content;
  Filter *filter = (Filter *)device->properties;

  device->da[0].action = ROTATING;
  if (filter->isWithDryRotation) device->da[0].remainingTime = filter->timeOfRotation;
  else device->da[0].remainingTime = 30;
  device->da[1].action = DISABLED;

  return true;
}

static bool forcedWashing(char *filterName, HttpError *httpError) {
  Node *node = filterService.list->getByCondition(filterService.list->head, filterName, isNamesEqual);
  if (!node) {
    httpError->statusCode = 404;
    snprintf(httpError->message, 64, "filter named \"%s\" not exist", filterName);
    return false;
  }

  Device *device = (Device *)node->content;
  Filter *filter = (Filter *)device->properties;

  device->da[1].action = WASHING;
  if (filter->isWithTimerWashing) device->da[1].remainingTime = filter->timeOfWashing;
  else device->da[1].remainingTime = 30;
  device->da[0].action = DISABLED;

  return true;
}

void newFilterService(void) {
  filterService.list           = newSinglyLinkedList();
  filterService.add            = add;
  filterService.get            = get;
  filterService.change         = change;
  filterService.changePower    = changePower;
  filterService.delete         = delete;
  filterService.delete         = delete;
  filterService.forcedRotation = forcedRotation;
  filterService.forcedWashing  = forcedWashing;
}
