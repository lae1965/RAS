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
