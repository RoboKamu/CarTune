/*!
  \file  main.c
  \brief IMU, LCD & button integration for MCU
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
#include "system_gd32vf103.h"
#include "drivers.h"
#include "gd32v_mpu6500_if.h"
#include "lcd.h"
#include "butt.h"
#include "clock.h"
#include "gd32vf103_rtc.h"
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

  // initialize timer and LCD
  init_pskiva();

  /* ----------- REAL TIME CLOCK  ------------ 

  // enable power management unit
  rcu_periph_clock_enable(RCU_PMU);
  // enable write access to backup domain registers
  pmu_backup_write_enable();
  rtc_register_sync_wait();
  // enable backup domain
  rcu_periph_clock_enable(RCU_BKPI);
  // reset backup domain regiser
  bkp_deinit();

  int cc=0;
  //        SETUP
  while(1){
    LCD_ShowNum(10, 10, cc, 4, WHITE);
    LCD_ShowNum(10, 30, rtc_counter_get(), 6, WHITE);
    // enable external low-speed crystal oscillator (XO)
    if (rcu_osci_stab_wait(RCU_HXTAL)){
      // use external low speed oscillator {i.e 32.768 kHz}
      rcu_rtc_clock_config(RCU_RTCSRC_HXTAL_DIV_128); 
      rcu_periph_clock_enable(RCU_RTC);
      // wait until shadow register are synced from the backup domain over the APB bus
      rtc_register_sync_wait();
      // wait for the changes are synced
      rtc_lwoff_wait();
      // prescale to 1 second
      cc++;
      rtc_prescaler_set(108); 
      rtc_lwoff_wait();
      rtc_interrupt_enable(RTC_INT_SECOND);
      rtc_lwoff_wait();
    }
    eclic_global_interrupt_enable();
    // ECLIC configure 
    eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL1_PRIO3);
    eclic_irq_enable(RTC_IRQn, 1, 0);
  }
  ------------------- END ----------------- */  

  int clk = 0;

  // The related data structure for the IMU, contains a vector of x, y, z floats
  mpu_vector_t vec, vec_temp;

  // counter variables 
  int c=0, tenmilli=0, s=0; 
  // display variables
  int hour = INITIAL_HOUR;
  int min = INITAL_MIN;

  // vector scales 
  int32_t v, v_temp;

  // inital vector resp. vecotr magnitude
  mpu6500_getAccel(&vec_temp);
  v_temp = sqrtf((vec_temp.x*vec_temp.x + vec_temp.y*vec_temp.y + vec_temp.z*vec_temp.z));

  //delay_until_1ms(1000);

  while(1){
    delay_until_1ms(10);              // 1 iteration takes 10 ms

    // display vector counter
    LCD_ShowNum(10, 10, c, 3, WHITE);                           
    //LCD_ShowNum(50, 10, (get_timer_value()%3600)%60, 8, WHITE);                           
    tenmilli++;

    if (!(tenmilli%10))                                          // every 100 ms..
      handle_imu(&vec, &vec_temp, &v, &v_temp, &c);        // ..handle imu

    if (!(tenmilli%15)){                                        // every 150 ms..
      butt(&hour, &min);                                  // ..handle buttons..
      displayClock(hour, min);                            // ..and display time
    }

    if (tenmilli > 99){
      tenmilli=0;
      s++;
      LCD_ShowNum(50, 50, s, 2, YELLOW);              // show seconds on clock
      if (!(s%15)) {
        // determine car motion 
        (c > MARGIN) ?  LCD_ShowStr(10, 50, "ON ", GREEN, OPAQUE) : LCD_ShowStr(10, 50, "Off", RED, OPAQUE);
        c=0;
      }
      if (s>59){
        s=0;
        incrementClock(&hour, &min);                  // increase clock by 1 minute
      }
    }
    LCD_Wait_On_Queue();
    while (!delay_finished());    // wait until iteration done
  }
}

/// @brief handles calculations for accelerometer & destroys old so it can be updated
/// @param pVec 
/// @param pVec_temp 
/// @param pV 
/// @param pV_temp 
/// @param pC 
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
