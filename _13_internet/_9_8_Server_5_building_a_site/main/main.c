#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event_loop.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "server.h"
#include "connect.h"
#include "lm75a.h"

#define TAG "DATA"
#define NUMBER CONFIG_TEL_NUMBER

xSemaphoreHandle connectionSemaphore;

void OnConnected(void *para)
{

  while (true)
  {
    if (xSemaphoreTake(connectionSemaphore, 10000 / portTICK_RATE_MS))
    {
      RegisterEndPoints();
      //esp_wifi_disconnect();
      xSemaphoreTake(connectionSemaphore, portMAX_DELAY);
    }
    else
    {
      ESP_LOGE(TAG, "Failed to connect. Retry in");
      for (int i = 0; i < 5; i++)
      {
        ESP_LOGE(TAG, "...%d", i);
        vTaskDelay(1000 / portTICK_RATE_MS);
      }
      esp_restart();
    }
  }
}

void app_main()
{
  esp_log_level_set(TAG, ESP_LOG_DEBUG);
  connectionSemaphore = xSemaphoreCreateBinary();
  wifiInit();
  xTaskCreate(&OnConnected, "handel comms", 1024 * 5, NULL, 5, NULL);
  xTaskCreate(&activateLM75A, "LM75A", 1024 * 3, NULL, 4, NULL);
}