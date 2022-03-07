#ifndef __CJ125_CONFIG_H__
#define __CJ125_CONFIG_H__

#include "driver/gpio.h"
#include "driver/adc.h"

#define PIN_NUM_MISO GPIO_NUM_19
#define PIN_NUM_MOSI GPIO_NUM_23
#define PIN_NUM_CLK  GPIO_NUM_18
#define PIN_NUM_CS   GPIO_NUM_5

#define CJ125_BUFF_LEN 2

#define CJ125_STACK_SIZE 8096

#define PWM_MODE        LEDC_LOW_SPEED_MODE
#define PWM_TIMER       LEDC_TIMER_0
#define PWM_DUTY_RES    LEDC_TIMER_8_BIT
#define PWM_CHANNEL     LEDC_CHANNEL_0
#define PWM_OUTPUT_IO   GPIO_NUM_13

#endif // __CJ125_CONFIG_H__
