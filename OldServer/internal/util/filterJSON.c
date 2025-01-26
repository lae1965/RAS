#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"
#include "filter.h"

Filter *topFilter = NULL;

static bool parseFilter(cJSON *jsonFilter, Filter *filter) {
  cJSON *jsonFilterName = cJSON_GetObjectItemCaseSensitive(jsonFilter, "filterName");

  if (cJSON_IsString(jsonFilterName) && jsonFilterName->valuestring != NULL && strlen(jsonFilterName->valuestring) < sizeof(filter->filterName)) {
    // NOLINTNEXTLINE
    strcpy(filter->filterName, jsonFilterName->valuestring);
  } else {
    printf("filterName error\n");
    return false;
  }

  filter->isWithDryRotation = cJSON_IsTrue(cJSON_GetObjectItemCaseSensitive(jsonFilter, "isWithDryRotation"));
  filter->isWithTimerWashing = cJSON_IsTrue(cJSON_GetObjectItemCaseSensitive(jsonFilter, "isWithTimerWashing"));
  filter->isWithLevelWashing = cJSON_IsTrue(cJSON_GetObjectItemCaseSensitive(jsonFilter, "isWithLevelWashing"));
  filter->timeBetweenRotations = cJSON_GetObjectItemCaseSensitive(jsonFilter, "timeBetweenRotations")->valueint;
  filter->timeOfRotation = cJSON_GetObjectItemCaseSensitive(jsonFilter, "timeOfRotation")->valueint;
  filter->timeBetweenWashings = cJSON_GetObjectItemCaseSensitive(jsonFilter, "timeBetweenWashings")->valueint;
  filter->timeOfWashing = cJSON_GetObjectItemCaseSensitive(jsonFilter, "timeOfWashing")->valueint;
  filter->levelOfBeginWashing = cJSON_GetObjectItemCaseSensitive(jsonFilter, "levelOfBeginWashing")->valueint;
  return true;
}

bool parseFilterList(const char *jsonFilterList) {
  cJSON *root = cJSON_Parse(jsonFilterList);
  if (root == NULL) {
    const char *errPtr = cJSON_GetErrorPtr();
    printf("filter parsing error");
    if (errPtr != NULL) printf(" before: %s", errPtr);
    printf("./n");
    return false;
  }

  if (!cJSON_IsArray(root)) {
    cJSON_Delete(root);
    printf("invalid filterList structure\n");
    return false;
  }

  int filtersCount = cJSON_GetArraySize(root);
  Filter *prevFilter = NULL;
  for (int i = 0; i < filtersCount; i++) {
    cJSON *jsonFilter = cJSON_GetArrayItem(root, i);
    if (jsonFilter == NULL) {
      // TODO deleteAllFilters();
      cJSON_Delete(root);
      printf("invalid filter #%i structure\n", i + 1);
      return false;
    }

    Filter *filter = calloc(1, sizeof(Filter));
    if (filter == NULL) {
      // TODO deleteAllFilters();
      cJSON_Delete(root);
      printf("memory allocating error: %s\n", strerror(errno));
      return false;
    }

    if (!parseFilter(jsonFilter, filter)) {
      // TODO deleteAllFilters();
      // NOLINTNEXTLINE
      cJSON_Delete(root);
      return false;
    }

    if (prevFilter == NULL)
      topFilter = filter;
    else
      prevFilter->next = filter;

    prevFilter = filter;
  }

  cJSON_Delete(root);
  return true;
}

bool parseFilterObject(const char *jsonFilter) {
  cJSON *root = cJSON_Parse(jsonFilter);
  if (root == NULL) {
    const char *errPtr = cJSON_GetErrorPtr();
    printf("filter parsing error");
    if (errPtr != NULL) printf(" before: %s", errPtr);
    printf("./n");
    return false;
  }

  Filter *filter = calloc(1, sizeof(Filter));
  if (filter == NULL) {
    cJSON_Delete(root);
    printf("memory allocating error: %s\n", strerror(errno));
    return false;
  }

  if (!parseFilter(root, filter)) {
    cJSON_Delete(root);
    free(filter);
    return false;
  }

  cJSON_Delete(root);
  free(filter);
  filter = NULL;

  return true;
}
