#include <stdio.h>
#include "esp_log.h"
#include  "freertos/FreeRTOS.h"
#include  "freertos/task.h"

void app_main(void)
{
  while (true)
  {
    printf("Hello world!\n");
    ESP_LOGI("BLE_DONGLE","TEST INFO");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  
  
  
}
