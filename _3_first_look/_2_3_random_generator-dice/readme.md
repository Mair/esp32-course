```c
#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// for idf version 4.x use #include "esp_system.h"
#include "esp_random.h"
#include "math.h"

#define TAG "DICE"

int dice_role()
{
  int random = esp_random();
  int positiveNumber = abs(random);
  int diceNumber = (positiveNumber % 6) + 1;
  return diceNumber;
}

void app_main(void)
{
  while (1)
  {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "random number %d", dice_role());
  }
}
```