#ifndef NTP_H
#define NTP_H

#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

typedef struct {
  unsigned char li_vn_mode;      // Leap indicator, version, and mode
  unsigned char stratum;         // Stratum level
  unsigned char poll;            // Poll interval
  unsigned char precision;       // Clock precision
  unsigned int  rootDelay;       // Root delay
  unsigned int  rootDispersion;  // Root dispersion
  unsigned int  refId;           // Reference ID
  unsigned int  refTimeSec;      // Reference time (seconds)
  unsigned int  refTimeFrac;     // Reference time (fraction of a second)
  unsigned int  origTimeSec;     // Origin time (seconds)
  unsigned int  origTimeFrac;    // Origin time (fraction of a second)
  unsigned int  recvTimeSec;     // Receive time (seconds)
  unsigned int  recvTimeFrac;    // Receive time (fraction of a second)
  unsigned int  txTimeSec;       // Transmit time (seconds)
  unsigned int  txTimeFrac;      // Transmit time (fraction of a second)
} NtpPacket;

#endif