#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event_loop.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "cJSON.h"
#include "string.h"
#include "connect.h"

#define SSID CONFIG_WIFI_SSID
#define PASSWORD CONFIG_WIFI_PASSWORD

char *TAG = "CONNECTION";

extern xSemaphoreHandle connectionSemaphore;

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
  switch (event->event_id)
  {
  case SYSTEM_EVENT_STA_START:
    esp_wifi_connect();
    ESP_LOGI(TAG, "connecting...\n");
    break;

  case SYSTEM_EVENT_STA_CONNECTED:
    ESP_LOGI(TAG, "connected\n");
    break;

  case SYSTEM_EVENT_STA_GOT_IP:
    ESP_LOGI(TAG, "got ip\n");

    break;

  case SYSTEM_EVENT_STA_DISCONNECTED:
    ESP_LOGI(TAG, "disconnected\n");
    break;

  default:
    break;
  }
  return ESP_OK;
}

void connectSTA(char *ssid, char *pass)
{
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  // wifi_config_t wifi_config =
  //     {
  //         .sta = {
  //             .ssid = CONFIG_WIFI_SSID,
  //             .password = CONFIG_WIFI_PASSWORD}};
  wifi_config_t wifi_config;
  memset(&wifi_config, 0, sizeof(wifi_config));
  strcpy((char *)wifi_config.sta.ssid, ssid);
  strcpy((char *)wifi_config.sta.password, pass);
  esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
}

void connectAP()
{
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
  wifi_config_t wifi_config =
      {
          .ap = {
              .ssid = "my esp32",
              .password = "P@ssword",
              .max_connection = 4,
              .authmode = WIFI_AUTH_WPA_WPA2_PSK}};
  esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config);
}

void wifiInit(void *params)
{
  ESP_ERROR_CHECK(nvs_flash_init());
  // **** these 2 lines have moved from within the while loop to here. Its better here to ensure it only initializes once
  tcpip_adapter_init();
  ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
  while (true)
  {
    //*******the below 2 lines should be moved before the while loop
    //tcpip_adapter_init();
    //ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    if (xSemaphoreTake(initSemaphore, portMAX_DELAY))
    {

      nvs_handle_t nvs;
      nvs_open("wifiCreds", NVS_READWRITE, &nvs);

      size_t ssidLen, passLen;
      char *ssid = NULL, *pass = NULL;

      if (nvs_get_str(nvs, "ssid", NULL, &ssidLen) == ESP_OK)
      {
        if (ssidLen > 0)
        {
          ssid = malloc(ssidLen);
          nvs_get_str(nvs, "ssid", ssid, &ssidLen);
        }
      }

      if (nvs_get_str(nvs, "pass", NULL, &passLen) == ESP_OK)
      {
        if (passLen > 0)
        {
          pass = malloc(passLen);
          nvs_get_str(nvs, "pass", pass, &passLen);
        }
      }

      wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
      ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));

      if (ssid != NULL && pass != NULL)
      {
        connectSTA(ssid, pass);
      }
      else
      {
        connectAP();
      }

      ESP_ERROR_CHECK(esp_wifi_start());
      xSemaphoreGive(connectionSemaphore);

      if (ssid != NULL)
        free(ssid);
      if (pass != NULL)
        free(pass);
    }
  }
}
