#include <stdio.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"

RTC_DATA_ATTR int wakeUpTimes = 0;

void app_main(void)
{
  if (wakeUpTimes == 0)
  {
    printf("starting first time\n");
  }
  else
  {
    printf("waking up %d\n", wakeUpTimes);
  }
  wakeUpTimes++;
  int wakeup_time_sec = 5;
  esp_sleep_enable_timer_wakeup(wakeup_time_sec * 1000000);
  printf("entering deep sleep\n");
  
  esp_deep_sleep_start();
  
}
