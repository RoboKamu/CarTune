#include "gd32vf103.h"
#include "drivers.h"
#include "lcd.h"
#include "delay.h"
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

int butt(void)
{
  int ms = 0, s = 0, key, pKey = -1, c = 0, idle = 0;
  uint32_t buttons[4] = {BUTTON1, BUTTON2, BUTTON3, BUTTON4};
  uint32_t butt_status;
  int hour = 12;                                    // Initial hour
  int minute = 0;                                   // Initial minute

  t5omsi();                                         // Initialize timer5 1kHz
  Lcd_SetType(LCD_INVERTED);                        // or use LCD_INVERTED!
  Lcd_Init();
  LCD_Clear(RED);
  rcu_periph_clock_enable(RCU_GPIOA);                                         // start clock for GPIOA
  gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, BITS(4, 7));     // initialize GPIOA input pins from 4 to 7 

  LCD_ShowStr(10, 10, "init-pskiva-butt", WHITE, TRANSPARENT);

  while (1)
  {
    LCD_WR_Queue(); // Manage LCD com queue!

    // store all the button status in an array
    int butt_status[4];
    for (int i=0; i<4; i++) 
      butt_status[i] = gpio_input_bit_get(GPIOA, LOOKUP_BUTT(i));
    
    // Used pull up resistor so low state means pressed
    for (int i=0; i<4; i++){        
      if (butt_status[i] == 0){     // button pressed
        pKey = LOOKUP_BUTT(i);      // set pKey to button 
        break;
      } else pKey = -1;             // not pressed ? -> set default pKey
    }

    delay_1ms(150);                 // dont read a button press in coming 150 ms
 
    switch (pKey)
    {
      case BUTTON1: 
        // Increase hour
        hour++;
        if (hour == 24)
        {
          hour = 0;
        }
        break;
      case BUTTON2:
        // Decrease hour
        hour--;
        if (hour == -1)
        {
          hour = 23;
        }
        break;
      case BUTTON3:
        // Increase minute
        minute++;
        if (minute == 60)
        {
          minute = 0;
          hour++;
          if (hour == 24)
          {
            hour = 0;
          }
        }
        break;
      case BUTTON4:
        // Decrease minute
        minute--;
        if (minute == -1)
        {
          minute = 59;
          hour--;
          if (hour == -1)
          {
            hour = 23;
          }
        }
        break;
    }
    // Update LCD display with the new time
    char timeStr[10];
    sprintf(timeStr, "CLOCK: %02d:%02d", hour, minute);
    LCD_ShowStr(10, 30, timeStr, WHITE, OPAQUE);
    LCD_Wait_On_Queue();
  }
}