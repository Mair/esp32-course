#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"

void xx(void *params)
{
}

void app_main(void)
{
  ledc_timer_config_t timer = {
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .duty_resolution = LEDC_TIMER_10_BIT,
      .timer_num = LEDC_TIMER_0,
      .freq_hz = 5000,
      .clk_cfg = LEDC_AUTO_CLK};

  ledc_timer_config(&timer);

  ledc_channel_config_t channel = {
      .gpio_num = 4,
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .channel = LEDC_CHANNEL_0,
      .timer_sel = LEDC_TIMER_0,
      .duty = 0,
      .hpoint = 0};
  ledc_channel_config(&channel);

  ledc_fade_func_install(0);
  for (int i = 0; i < 1024; i++)
  {
    ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_0,i,0);
    // ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, i);
    // ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }

  while(true)
  {
    ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_0,0,1000,LEDC_FADE_WAIT_DONE);
    ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_0,1024,1000,LEDC_FADE_WAIT_DONE);
  }

}
