#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"

void app_main(void)
{
  ledc_timer_config_t ledc_timer = {
      .duty_resolution = LEDC_TIMER_10_BIT, // resolution of PWM duty
      .freq_hz = 5000,                      // frequency of PWM signal
      .speed_mode = LEDC_LOW_SPEED_MODE,   // timer mode
      .timer_num = LEDC_TIMER_0,            // timer index
      .clk_cfg = LEDC_AUTO_CLK,             // Auto select the source clock
  };
  // Set configuration of timer0 for high speed channels
  ledc_timer_config(&ledc_timer);

  ledc_channel_config_t ledc_channel = {
      .channel = LEDC_CHANNEL_0,
      .duty = 0,
      .gpio_num = 2,
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .hpoint = 0,
      .timer_sel = LEDC_TIMER_0};
  ledc_channel_config(&ledc_channel);

  // while (true)
  // {
  //   for (int i = 0; i < 1023; i++)
  //   {
  //     ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, i);
  //     ledc_update_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_0);
  //     vTaskDelay(10 / portTICK_PERIOD_MS);
  //   }

  //   for (int i = 1023; i >= 0; i--)
  //   {
  //     ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, i);
  //     ledc_update_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_0);
  //     vTaskDelay(10 / portTICK_PERIOD_MS);
  //   }
  // }

  ledc_fade_func_install(0);

  while(true)
  {
    ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_0,1023,3000,LEDC_FADE_NO_WAIT);
    ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_0,0,3000,LEDC_FADE_NO_WAIT);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("got here\n");
  }
}
