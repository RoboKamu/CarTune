#include "gd32vf103.h"
#include "dac.h"

void DAC0powerUpInit(){
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_DAC);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
    dac_deinit();
    dac_trigger_disable(DAC0);
    dac_wave_mode_config(DAC0,DAC_WAVE_DISABLE);
    dac_output_buffer_enable(DAC0);
    dac_data_set(DAC0, DAC_ALIGN_12B_R, 0x07ff);
    dac_enable(DAC0);
}

void DAC0set(int value){
    dac_data_set(DAC0, DAC_ALIGN_12B_R, value);
}