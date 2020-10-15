#include <stdio.h>
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"

#define TAG "UART"

#define TXD_PIN 4
#define RXD_PIN 5

#define RX_BUF_SIZE 1024
#define TX_BUF_SIZE 1024
#define PATTERN_LEN 3

QueueHandle_t uart_queue;

void uart_event_task(void *params)
{
  uart_event_t uart_event;
  uint8_t *received_buffer = malloc(RX_BUF_SIZE);
  size_t datalen;
  while (true)
  {
    if (xQueueReceive(uart_queue, &uart_event, portMAX_DELAY))
    {
      switch (uart_event.type)
      {
      case UART_DATA:
        ESP_LOGI(TAG, "UART_DATA");
        uart_read_bytes(UART_NUM_1, received_buffer, uart_event.size, portMAX_DELAY);
        printf("received: %.*s\n", uart_event.size, received_buffer);
        break;
      case UART_BREAK:
        ESP_LOGI(TAG, "UART_BREAK");
        break;
      case UART_BUFFER_FULL:
        ESP_LOGI(TAG, "UART_BUFFER_FULL");
        break;
      case UART_FIFO_OVF:
        ESP_LOGI(TAG, "UART_FIFO_OVF");
        uart_flush_input(UART_NUM_1);
        xQueueReset(uart_queue);
        break;
      case UART_FRAME_ERR:
        ESP_LOGI(TAG, "UART_FRAME_ERR");
        break;
      case UART_PARITY_ERR:
        ESP_LOGI(TAG, "UART_PARITY_ERR");
        break;
      case UART_DATA_BREAK:
        ESP_LOGI(TAG, "UART_DATA_BREAK");
        break;
      case UART_PATTERN_DET:
        ESP_LOGI(TAG, "UART_PATTERN_DET");
        uart_get_buffered_data_len(UART_NUM_1, &datalen);
        int pos = uart_pattern_pop_pos(UART_NUM_1);
        ESP_LOGI(TAG, "Detected %d pos %d", datalen, pos);
        uart_read_bytes(UART_NUM_1, received_buffer, datalen - PATTERN_LEN, pdMS_TO_TICKS(100));
        uint8_t pat[PATTERN_LEN + 1];
        memset(pat, 0, sizeof(pat));
        uart_read_bytes(UART_NUM_1, pat, PATTERN_LEN, pdMS_TO_TICKS(100));
        printf("data: %.*s === pattern: %s\n", datalen - PATTERN_LEN, received_buffer, pat);
        break;
      default:
        break;
      }
    }
  }
}

void app_main(void)
{
  uart_config_t uart_config = {
      .baud_rate = 9600,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
  uart_param_config(UART_NUM_1, &uart_config);
  uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  uart_driver_install(UART_NUM_1, RX_BUF_SIZE, TX_BUF_SIZE, 20, &uart_queue, 0);

  uart_enable_pattern_det_intr(UART_NUM_1, '+', PATTERN_LEN, 10000, 10, 10);
  uart_pattern_queue_reset(UART_NUM_1, 20);
  xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 10, NULL);
}
