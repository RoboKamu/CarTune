/*!
  \file  main.c
  \brief USB CDC ACM device
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
#include "gd32v_mpu6500_if.h"
#include <math.h>

#define GRAPH_HEIGHT    30


int main(void){
  /* The related data structure for the IMU, contains a vector of x, y, z floats*/
  mpu_vector_t vec, vec_temp;
  /* for lcd */
  uint16_t line_color;

  /* Initialize pins for I2C */
  rcu_periph_clock_enable(RCU_GPIOB);
  rcu_periph_clock_enable(RCU_I2C0);
  gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7);
  /* Initialize the IMU (Notice that MPU6500 is referenced, this is due to the fact that ICM-20600
      ICM-20600 is mostly register compatible with MPU6500, if MPU6500 is used only thing that needs 
      to change is MPU6500_WHO_AM_I_ID from 0x11 to 0x70. */
  mpu6500_install(I2C0);
  
  /* Initialize LCD */
  Lcd_SetType(LCD_INVERTED);
  Lcd_Init();
  LCD_Clear(BLACK);
  
  mpu6500_getAccel(&vec_temp);
  int c=0, idle=0; 
  int32_t v, v_temp;

  while(1){
    // display the conditional counter
    LCD_ShowNum(10, 50, c, 3, WHITE);

    /* Get accelleration data (Note: Blocking read) puts a force vector with 1G = 4096 into x, y, z directions respectively */
    mpu6500_getAccel(&vec_temp);
    // calc vector magnitude
    v_temp = sqrtf((vec_temp.x*vec_temp.x + vec_temp.y*vec_temp.y + vec_temp.z*vec_temp.z));
    // and display it
    LCD_ShowNum(10, 10, v, 4, WHITE);
    // wait 100 ms
    delay_1ms(100);
    idle++;

    /* Get accelleration data (Note: Blocking read) puts a force vector with 1G = 4096 into x, y, z directions respectively */
    mpu6500_getAccel(&vec);                                         
    // calc another vec magnitude 
    v = sqrtf((vec.x*vec.x + vec.y*vec.y + vec.z*vec.z));

    // change between intervall? --> c++; 
    if (v_temp <= v+50 && v_temp >= v-50) c++;
    // after 15 seconds chech if c is true ~65% of the time 
    if (idle >= 150){
      if (c>=98)
        LCD_ShowString(10, 30, "On ", RED);
      else 
        LCD_ShowString(10, 30, "Off", GREEN);
      
      // wait for the LCD to finish draw then wait 1 second
      LCD_Wait_On_Queue();
      delay_1ms(1000);
      // reset variables
      c=0; idle=0;
    }
    /* Store the last vector so it can be erased */
    vec_temp = vec;
    /* Wait for LCD to finish drawing */
    LCD_Wait_On_Queue();    
  }
}

