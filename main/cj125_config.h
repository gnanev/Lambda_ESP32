#ifndef __CJ125_CONFIG_H__
#define __CJ125_CONFIG_H__

#include "driver/gpio.h"

#define PIN_NUM_MISO GPIO_NUM_19
#define PIN_NUM_MOSI GPIO_NUM_23
#define PIN_NUM_CLK  GPIO_NUM_18
#define PIN_NUM_CS   GPIO_NUM_5

#define CJ125_BUFF_LEN 2

#define CJ125_STACK_SIZE 8096

#endif // __CJ125_CONFIG_H__
