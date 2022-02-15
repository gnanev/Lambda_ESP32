#include <stdio.h>
#include "esp8266_wrapper.h"
#include "cj125.h"

#define STACK_SIZE 8096

void task_mainLoop(void *pvParameters)
{
     printf("Starting task_mainLoop\n");

    TickType_t last_wakeup = xTaskGetTickCount();

    while (1)
    {
        // uint16_t ret = cj125_Comm(CJ125_DIAG_REG_REQUEST);
        // printf("CJ125_DIAG_REG_REQUEST == %X \n", ret);
        vTaskDelayUntil(&last_wakeup, 100 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    printf("Starting app... \n");

    // gpio_enable(PIN_NUM_CLK, GPIO_MODE_OUTPUT);
    // gpio_enable(PIN_NUM_MOSI, GPIO_MODE_OUTPUT);
    // gpio_enable(PIN_NUM_MISO, GPIO_MODE_INPUT);

    // if (spi_bus_init(HSPI_HOST, PIN_NUM_CLK, PIN_NUM_MISO, PIN_NUM_MOSI))
    //     printf("SPI bus initialized \n");
    // else
    //     printf("SPI bus init ERROR \n");

    // if (spi_device_init(HSPI_HOST, PIN_NUM_CS))
    //     printf("SPI device initialized \n");
    // else
    //     printf("SPI device init ERROR \n");

    int result = cj125_Init();
    if (result != ESP_OK) {
        printf("CJ125 initialization failed. ERROR: %d \n", result);
    }
    else {
        printf("CJ125 initialized \n");
    }

    xTaskCreate(task_mainLoop, "task_mainLoop", STACK_SIZE, NULL, 2, NULL);
}
