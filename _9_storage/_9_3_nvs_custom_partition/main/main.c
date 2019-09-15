#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#define TAG "NVS"

void app_main()
{
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  ESP_ERROR_CHECK(nvs_flash_init_partition("MyNvs"));

  nvs_handle handle;
  ESP_ERROR_CHECK(nvs_open_from_partition("MyNvs", "store", NVS_READWRITE, &handle));
 
  nvs_stats_t nvsStats;
  nvs_get_stats("MyNvs",&nvsStats);
  ESP_LOGI(TAG,"used: %d, free: %d, total: %d, namespace count: %d", nvsStats.used_entries,
  nvsStats.free_entries, nvsStats.total_entries, nvsStats.namespace_count);

  int32_t val = 0;
  esp_err_t result = nvs_get_i32(handle, "val", &val);
  switch (result)
  {
  case ESP_ERR_NVS_NOT_FOUND:
  case ESP_ERR_NOT_FOUND:
    ESP_LOGE(TAG, "Value not set yet");
    break;
  case ESP_OK:
    ESP_LOGI(TAG, "Value is %d", val);
    break;
  default:
    ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(result));
    break;
  }
  val++;
  ESP_ERROR_CHECK(nvs_set_i32(handle, "val", val));
  ESP_ERROR_CHECK(nvs_commit(handle));
  nvs_close(handle);
}