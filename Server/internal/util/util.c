#include "device.h"

bool parseProperty(const char *json, char *key, void *value, PropType propType, HttpError *httpError) {
  char *p = strstr(json, key);
  if (p == NULL) {
    httpError->statusCode = 400;
    snprintf(httpError->message, 64, "parsing error: not found key \"%s\" in body json", key);
    return false;
  }

  p += strlen(key);
  while (*p == ' ') p++;
  switch (propType) {
    case integer: {
      int *intValue = (int *)value;
      *intValue     = atoi(p);
      break;
    }
    case boolean: {
      bool *boolValue = (bool *)value;
      *boolValue      = strncmp(p, "true", 4) == 0;
      break;
    }
    case string: {
      char *q = value;
      int   i;
      p++;
      for (i = 0; i < MAX_NAME_SIZE; i++) {
        if (*p == '\"') {
          *q = '\0';
          break;
        }
        *q++ = *p++;
      }
      if (i == MAX_NAME_SIZE) {
        httpError->statusCode = 400;
        snprintf(httpError->message, 64, "invalid json structure: value of key \"%s\" not finished", key);
        return false;
      }
      break;
    }
    default: break;
  }

  return true;
}

char *stringifyProperty(char *json, char *property, void *value, PropType propType, bool lastProperty) {
  char *p = json;

  p += sprintf(p, "    \"%s\": ", property);
  switch (propType) {
    case string:
      p += sprintf(p, "\"%s\"", (char *)value);
      break;
    case boolean: {
      bool *boolValue = (bool *)value;
      p += sprintf(p, "%s", *boolValue ? "true" : "false");
      break;
    }
    case integer: {
      int *intValue = (int *)value;
      p += sprintf(p, "%d", *intValue);
      break;
    }
    default:
      return p;
  }

  if (!lastProperty) *p++ = ',';
  *p++ = '\n';
  return p;
}

// static char *messages[] = {
// 0     "До начала прокрутки:",
// 1     "До конца прокрутки:",
// 2     "До начала промывки:",
// 3     "До конца промывки:",
// 4     "Уровень воды:"
// 5     "До начала кормления: ",
// 6     "До конца кормления: ",
// 7     "Количество корма: ",
// };

enum clientAction {
  _NO_ACTIONS,
  _ROTATION,
  _WASHING,
};
#define _FEEDING _ROTATION

void createReportJSON(char *buf) {
  char *p = buf;
  p += sprintf(p, "data:{\"type\":\"everySecondReport\",");

  p += sprintf(p, "\"filters\":[");
  Node *node = filterService.list->head;
  while (node) {
    enum clientAction ca     = _NO_ACTIONS;
    Device           *device = (Device *)node->content;
    Filter           *filter = (Filter *)device->properties;
    DeviceAction     *da     = device->da;
    *p++                     = '{';
    if (!(da[0].action == DISABLED && da[1].action == DISABLED)) {
      p += sprintf(p, "\"name\":\"%s\",", filter->name);
      p += sprintf(p, "\"messageList\":[");

      bool wasPrevAction = false;
      for (int i = 0; i < 2; i++) {
        if (da[i].action != DISABLED) {
          if (da[i].action == ROTATING && ca != _WASHING) ca = _ROTATION;
          if (da[i].action == WASHING) ca = _WASHING;

          if (wasPrevAction) *p++ = ',';
          p += sprintf(p, "{\"message\":%d,\"time\":%d}", da[i].action - 1, da[i].remainingTime);
          wasPrevAction = true;
        }
      }
      p += sprintf(p, "],\"action\":%d", ca);
    }
    *p++ = '}';
    node = node->next;
    if (node) *p++ = ',';
  }
  p += sprintf(p, "],");

  p += sprintf(p, "\"feeders\":[");
  node = feederService.list->head;
  while (node) {
    Device       *device = (Device *)node->content;
    Feeder       *feeder = (Feeder *)device->properties;
    DeviceAction *da     = device->da;
    *p++                 = '{';
    if (da[0].action != DISABLED) {
      p += sprintf(p, "\"name\":\"%s\",", feeder->name);
      p += sprintf(p, "\"messageList\":[");
      p += sprintf(p, "{\"message\":%d,\"time\":%d}", da[0].action, da[0].remainingTime);
      p += sprintf(p, "],\"action\":%d", da[0].action - 5);
    }
    *p++ = '}';
    node = node->next;
    if (node) *p++ = ',';
  }
  *p++ = ']';

  p += sprintf(p, "}\n\n");
  *p = '\0';
}