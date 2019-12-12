#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"

RTC_DATA_ATTR int timesWokenUp = 0;
void app_main(void)
{
  esp_sleep_enable_timer_wakeup(5 * 1000000);
  printf("going to sleep. woken up %d\n", timesWokenUp++);

  esp_deep_sleep_start();
}
