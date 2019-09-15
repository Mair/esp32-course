#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event_loop.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"

#define SSID CONFIG_WIFI_SSID
#define PASSWORD CONFIG_WIFI_PASSWORD

xSemaphoreHandle onConnectionHandler;

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
  switch (event->event_id)
  {
  case SYSTEM_EVENT_STA_START:
    esp_wifi_connect();
    printf("connecting...\n");
    break;

  case SYSTEM_EVENT_STA_CONNECTED:
    printf("connected\n");
    break;

  case SYSTEM_EVENT_STA_GOT_IP:
    printf("got ip\n");
    xSemaphoreGive(onConnectionHandler);
    break;

  case SYSTEM_EVENT_STA_DISCONNECTED:
    printf("disconnected\n");
    break;

  default:
    break;
  }
  return ESP_OK;
}

void wifiInit()
{
  ESP_ERROR_CHECK(nvs_flash_init());
  tcpip_adapter_init();
  ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

  wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_start());

  wifi_config_t wifi_config =
      {
          .sta = {
              .ssid = CONFIG_WIFI_SSID,
              .password = CONFIG_WIFI_PASSWORD}};
  esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
  ESP_ERROR_CHECK(esp_wifi_start())
}

void onConnected(void *param)
{
  while (true)
  {
    if (xSemaphoreTake(onConnectionHandler, 10 * 1000 / portTICK_PERIOD_MS))
    {
      //do something
      xSemaphoreTake(onConnectionHandler, portMAX_DELAY);
    } 
    else 
    {
      ESP_LOGE("CONNECTION", "Could not connect");
      esp_restart();
    }
  }
}

void app_main()
{
  onConnectionHandler = xSemaphoreCreateBinary();
  wifiInit();
  xTaskCreate(&onConnected, "On Connected", 1024 * 4, NULL, 5, NULL);
}