#include <stdio.h>
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "string.h"
#include "freertos/queue.h"

#define TAG "UART"
#define TXD_PIN (GPIO_NUM_4)
#define RXD_PIN (GPIO_NUM_5)

#define PATTERN_LENGTH 3
#define RX_BUF_SIZE 1024
#define TCX_BUF_SIZE 1024
xQueueHandle uartQueue;

void uart_event_task(void *params)
{
  uart_event_t event;
  size_t buffered_size;
  uint8_t *receive_buffer = (uint8_t *)malloc(RX_BUF_SIZE);
  while (true)
  {
   
    if (xQueueReceive(uartQueue, (void *)&event, portMAX_DELAY))
    {
       memset(receive_buffer, 0, sizeof(RX_BUF_SIZE));
      switch (event.type)
      {
      case UART_DATA:
        uart_read_bytes(UART_NUM_1, receive_buffer, event.size, portMAX_DELAY);
        printf("received %s\n", receive_buffer);
        break;
      case UART_FIFO_OVF:
        ESP_LOGI(TAG, "hw fifo overflow");
        uart_flush_input(UART_NUM_1);
        xQueueReset(uartQueue);
        break;
      case UART_BUFFER_FULL:
        ESP_LOGI(TAG, "ring buffer full");
        // If buffer full happened, you should consider encreasing your buffer size
        // As an example, we directly flush the rx buffer here in order to read more data.
        uart_flush_input(UART_NUM_1);
        xQueueReset(uartQueue);
        break;
      //Event of UART RX break detected
      case UART_BREAK:
        ESP_LOGI(TAG, "uart rx break");
        break;
      //Event of UART parity check error
      case UART_PARITY_ERR:
        ESP_LOGI(TAG, "uart parity error");
        break;
      //Event of UART frame error
      case UART_FRAME_ERR:
        ESP_LOGI(TAG, "uart frame error");
        break;
      //UART_PATTERN_DET
      case UART_PATTERN_DET:
        uart_get_buffered_data_len(UART_NUM_1, &buffered_size);
        int pos = uart_pattern_pop_pos(UART_NUM_1);
        ESP_LOGI(TAG, "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffered_size);
        if (pos == -1)
        {
          // There used to be a UART_PATTERN_DET event, but the pattern position queue is full so that it can not
          // record the position. We should set a larger queue size.
          // As an example, we directly flush the rx buffer here.
          uart_flush_input(UART_NUM_1);
        }
        else
        {
          uart_read_bytes(UART_NUM_1, receive_buffer, pos, 100 / portTICK_PERIOD_MS);
          uint8_t pat[PATTERN_LENGTH + 1];
          memset(pat, 0, sizeof(pat));
          uart_read_bytes(UART_NUM_1, pat, PATTERN_LENGTH, 100 / portTICK_PERIOD_MS);
          ESP_LOGI(TAG, "read data: %.*s",buffered_size -PATTERN_LENGTH, receive_buffer);
          ESP_LOGI(TAG, "read pat : %s", pat);
        }
        break;
      //Others
      default:
        ESP_LOGI(TAG, "uart event type: %d", event.type);
        break;
      }
    }
  }
}

void app_main(void)
{
  const uart_config_t uart_config = {
      .baud_rate = 9600,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
  uart_param_config(UART_NUM_1, &uart_config);
  uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  // We won't use a buffer for sending data.
  uart_driver_install(UART_NUM_1, RX_BUF_SIZE, TCX_BUF_SIZE, 20, &uartQueue,0);

  uart_enable_pattern_det_intr(UART_NUM_1, '+', PATTERN_LENGTH, 10000, 10, 10);
  uart_pattern_queue_reset(UART_NUM_1, 20);
  xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, NULL);
}
