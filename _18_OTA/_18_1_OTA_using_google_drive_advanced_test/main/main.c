#include <stdio.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "protocol_examples_common.h"

#define TAG "OTA"
xSemaphoreHandle ota_semaphore;

const int software_version = 106;
extern const uint8_t server_cert_pem_start[] asm("_binary_google_cer_start");

static esp_err_t validate_image_header(esp_app_desc_t *new_app_info)
{
  const esp_partition_t *running = esp_ota_get_running_partition();
  esp_app_desc_t running_app_info;
  if (esp_ota_get_partition_description(running, &running_app_info) == ESP_OK)
  {
    ESP_LOGI(TAG, "Running firmware version: %s", running_app_info.version);
  }

  if (memcmp(new_app_info->version, running_app_info.version, sizeof(new_app_info->version)) == 0)
  {
    ESP_LOGW(TAG, "Current running version is the same as a new. We will not continue the update.");
    return ESP_FAIL;
  }
  return ESP_OK;
}

void gracefully_err_out(char *err, esp_https_ota_handle_t https_ota_handle)
{
  ESP_LOGE(TAG, "%s", err);
  esp_https_ota_finish(https_ota_handle);
  vTaskDelete(NULL);
}

void run_ota(void *params)
{
  ESP_ERROR_CHECK(nvs_flash_init());
  tcpip_adapter_init();
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  while (true)
  {
    xSemaphoreTake(ota_semaphore, portMAX_DELAY);
    ESP_LOGI(TAG, "Invoking OTA");

    ESP_ERROR_CHECK(example_connect());

    esp_http_client_config_t clientConfig = {
        .url = "https://drive.google.com/uc?authuser=0&id=1HQln4CQI55Qh9Yi4Km1Nyl4Hklv9QuqK&export=download", // our ota location
        .cert_pem = (char *)server_cert_pem_start};

    esp_https_ota_config_t ota_config = {
        .http_config = &clientConfig,
    };
    esp_https_ota_handle_t https_ota_handle = NULL;
    esp_err_t err = esp_https_ota_begin(&ota_config, &https_ota_handle);
    if (err != ESP_OK)
    {
      ESP_LOGE(TAG, "ESP HTTPS OTA Begin failed");
      vTaskDelete(NULL);
    }

    esp_app_desc_t app_desc;
    if (esp_https_ota_get_img_desc(https_ota_handle, &app_desc) != ESP_OK)
    {
      ESP_LOGE(TAG, "esp_https_ota_read_img_desc failed");
      esp_https_ota_finish(https_ota_handle);
      continue;
    }

    if (validate_image_header(&app_desc) != ESP_OK)
    {
      ESP_LOGE(TAG, "validate_image_header failed");
      esp_https_ota_finish(https_ota_handle);
      continue;
    }

    while (esp_https_ota_perform(https_ota_handle) != ESP_ERR_HTTPS_OTA_IN_PROGRESS)
    {
      ESP_LOGD(TAG, "Image bytes read: %d", esp_https_ota_get_image_len_read(https_ota_handle));
    }

    while (1)
    {
      err = esp_https_ota_perform(https_ota_handle);
      if (err != ESP_ERR_HTTPS_OTA_IN_PROGRESS)
      {
        break;
      }
      // esp_https_ota_perform returns after every read operation which gives user the ability to
      // monitor the status of OTA upgrade by calling esp_https_ota_get_image_len_read, which gives length of image
      // data read so far.
      ESP_LOGD(TAG, "Image bytes read: %d", esp_https_ota_get_image_len_read(https_ota_handle));
    }

    ESP_LOGI(TAG, "ESP_HTTPS_OTA upgrade successful. Rebooting ...");
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    esp_restart();
  }
}

void on_button_pushed(void *params)
{
  xSemaphoreGiveFromISR(ota_semaphore, pdFALSE);
}

void app_main(void)
{
  printf("HAY!!! This is a new feature\n");
  ESP_LOGI("SOFTWARE VERSION", "we are running %d", software_version);
  gpio_config_t gpioConfig = {
      .pin_bit_mask = 1ULL << GPIO_NUM_0,
      .mode = GPIO_MODE_DEF_INPUT,
      .pull_up_en = GPIO_PULLUP_ENABLE,
      .pull_down_en = GPIO_PULLUP_DISABLE,
      .intr_type = GPIO_INTR_NEGEDGE};
  gpio_config(&gpioConfig);
  gpio_install_isr_service(0);
  gpio_isr_handler_add(GPIO_NUM_0, on_button_pushed, NULL);

  ota_semaphore = xSemaphoreCreateBinary();
  xTaskCreate(run_ota, "run_ota", 1024 * 8, NULL, 2, NULL);
}
