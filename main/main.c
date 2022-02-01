#include <stdio.h>
#include "esp8266_wrapper.h"

#define STACK_SIZE 8096 //configMINIMAL_STACK_SIZE

#define PIN_NUM_MISO GPIO_NUM_19
#define PIN_NUM_MOSI GPIO_NUM_23
#define PIN_NUM_CLK  GPIO_NUM_18

//Define CJ125 registers used.
#define           CJ125_IDENT_REG_REQUEST             0x4800        /* Identify request, gives revision of the chip. */
#define           CJ125_DIAG_REG_REQUEST              0x7800        /* Dignostic request, gives the current status. */
#define           CJ125_INIT_REG1_REQUEST             0x6C00        /* Requests the first init register. */
#define           CJ125_INIT_REG2_REQUEST             0x7E00        /* Requests the second init register. */
#define           CJ125_INIT_REG1_MODE_CALIBRATE      0x569D        /* Sets the first init register in calibration mode. */
#define           CJ125_INIT_REG1_MODE_NORMAL_V8      0x5688        /* Sets the first init register in operation mode. V=8 amplification. */
#define           CJ125_INIT_REG1_MODE_NORMAL_V17     0x5689        /* Sets the first init register in operation mode. V=17 amplification. */
#define           CJ125_DIAG_REG_STATUS_OK            0x28FF        /* The response of the diagnostic register when everything is ok. */
#define           CJ125_DIAG_REG_STATUS_NOPOWER       0x2855        /* The response of the diagnostic register when power is low. */
#define           CJ125_DIAG_REG_STATUS_NOSENSOR      0x287F        /* The response of the diagnostic register when no sensor is connected. */
#define           CJ125_INIT_REG1_STATUS_0            0x2888        /* The response of the init register when V=8 amplification is in use. */
#define           CJ125_INIT_REG1_STATUS_1            0x2889        /* The response of the init register when V=17 amplification is in use. */

uint16_t COM_SPI(uint16_t TX_data) {

    uint8_t txBuff[4] = {0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t rxBuff[4] = {0xFF, 0xFF, 0xFF, 0xFF};

    txBuff[0] = (uint8_t)(TX_data & 0xFF);
    txBuff[1] = (uint8_t)(TX_data >> 8);

    printf("spi_transfer_pf started %x %x \n", txBuff[0], txBuff[1]);

    size_t ret = spi_transfer_pf(SPI2_HOST, 0, txBuff, rxBuff, 2);
    if (ret == 0) {
        printf("spi_transfer_pf failed \n");
        return 0;
    }
 printf("spi_transfer_pf OK \n");
  uint16_t Response = (rxBuff[1] << 8) + rxBuff[0];
 
  return Response;
}  

void task_mainLoop(void *pvParameters)
{
     printf("Starting task_mainLoop\n");

    TickType_t last_wakeup = xTaskGetTickCount();

    while (1)
    {
        uint16_t ret = COM_SPI(CJ125_DIAG_REG_REQUEST);
    
        printf("CJ125_DIAG_REG_REQUEST == %d \n", ret);

        vTaskDelayUntil(&last_wakeup, 500 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    printf("Starting app... \n");

    if (spi_bus_init(SPI2_HOST, PIN_NUM_CLK, PIN_NUM_MISO, PIN_NUM_MOSI))
        printf("SPI bus initialized \n");
    else
        printf("SPI bus init ERROR \n");

    if (spi_device_init(SPI2_HOST, 0))
        printf("SPI device initialized \n");
    else
        printf("SPI device init ERROR \n");

    xTaskCreate(task_mainLoop, "task_mainLoop", STACK_SIZE, NULL, 2, NULL);
}
