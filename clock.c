#include "clock.h"

void incrementClock(int *pHour, int *pMinute) {
  (*pMinute)++;
  if ((*pMinute) == 60) {
    (*pMinute) = 0;
    (*pHour)++;
    if ((*pHour) == 24) {
      (*pHour) = 0;
    }
  }
}

void decrementClock(int *pHour, int *pMinute) {
  (*pMinute)--;
  if ((*pMinute) == -1) {
    (*pMinute) = 59;
    (*pHour)--;
    if ((*pHour) == -1) {
      (*pHour) = 23;
    }
  }
}

void tick(int *pMs, int *pHour, int *pMinute) {
  (*pMs)++;
  if ((*pMs) == 60000) {
    (*pMs) = 0;
    incrementClock(pHour, pMinute);
  }
}
