/*!
  \file  main.c
  \brief IMU, LCD & button integration for GD32VF103 MCU
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
#include "gd32v_mpu6500_if.h"
#include "butt.h"
#include "clock.h"
#include "delay.h"
#include <math.h>

#define MARGIN            104                 // margin for movement to turn pskiva on (current ~70% )  
#define INITIAL_HOUR      12
#define INITAL_MIN        0

void handle_imu();

int main(void){
  /* Initialize pins for I2C */
  rcu_periph_clock_enable(RCU_GPIOB);
  rcu_periph_clock_enable(RCU_I2C0);
  gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7);
  /* Initialize the IMU (Notice that MPU6500 is referenced, this is due to the fact that ICM-20600
      ICM-20600 is mostly register compatible with MPU6500, if MPU6500 is used only thing that needs 
      to change is MPU6500_WHO_AM_I_ID from 0x11 to 0x70. */
  mpu6500_install(I2C0);

  // initialize LCD and buttons
  init_butt();
  init_clock_display();

  // The related data structure for the IMU, contains a vector of x, y, z floats
  mpu_vector_t vec, vec_temp;

  // counter variables 
  uint16_t c=0;
  int8_t hunmilli=0, s=0; 
  
  // display variables
  int8_t hour = INITIAL_HOUR;
  int8_t min = INITAL_MIN;
  // initial clock
  displayClock(hour, min);

  // vector scales 
  int32_t v, v_temp;

  // inital vector resp. vecotr magnitude 
  mpu6500_getAccel(&vec_temp);
  v_temp = sqrtf((vec_temp.x*vec_temp.x + vec_temp.y*vec_temp.y + vec_temp.z*vec_temp.z));

  // on or off case for pskiva
  uint8_t enable = 0; 
  while(1){
    delay_until_1ms(100);              // 1 iteration takes 100 ms
    hunmilli++;

    handle_imu(&vec, &vec_temp, &v, &v_temp, &c);         // handle imu..
    butt(&hour, &min, &hunmilli, &s, &enable);            // ..button presses..
    
    if (hunmilli > 9){                                    // after 1 second..
      hunmilli=0;                                         // ..reset hunmilli and..
      s++;                                                // ..increment seconds
      if (!(s%15)) {                                      
        // determine car motion every 15 seconds
        (c > MARGIN) ? pskiva(1, hour, min, &enable) : pskiva(0, hour, min, &enable);
        c=0;
      }
      if (s>59){                                          // after 60 seconds..
        s=0;                                              // ..reset seconds counter..
        incrementClock(&hour, &min);                      // ..and increase clock by 1 minute!
      }
    }
    while (!delay_finished());         // wait until iteration done
  }
}
/*!
 @brief    handles calculations for accelerometer & destroys old so it can be updated
 @param    pVec: pointer to vector struct
 @param    pVec_temp: pointer to temporary vector struct
 @param    pV: pointer to vectro magnitude
 @param    pV_temp: pointer to temp vector magnitude
 @param    pC: pointer to counter
*/
void handle_imu(mpu_vector_t* pVec, mpu_vector_t* pVec_temp, int32_t* pV, int32_t* pV_temp, int* pC){
  /* Get accelleration data (Note: Blocking read) puts a force vector with 1G = 4096 into x, y, z directions respectively */
  mpu6500_getAccel(pVec);
  // calc vector magnitude (ALT. could use fixed point)
  (*pV) = sqrtf((pVec->x*pVec->x + pVec->y*pVec->y + pVec->z*pVec->z));
  
  // note if there is a change in magnitude withing an interval 
  if ( (*pV_temp) <= (*pV)+50 && (*pV_temp) >= (*pV)-50 ) (*pC)++;
  /* Store the last vector so it can be erased */
  (*pVec_temp) = (*pVec);
  (*pV_temp) = (*pV);
}
