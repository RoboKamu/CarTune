#include "clock.h"

void init_clock_display(){
  //t5omsi();                                         // Initialize timer5 1kHz
  Lcd_SetType(LCD_INVERTED);                          // or use LCD_INVERTED!
  Lcd_Init();
  LCD_Clear(BLACK);
}
void incrementClock(int8_t *pHour, int8_t *pMinute) {
  (*pMinute)++;
  if ((*pMinute) == 60) {
    (*pMinute) = 0;
    (*pHour)++;
    if ((*pHour) == 24) {
      (*pHour) = 0;
    }
  }
}

void decrementClock(int8_t *pHour, int8_t *pMinute) {
  (*pMinute)--;
  if ((*pMinute) == -1) {
    (*pMinute) = 59;
    (*pHour)--;
    if ((*pHour) == -1) {
      (*pHour) = 23;
    }
  }
}

void tick(int *pMs, int8_t *pHour, int8_t *pMinute) {
  (*pMs)++;
  if ((*pMs) == 60000) {
    (*pMs) = 0;
    incrementClock(pHour, pMinute);
  }
}

void displayClock(int8_t hour, int8_t mins){
  // Update LCD display with the new time
  char timeStr[10];
  sprintf(timeStr, "CLOCK: %02d:%02d", hour, mins);
  LCD_ShowStr(10, 30, timeStr, WHITE, OPAQUE);
}

void pskiva(uint8_t enable, int8_t hour, int8_t min){
  if (enable){
    // pskiva on
    LCD_ShowStr(10, 50, "ON ", GREEN, OPAQUE);
    //char tmpStr[10];
    
    if (min+30 > 60) {                                    // if the coming half hour includes a new hour 
      displayClock(hour+1, 0);     // show minutes as 0 and hour as the coming hour (+1)
    } else {
      displayClock(hour, 30);      // otherwise keep hour and set min to 30
    }
    //displayClock(tmpStr[8], tmpStr[9]);                   // dislay pskiva
    return;
  }
  // otherwise..pskiva off
  LCD_ShowStr(10, 50, "OFF", GREEN, OPAQUE);
  displayClock(hour, min);
}
