#include "clock.h"

#define BITMASK   0xFFFFFFF8
#define RED 	    0xF800
#define GREEN	    0x07E0
#define BLUE	    0x001F

void init_clock_display(){
  lio_lcd_config_t lcd_conf = {0x00, 160, 80, 0, 24};
	lio_lcd_init(lcd_conf);
	lio_lcd_clear(0);
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
  lio_lcd_printf_scaled(15, 15, 0xFFFF, 3, "%02d:%02d", hour, mins);
}

void pskiva(uint8_t state, int8_t hour, int8_t min, uint8_t* pEnable){
  // update time only if there is a change in state, not when its already on
  if ((*pEnable) > 0 && state > 0) return; 
  
  if (state){                         // pskiva on
    (*pEnable) = 1;                   // note enabled
    if (min+30 > 60) {                // if the coming half hour includes a new hour 
      displayClock(hour+1, 0);        // show minutes as 0 and hour as the coming hour (+1)
    } else {
      displayClock(hour, 30);         // otherwise keep hour and set min to 30
    }
    return; 
  }
  // otherwise..pskiva off
  displayClock(hour, min);
  (*pEnable) = 0;
}
