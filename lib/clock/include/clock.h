#pragma once

#include <stdio.h>
#include <stdint.h>
#include "lio_lcd.h"

/// @brief    initialize LCD for clock display
void init_clock_display();

/*!
  @brief      increment the clock by 1 minute 
  @param      pHour: pointer to initial hour
  @param      pMinute: pointer to initial minute
*/
void incrementClock(int8_t *pHour, int8_t *pMinute);

/*!
  @brief      decrement the clock by 1 minute
  @param      pHour: pointer to initial hour
  @param      pMinute: pointer to initial minute
*/
void decrementClock(int8_t *pHour, int8_t *pMinute);

/*!
  @brief      increment the clock by 1 millisecond 
  @param      pMs: pointer to initial millisecond
  @param      pHour: pointer to initial hour
  @param      pMinute: pointer to initial minute
*/
void tick(int *pMs, int8_t *pHour, int8_t *pMinute);

/// @brief    displays HH:MM on LCD display
/// @param    hour: HH
/// @param    mins: MM
void displayClock(int8_t hour, int8_t mins);

/*!
  @brief      turn the pskiva on or off and shows the value 
  @param      enable: 1 to turn on; 0 to turn off
  @param      hour: current hours
  @param      min: current minutes
  @param      pEnable: pointer to current pskiva status variable
  @attention  According to swedish law ( 49 a §) on setting the pskiva it should be set to the nearest COMING half hour. 
              This means if the clock is for example 17.04 the pskiva should be set to 17.30, likewise if the 
              clock is 16.32 it should be set to 17.00
              read more here: www.riksdagen.se/sv/dokument-och-lagar/dokument/svensk-forfattningssamling/trafikforordning-19981276_sfs-1998-1276/ 
*/
void pskiva(uint8_t enable, int8_t hour, int8_t min, uint8_t* pEnable);