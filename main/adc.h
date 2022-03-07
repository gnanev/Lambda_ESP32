#ifndef __ADC_H__
#define __ADC_H__

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define ADC_CHANNELS_ACTIVE 2
#define ADC_TIMER_PERIOD_US 5000

static adc1_channel_t adcChannels[ADC_CHANNELS_ACTIVE] = {ADC2_CHANNEL_0, ADC2_CHANNEL_3};

typedef struct ADC_ChannelRunning {
    uint32_t            sum;
    uint32_t            count;
    bool                reset;
    bool                correction;
    SemaphoreHandle_t   mutex;
} ADC_ChannelRunning;

void    ADC_Init();
void    ADC_EnableCorrection(int channel, bool enable);
bool    ADC_AquireReading(int channel, uint32_t* retVal);

#endif  //__ADC_H__
