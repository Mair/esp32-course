#include <stdio.h>
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"

#define TXD_PIN 4
#define RXD_PIN 5

#define RX_BUF_SIZE 1024

void app_main(void)
{
  uart_config_t uart_config = {
    .baud_rate = 9600,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
  };
  uart_param_config(UART_NUM_1 ,&uart_config);
  uart_set_pin(UART_NUM_1,TXD_PIN,RXD_PIN,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE);
  uart_driver_install(UART_NUM_1,RX_BUF_SIZE,0,0,NULL,0);

  char message[] = "ping";
  printf("sending: %s\n", message);
  uart_write_bytes(UART_NUM_1,message, sizeof(message));

  char incoming_message[RX_BUF_SIZE];
  memset(incoming_message, 0 , sizeof(incoming_message));
  uart_read_bytes(UART_NUM_1,(uint8_t *) incoming_message,RX_BUF_SIZE,pdMS_TO_TICKS(500));
  printf("received: %s\n",incoming_message);
  

}
