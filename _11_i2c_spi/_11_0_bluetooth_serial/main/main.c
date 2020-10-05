#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
///warning New version requires that you set the watchdog timer to a higher value
void app_main(void)
{
  while (true)
  {
    printf("Hello world!\n");
    ESP_LOGI("LOG", "AN INFO");
    ESP_LOGW("LOG", "A WARNING");
    ESP_LOGE("LOG", "AN ERROR");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
