#include "device.h"
#include "http.h"
#include "linked_list.h"

FilterService filterService;

static bool isNamesEqual(void *content1, void *content2) {
  return (strcmp(((Filter *)(content1))->name, (char *)(content2)) == 0);
}

static bool add(Filter *filter, HttpError *httpError) {
  if (filterService.list->getByCondition(filterService.list->head, filter->name, isNamesEqual)) {
    httpError->statusCode = 409;
    snprintf(httpError->message, 64, "filter named \"%s\" already exist", filter->name);
    return false;
  }

  filterService.list->push(&filterService.list->head, filter);
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

  return (Filter *)(node->content);
}

static bool change(Filter *filterWithNewValues, char *changingFilterName, HttpError *httpError) {
  Node *node = filterService.list->getByCondition(filterService.list->head, changingFilterName, isNamesEqual);
  if (!node) {
    httpError->statusCode = 404;
    snprintf(httpError->message, 64, "filter named \"%s\" not exist", changingFilterName);
    return false;
  }
  Filter *changingFilter = (Filter *)(node->content);

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

  if (!filterService.list->erase(&filterService.list->head, node, true)) {
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

  Filter *filter = (Filter *)(node->content);

  filter->isPowerOn = !filter->isPowerOn;
  saveFilterList();
  return filter->isPowerOn;
}

void newFilterService(void) {
  filterService.list        = newSinglyLinkedList();
  filterService.add         = add;
  filterService.get         = get;
  filterService.change      = change;
  filterService.changePower = changePower;
  filterService.delete      = delete;
}
