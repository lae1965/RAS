#ifndef DEVICE_H
#define DEVICE_H

#include <stdbool.h>
#include <stdio.h>

#include "http.h"
#include "linked_list.h"

#define MAX_NAME_SIZE 32

typedef enum {
  FILTER,
  FEEDER
} DeviceType;

typedef struct {
  char name[MAX_NAME_SIZE];
  bool isPowerOn;
  bool isWithDryRotation;
  bool isWithTimerWashing;
  bool isWithLevelWashing;
  int  timeBetweenRotations;
  int  timeOfRotation;
  int  timeBetweenWashings;
  int  timeOfWashing;
  int  levelOfBeginWashing;
} Filter;

typedef enum {
  DISABLED,
  AWAITING_ROTATION,
  ROTATING,
  AWAITING_WASHING,
  WASHING,
  AWAITING_FEEDING,
  FEEDING
} Action;

typedef struct {
  Action action;
  int    remainingTime;
} DeviceAction;

typedef struct {
  DeviceType   type;
  void        *properties;
  DeviceAction da[2];
} Device;

typedef struct {
  SinglyLinkedList *list;
  bool (*add)(Filter *filter, HttpError *httpError);
  Filter *(*get)(char *filterName, HttpError *httpError);
  bool (*change)(Filter *filterWithNewValues, char *changingFilterName, HttpError *httpError);
  bool (*changePower)(char *filterName, HttpError *httpError);
  bool (*delete)(char *filterName, HttpError *httpError);
  bool (*forcedRotation)(char *filterName, HttpError *httpError);
  bool (*forcedWashing)(char *filterName, HttpError *httpError);
} FilterService;

typedef struct {
  char name[MAX_NAME_SIZE];
  bool isPowerOn;
  int  timeBetweenFeedings;
  int  timeOfFeeding;
} Feeder;

typedef struct {
  SinglyLinkedList *list;
  bool (*add)(Feeder *feeder, HttpError *httpError);
  Feeder *(*get)(char *feederName, HttpError *httpError);
  bool (*change)(Feeder *feederWithNewValues, char *changingFeederName, HttpError *httpError);
  bool (*changePower)(char *feederName, HttpError *httpError);
  bool (*delete)(char *feederName, HttpError *httpError);
  bool (*forcedFeeding)(char *feederName, HttpError *httpError);
} FeederService;

extern FilterService filterService;
extern FeederService feederService;

// Service
void newFilterService(void);
void newFeederService(void);

// Repo
bool getFilterList(void);
bool saveFilterList(void);
bool getFeederList(void);
bool saveFeederList(void);

// Util
typedef enum {
  integer,
  boolean,
  string
} PropType;

bool  parseProperty(const char *json, char *key, void *value, PropType propType, HttpError *httpError);
char *stringifyProperty(char *json, char *property, void *value, PropType propType, bool lastProperty);
void  createReportJSON(char *buf);

Filter *parseFilterObject(const char *jsonFilter, HttpError *httpError);
char   *stringifyFilterObject(Filter *filter);
char   *stringifyFilterList(void);
void    filterWorkingInit(Device *device);
void    filterWorkingCorrect(Device *device);
void    filterListWorkingInit(void);
void    filterListWorkingCorrect(void);

Feeder *parseFeederObject(const char *jsonFeeder, HttpError *httpError);
char   *stringifyFeederObject(Feeder *feeder);
char   *stringifyFeederList(void);
void    feederWorkingInit(Device *device);
void    feederWorkingCorrect(Device *device);
void    feederListWorkingInit(void);
void    feederListWorkingCorrect(void);

#endif