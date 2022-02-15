#include <sys/time.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/spi_common.h"

#include "cj125.h"
#include "cj125_config.h"

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

spi_device_handle_t h_cj125 = { 0 };
spi_transaction_t spi_trans = { 0 };
bool spi_transaction_not_initialized = true;
uint8_t txBuff[CJ125_BUFF_LEN];
uint8_t rxBuff[CJ125_BUFF_LEN];

void cj125_Task(void *pvParameters)
{
    printf("Starting cj125_Task\n");

    TickType_t last_wakeup = xTaskGetTickCount();

    while (1)
    {
        vTaskDelayUntil(&last_wakeup, 10 / portTICK_PERIOD_MS);
    }
}

int cj125_Init()
{
    spi_bus_config_t spi_bus_cfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096
    };

    int result = spi_bus_initialize(HSPI_HOST, &spi_bus_cfg, 1);

    if (result != ESP_OK)
        return result;

    spi_device_interface_config_t dev_cfg = {
        .clock_speed_hz = 1000000,
        .mode = 1,
        .spics_io_num = PIN_NUM_CS,
        .queue_size = 1,
        .flags = 0,
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .pre_cb = NULL,
		.post_cb = NULL,
        .duty_cycle_pos = 0,
        .cs_ena_posttrans = 0,
        .cs_ena_pretrans = 0   
    };

    result = spi_bus_add_device(HSPI_HOST, &dev_cfg, &h_cj125);

    if (result != ESP_OK)
        return result;
    
    spi_trans.tx_buffer = txBuff;
    spi_trans.rx_buffer = rxBuff;
    spi_trans.length = CJ125_BUFF_LEN * 8;

    xTaskCreate(cj125_Task, "cj125_Task", CJ125_STACK_SIZE, NULL, 2, NULL);

    return ESP_OK;
}

uint16_t cj125_Comm(uint16_t TX_data)
{
    txBuff[0] = (uint8_t)(TX_data >> 8);
    txBuff[1] = (uint8_t)(TX_data & 0xFF);
    
    if (spi_device_transmit(h_cj125, &spi_trans) != ESP_OK)
        return 0; // 0 is invalid value

    uint16_t response = (rxBuff[0] << 8) + rxBuff[1];
 
    return response;
}
