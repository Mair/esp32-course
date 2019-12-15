#include <stdio.h>
#include "esp_log.h"

void app_main(void)
{
  // dont forget to set the minmum level in the menuconfig
  esp_log_level_set("LOGGING", ESP_LOG_INFO);
  int myNumber = 5;
  ESP_LOGE("LOGGING", "This is an error %d", myNumber++);
  ESP_LOGW("LOGGING", "This is an warning %d", myNumber++);
  ESP_LOGI("LOGGING", "This is an info %d", myNumber++);
  ESP_LOGD("LOGGING", "This is an debug %d", myNumber++);
  ESP_LOGV("LOGGING", "This is an verbose %d", myNumber++);
}