```c
#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "DELAY"

void app_main(void)
{
  int i = 0;
  while (1)
  {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "in loop %d", i++);
  }
}
```