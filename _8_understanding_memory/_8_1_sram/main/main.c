#include <stdio.h>
#include <string.h>
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#define TAG "Memory"

void app_main()
{
  ESP_LOGI(TAG, "xPortGetFreeHeapSize %dk = DRAM", xPortGetFreeHeapSize());

  int DRam = heap_caps_get_free_size(MALLOC_CAP_8BIT);
  int IRam = heap_caps_get_free_size(MALLOC_CAP_32BIT) - heap_caps_get_free_size(MALLOC_CAP_8BIT);

  ESP_LOGI(TAG, "DRAM \t\t %d", DRam);
  ESP_LOGI(TAG, "IRam \t\t %d", IRam);  
}