#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "driver/rtc_io.h"



RTC_DATA_ATTR int timesWokenUp = 0;
void app_main(void)
{
  rtc_gpio_deinit(GPIO_NUM_25);
  rtc_gpio_deinit(GPIO_NUM_26);

  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
  rtc_gpio_pulldown_dis(GPIO_NUM_25);
  rtc_gpio_pulldown_dis(GPIO_NUM_26);
  rtc_gpio_pullup_en(GPIO_NUM_25);
  rtc_gpio_pullup_en(GPIO_NUM_26);

  esp_sleep_enable_ext1_wakeup(1ULL << GPIO_NUM_25 |  1ULL << GPIO_NUM_26,ESP_EXT1_WAKEUP_ALL_LOW);
  printf("going to sleep. woken up %d\n", timesWokenUp++);

  esp_deep_sleep_start();
}
