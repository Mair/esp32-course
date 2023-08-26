```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_now.h"

#define TAG "ESP_NOW"

// IDF v4
// xQueueHandle message_received_queue;

/// IDF v5
QueueHandle_t message_received_queue;

typedef enum message_type_t
{
  BROADCAST,
  SEND_MESSAGE
} message_type_t;

typedef struct payload_t
{
  char message[100];
  message_type_t message_type;
} payload_t;

typedef struct payload_ext_t
{
  payload_t payload;
  uint8_t from_mac[6];
} payload_ext_t;

char *mac_to_str(char *buffer, uint8_t *mac)
{
  sprintf(buffer, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return buffer;
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
// IDF 4
// void on_receive(const uint8_t *mac_addr, const uint8_t *data, int data_len)
// IDF 5: esp_now_recv_info_t *mac_addr is OK because its the first entry in the structure
void on_receive(const esp_now_recv_info_t *mac_addr, const uint8_t *data, int data_len)
{
  if (data_len != sizeof(payload_t))
  {
    ESP_LOGE(TAG, "received incorrect payload");
    return;
  }
  payload_ext_t payload_ext;
  payload_t *payload = (payload_t *)data;
  payload_ext.payload = *payload;
  memcpy(payload_ext.from_mac, mac_addr, 6);
  xQueueSend(message_received_queue, &payload_ext, 0);
}

void broadcast_cb(TimerHandle_t xTimer)
{
  uint8_t bradcast_address[6];
  memset(bradcast_address, 0xFF, 6);
  payload_t payload = {
      .message = "Ping",
      .message_type = BROADCAST};

  ESP_ERROR_CHECK(esp_now_send(bradcast_address, (uint8_t *)&payload, sizeof(payload_t)));
}

void message_received_task(void *params)
{
  message_received_queue = xQueueCreate(10, sizeof(payload_ext_t));
  char buffer[13];
  while (true)
  {
    payload_ext_t payload_ext;
    xQueueReceive(message_received_queue, &payload_ext, portMAX_DELAY);
    switch (payload_ext.payload.message_type)
    {
    case BROADCAST:
      if (!esp_now_is_peer_exist(payload_ext.from_mac))
      {
        ESP_LOGI(TAG, "got broadcast from %s", mac_to_str(buffer, payload_ext.from_mac));
        esp_now_peer_info_t peer;
        memset(&peer, 0, sizeof(esp_now_peer_info_t));
        memcpy(peer.peer_addr, payload_ext.from_mac, 6);
        esp_now_add_peer(&peer);
      }
      break;

    default:
      break;
    }
  }
}

void app_main(void)
{

  nvs_flash_init();
  // idf 4.0
  //  tcpip_adapter_init();

  // idf 5.0
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_ERROR_CHECK(esp_now_init());
  ESP_ERROR_CHECK(esp_now_register_send_cb(on_sent));
  ESP_ERROR_CHECK(esp_now_register_recv_cb(on_receive));

  esp_now_peer_info_t peer;
  memset(&peer, 0, sizeof(esp_now_peer_info_t));
  memset(peer.peer_addr, 0xff, 6);
  esp_now_add_peer(&peer);

  TimerHandle_t timer_handler = xTimerCreate("broadcast", pdMS_TO_TICKS(10000), pdTRUE, NULL, broadcast_cb);
  xTimerStart(timer_handler, 0);

  xTaskCreate(message_received_task, "message_recived_task", 1024 * 2, NULL, 5, NULL);

  // ESP_ERROR_CHECK(esp_now_deinit());
  // ESP_ERROR_CHECK(esp_wifi_stop());
}
```
