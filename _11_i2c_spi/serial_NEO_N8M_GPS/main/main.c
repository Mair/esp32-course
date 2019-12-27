#include <stdio.h>
#include <string.h>
#include "driver/uart.h"
#include "esp_log.h"

#define BUF_SIZE (1024)

void app_main(void)
{
  const int uart_num = UART_NUM_1;
  uart_config_t uart_config = {
      .baud_rate = 9600,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
  ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
  ESP_ERROR_CHECK(uart_set_pin(uart_num, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
  uart_driver_install(uart_num, BUF_SIZE * 2, 0, 0, NULL, 0);
  
   uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
   memset(data,0, sizeof(BUF_SIZE));

   while (1) {
        // Read data from the UART
        int len = uart_read_bytes(uart_num, data, BUF_SIZE, 2000 / portTICK_RATE_MS);
        // Write data back to the UART
       printf("len = %d, data = %.*s\n",len, len, data);
        
    }
}
