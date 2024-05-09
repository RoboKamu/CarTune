/*!
  \file     butt.c
  \brief    integrating push buttons with MCU to change clock
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32vf103.h"
#include "drivers.h"
#include "lcd.h"
#include "delay.h"
#include "butt.h"
#include <stdio.h>

#define BUTTON1 GPIO_PIN_4    // closest to mcu on breadboard
#define BUTTON2 GPIO_PIN_5
#define BUTTON3 GPIO_PIN_6
#define BUTTON4 GPIO_PIN_7    // furthest from mcy on breadboard

// lookup table used on for-loop to go from index to button definition
#define LOOKUP_BUTT(x)        ((x) == 0 ? BUTTON1 : \
                               (x) == 1 ? BUTTON2 : \
                               (x) == 2 ? BUTTON3 : \
                               (x) == 3 ? BUTTON4 : -1)   // return -1 if none of the buttons reached

void init_pskiva(){
  //t5omsi();                                         // Initialize timer5 1kHz
  Lcd_SetType(LCD_INVERTED);                        // or use LCD_INVERTED!
  Lcd_Init();
  LCD_Clear(BLACK);
  rcu_periph_clock_enable(RCU_GPIOA);                                         // start clock for GPIOA
  gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, BITS(4, 7));     // initialize GPIOA input pins from 4 to 7 
}

void butt(int* pHour, int* pMin){
  int key = -1;

  // store all the button status in an array
  uint32_t butt_status[4];
  for (int i=0; i<4; i++) 
    butt_status[i] = gpio_input_bit_get(GPIOA, LOOKUP_BUTT(i));
  
  // Used pull up resistor so low state means pressed
  for (int i=0; i<4; i++){        
    if (butt_status[i] == 0){     // button pressed
      key = LOOKUP_BUTT(i);      // set key to button 
      break;
    } else key = -1;             // not pressed ? -> set default key
  }

  switch (key){
    case BUTTON1: 
      // Increase hour
      (*pHour)++;
      if ((*pHour) == 24){
        (*pHour) = 0;
      }
      break;
    case BUTTON2:
      // Decrease hour
      (*pHour)--;
      if ((*pHour) == -1){
        (*pHour) = 23;
      }
      break;
    case BUTTON3:
      // Increase minute
      (*pMin)++;
      if ((*pMin) == 60){
        (*pMin) = 0;
        (*pHour)++;
        if ((*pHour) == 24){
          (*pHour) = 0;
        }
      }
      break;
    case BUTTON4:
      // Decrease minute
      (*pMin)--;
      if ((*pMin) == -1){
        (*pMin) = 59;
        (*pHour)--;
        if ((*pHour) == -1){
          (*pHour) = 23;
        }
      }
      break;
  }
}

void displayClock(int hour, int mins){
  // Update LCD display with the new time
  char timeStr[10];
  sprintf(timeStr, "CLOCK: %02d:%02d", hour, mins);
  LCD_ShowStr(10, 30, timeStr, WHITE, OPAQUE);
}