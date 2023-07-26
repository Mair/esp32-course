```c
#include <stdio.h>
#include "esp_log.h"

void app_main(void)
{
  esp_log_level_set("LOG", ESP_LOG_INFO);
  ESP_LOGE("LOG", "This is an error");
  ESP_LOGW("LOG", "This is a warning");
  ESP_LOGI("LOG", "This is an info");
  ESP_LOGD("LOG", "This is a Debug");
  ESP_LOGV("LOG", "This is Verbose");

  int number = 0;
  ESP_LOGE("TAG 2", "This is an error %d", number++);
  ESP_LOGW("TAG 2", "This is a warning %d", number++);
  ESP_LOGI("TAG 2", "This is an info %d", number++);
  ESP_LOGD("TAG 2", "This is a Debug %d", number++);
  ESP_LOGV("TAG 2", "This is Verbose %d", number++);
}
```