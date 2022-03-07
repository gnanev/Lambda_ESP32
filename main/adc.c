#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "driver/adc.h"

#include "adc.h"
#include "adc_correction.h"
#include "cj125_config.h"

ADC_ChannelRunning  adcRunning[ADC_CHANNELS_ACTIVE];
esp_timer_handle_t  hTimers[ADC_CHANNELS_ACTIVE];

static void adcTimerCallback(void* arg)
{   
    uint32_t  currentChannel = (uint32_t)arg;

    if (!xSemaphoreTake(adcRunning[currentChannel].mutex, 0))
        return;

    if (adcRunning[currentChannel].reset) {
        adcRunning[currentChannel].sum = 0;
        adcRunning[currentChannel].count = 0;
        adcRunning[currentChannel].reset = false;
    }

    adcRunning[currentChannel].sum += adc1_get_raw(adcChannels[currentChannel]);
    adcRunning[currentChannel].count++;

    xSemaphoreGive(adcRunning[currentChannel].mutex);
}

void ADC_Init()
{
    adc_set_clk_div(2);
    adc1_config_width(ADC_WIDTH_BIT_12);

    esp_timer_create_args_t periodic_timer_args;
    
    for(int i = 0; i < ADC_CHANNELS_ACTIVE; i++ ) {
        
        adc1_config_channel_atten(adcChannels[i], ADC_ATTEN_DB_0);

        adcRunning[i].sum = 0;
        adcRunning[i].count = 0;
        adcRunning[i].reset = false;
        adcRunning[i].mutex = xSemaphoreCreateMutex();

        if (adcRunning[i].mutex == NULL) {
            printf("Error: Mutex can't be created");
            abort();
        }

        periodic_timer_args.name = "adcTimer";
        periodic_timer_args.callback = adcTimerCallback;
        periodic_timer_args.arg = (void*)i;
        periodic_timer_args.dispatch_method = ESP_TIMER_TASK;
        periodic_timer_args.skip_unhandled_events = true;

        ESP_ERROR_CHECK( esp_timer_create(&periodic_timer_args, &hTimers[i]) );       
        ESP_ERROR_CHECK( esp_timer_start_periodic(hTimers[i], ADC_TIMER_PERIOD_US) );
    }
}

void ADC_EnableCorrection(int channel, bool enable)
{
    adcRunning[channel].correction = enable;
}

bool ADC_AquireReading(int channel, uint32_t* retVal)
{
    if (!xSemaphoreTake(adcRunning[channel].mutex, pdMS_TO_TICKS(10)))
        return false;

    if (adcRunning[channel].count == 0) {
        xSemaphoreGive(adcRunning[channel].mutex);
        return false;
    }

    uint32_t reading = adcRunning[channel].sum / adcRunning[channel].count;
    
    if (adcRunning[channel].correction) {
        reading = ADC_CORRECTION_MAP[reading];
    }

    *retVal = reading >> 2; // we need 10 bit precision
    adcRunning[channel].reset = true;

    xSemaphoreGive(adcRunning[channel].mutex);

    return true;
}