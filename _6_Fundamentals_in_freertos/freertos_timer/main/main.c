#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "esp_system.h"

void on_timer(TimerHandle_t xTimer)
{
  printf("time hit %lld\n", esp_timer_get_time() /1000);
}
void app_main(void)
{
  printf("app started %lld\n", esp_timer_get_time() / 1000);
  TimerHandle_t xTimer = xTimerCreate("my timer", pdMS_TO_TICKS(1000),true,NULL, on_timer);
  xTimerStart(xTimer,0);

}
