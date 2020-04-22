#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_now.h"

#define TAG "ESP_NOW"
xQueueHandle _payload_queue;

typedef enum message_type_t
{
  BROAD_CAST,
  BROAD_CAST_RESPONCE
} message_type_t;

typedef struct payload_t
{
  uint8_t to_mac[6];
  message_type_t message_type;
  uint8_t message[100];
} payload_t;

char *mac_to_str(char *buffer, uint8_t *mac)
{
  sprintf(buffer, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return buffer;
}

void message_task(void *params)
{
  payload_t payload;
  while (true)
  {
    xQueueReceive(_payload_queue, &payload, portMAX_DELAY);
  }
}

void on_sent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  char buffer[13];
  switch (status)
  {
  case ESP_NOW_SEND_SUCCESS:
    ESP_LOGI(TAG, "message sent to %s", mac_to_str(buffer, (uint8_t *)mac_addr));
    break;
  case ESP_NOW_SEND_FAIL:
    ESP_LOGE(TAG, "message sent to %s failed", mac_to_str(buffer, (uint8_t *)mac_addr));
    break;
  }
}

void on_receive(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{
  if (data_len != sizeof(payload_t))
  {
    ESP_LOGE(TAG, "bad payload");
    return;
  }
  payload_t *payload = data;
  xQueueReceive(_payload_queue, data, pdMS_TO_TICKS(500));
}

void app_main(void)
{
  _payload_queue = xQueueCreate(10, sizeof(payload_t));
  xTaskCreate(message_task, "message_task", 1024 * 2, NULL, 5, NULL);

  nvs_flash_init();
  tcpip_adapter_init();
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_ERROR_CHECK(esp_now_init());
  ESP_ERROR_CHECK(esp_now_register_send_cb(on_sent));
  ESP_ERROR_CHECK(esp_now_register_recv_cb(on_receive));

  payload_t payload = {
    .message_type = BROAD_CAST,
    .message = "ping",
  };

  uint8_t broadcast_mac[6] = {0xff,0xff,0xff,0xff,0xff,0xff};

  while(true)
  {
    ESP_ERROR_CHECK(esp_now_send(broadcast_mac, (uint8_t *)&payload, sizeof(payload_t)));
    vTaskDelay(pdMS_TO_TICKS(1000));
  }

  ESP_ERROR_CHECK(esp_now_deinit());
  ESP_ERROR_CHECK(esp_wifi_stop());
}
