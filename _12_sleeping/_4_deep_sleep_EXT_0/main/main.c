#include <stdio.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"

#define WAKE_UP_GPIO GPIO_NUM_0

RTC_DATA_ATTR int wakeUpTimes = 0;

void app_main(void)
{
  rtc_gpio_deinit(WAKE_UP_GPIO);

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
  rtc_gpio_pulldown_en(WAKE_UP_GPIO);
  esp_sleep_enable_ext0_wakeup(WAKE_UP_GPIO, 0);
  printf("entering deep sleep\n");

  esp_deep_sleep_start();
}
