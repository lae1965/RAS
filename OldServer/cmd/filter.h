#ifndef FILTERH
#define FILTERH

#include <stdbool.h>
#include <stdio.h>

typedef struct filter {
  char filterName[17];
  bool isWithDryRotation;
  bool isWithTimerWashing;
  bool isWithLevelWashing;
  int timeBetweenRotations;
  int timeOfRotation;
  int timeBetweenWashings;
  int timeOfWashing;
  int levelOfBeginWashing;
  struct filter *next;
} Filter;

extern Filter *topFilter;

#endif