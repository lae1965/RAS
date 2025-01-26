#ifndef DEVICE_H
#define DEVICE_H

#include <stdbool.h>
#include <stdio.h>

#include "http.h"
#include "linked_list.h"

#define MAX_NAME_SIZE 32

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

typedef struct {
  SinglyLinkedList *list;
  bool (*add)(Filter *filter, HttpError *httpError);
  Filter *(*get)(char *filterName, HttpError *httpError);
  bool (*change)(Filter *filterWithNewValues, char *changingFilterName, HttpError *httpError);
  bool (*changePower)(char *filterName, HttpError *httpError);
  bool (*delete)(char *filterName, HttpError *httpError);
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
} FeederService;

extern Filter       *topFilter;
extern FilterService filterService;
extern Feeder       *topFeeder;
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

Filter *findFilterByName(char *filterName);
void    deleteAllFilters();
bool    deleteOneFilter(char *filterName);
void    insertNewFilter(Filter *newFilter);
Filter *parseFilterObject(const char *jsonFilter, HttpError *httpError);
char   *stringifyFilterObject(Filter *filter);
char   *stringifyFilterList(void);

Feeder *findFeederByName(char *feederName);
void    deleteAllFeeders();
bool    deleteOneFeeder(char *feederName);
void    insertNewFeeder(Feeder *newFeeder);
Feeder *parseFeederObject(const char *jsonFeeder, HttpError *httpError);
char   *stringifyFeederObject(Feeder *feeder);
char   *stringifyFeederList(void);

#endif