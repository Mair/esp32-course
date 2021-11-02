#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
//#include "esp_event_loop.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"

char *TAG = "CONNECTION";

xSemaphoreHandle connectionSemaphore;

static void event_handler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
  switch (event_id)
  {
  case SYSTEM_EVENT_STA_START:
    esp_wifi_connect(); 
    ESP_LOGI(TAG,"connecting...\n");
    break; 

  case SYSTEM_EVENT_STA_CONNECTED:
    ESP_LOGI(TAG,"connected\n");
    break;

  case IP_EVENT_STA_GOT_IP:
    ESP_LOGI(TAG,"got ip\n");
    xSemaphoreGive(connectionSemaphore);
    break;

  case SYSTEM_EVENT_STA_DISCONNECTED:
    ESP_LOGI(TAG,"disconnected\n");
    break;

  default:
    break;
  }
  //return ESP_OK;
}

void wifiInit()
{
  ESP_ERROR_CHECK(nvs_flash_init());

  //tcpip_adapter_init();
  //ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_t *netif_sta = esp_netif_create_default_wifi_sta();

  // esp_netif_dhcpc_stop(netif_sta);
  // esp_netif_ip_info_t ip_info;
  // ip_info.ip.addr = ipaddr_addr("192.168.0.222");
  // ip_info.gw.addr = ipaddr_addr("192.168.0.1");
  // ip_info.netmask.addr = ipaddr_addr("255.255.255.0");
  // esp_netif_set_ip_info(netif_sta, &ip_info);
  // //esp_netif_dhcpc_start(netif_sta);

  wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));

  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT,ESP_EVENT_ANY_ID,event_handler,NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT,IP_EVENT_STA_GOT_IP,event_handler,NULL));

  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));


  wifi_config_t wifi_config =
      {
          .sta = {
              .ssid = CONFIG_WIFI_SSID,
              .password = CONFIG_WIFI_PASSWORD
              }};
  esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
  ESP_ERROR_CHECK(esp_wifi_start());
}

void OnConnected(void *para)
{
  while (true)
  {
    if (xSemaphoreTake(connectionSemaphore, 10000 / portTICK_RATE_MS))
    {
      ESP_LOGI(TAG, "connected");
      
      
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
  xTaskCreate(&OnConnected, "handel comms", 1024 * 3, NULL, 5, NULL);
}