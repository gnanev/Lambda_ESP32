#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "cj125.h"

#define STACK_SIZE 8096

#define SAMPLES_COUNT 256
#define GET_UNIT(x) ((x>>3) & 0x1)

void task_mainLoop(void *pvParameters)
{
    printf("Starting task_mainLoop\n");
    
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
} 

void app_main(void)
{
    printf("Starting app... \n");

    int result = cj125_Init();
    if (result != ESP_OK) {
        printf("CJ125 initialization failed. ERROR: %d \n", result);
    }
    else {
        printf("CJ125 initialized \n");
    }

    xTaskCreate(task_mainLoop, "task_mainLoop", STACK_SIZE, NULL, 2, NULL);
}
