#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "driver/rtc_io.h"

#define BUTTON_1 GPIO_NUM_25
#define BUTTON_2 GPIO_NUM_26

RTC_DATA_ATTR int timesWokenUp = 0;
void app_main(void)
{
  rtc_gpio_deinit(BUTTON_1);
  rtc_gpio_deinit(BUTTON_2);
  /// general gpio functions using the pin.

  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
  rtc_gpio_pullup_dis(BUTTON_1);
  rtc_gpio_pulldown_en(BUTTON_1);
  rtc_gpio_pullup_dis(BUTTON_2);
  rtc_gpio_pulldown_en(BUTTON_2);

  uint64_t mask = (1ULL << BUTTON_1) | (1ULL << BUTTON_2);
  esp_sleep_enable_ext1_wakeup(mask, ESP_EXT1_WAKEUP_ANY_HIGH);
  printf("going to sleep. woken up %d\n", timesWokenUp++);

  esp_deep_sleep_start();
}
