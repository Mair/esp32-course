#include <string.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define TAG "OTA"

const int softwareVersion = 4;

extern const uint8_t server_cert_pem_start[] asm("_binary_google_cer_start");

esp_err_t clientEventHandler(esp_http_client_event_t *evt)
{
  return ESP_OK;
}

void runOTA(void *params)
{
  esp_http_client_config_t clientConfig = {
      .url = "https://drive.google.com/uc?authuser=0&id=1o9H6tAGXjF_UqUFskVH3CzQpkLTQ95xx&export=download",
      .event_handler = clientEventHandler,
      .cert_pem = (char *)server_cert_pem_start};

  esp_err_t ret = esp_https_ota(&clientConfig);
  if (ret == ESP_OK)
  {
    ESP_LOGI(TAG, "OTA flash successull for versio %d", softwareVersion);
    ESP_LOGW(TAG, "new version will run in 5 secs");
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    esp_restart();
  }
  else
    ESP_LOGE(TAG, "Firmware upgrade failed");

  while (true)
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void app_main(void)
{
  ESP_ERROR_CHECK(nvs_flash_init());
  tcpip_adapter_init();
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  ESP_ERROR_CHECK(example_connect());
  printf("This is version %d\n", softwareVersion);
  vTaskDelay(5000 / portTICK_PERIOD_MS);
  xTaskCreate(runOTA, "OTA", 1024 * 8, NULL, 2, NULL);
}
