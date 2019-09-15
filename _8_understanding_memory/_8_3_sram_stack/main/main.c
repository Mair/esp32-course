#include <stdio.h>
#include <string.h>
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "Memory"

void aTask(void *param)
{
  int stackmem = uxTaskGetStackHighWaterMark(NULL);
  ESP_LOGI(TAG, "stack space = %d", stackmem);

  char buffer [6500];
  memset(&buffer,1,sizeof(buffer));

  while(true) vTaskDelay(1000);
}
void app_main()
{
  ESP_LOGI(TAG, "xPortGetFreeHeapSize %dk = DRAM", xPortGetFreeHeapSize());

  int DRam = heap_caps_get_free_size(MALLOC_CAP_8BIT);
  int IRam = heap_caps_get_free_size(MALLOC_CAP_32BIT) - heap_caps_get_free_size(MALLOC_CAP_8BIT);

  ESP_LOGI(TAG, "DRAM \t\t %d", DRam);
  ESP_LOGI(TAG, "IRam \t\t %d", IRam);
  int free = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
  ESP_LOGI(TAG, "free = %d", free);

  int stackmem = uxTaskGetStackHighWaterMark(NULL);
  ESP_LOGI(TAG, "stack space = %d", stackmem);
  
  xTaskCreate(&aTask,"a task",8000,NULL,1, NULL);
  
}