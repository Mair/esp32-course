#include <stdio.h>
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"

void timer_callback(void *param)
{
  static bool on;
  on = !on;
  
  gpio_set_level(GPIO_NUM_4, on);
}

void app_main(void)
{
  gpio_pad_select_gpio(GPIO_NUM_4);
  gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);

  const esp_timer_create_args_t my_timer_args = {
      .callback = &timer_callback,
      .name = "a timer"};
  esp_timer_handle_t timer_handler;
  ESP_ERROR_CHECK(esp_timer_create(&my_timer_args, &timer_handler));
  ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handler, 50));
  //or only fire once
  //ESP_ERROR_CHECK(esp_timer_start_once(timer_handler, 94));

  while (true)
  {
    //other stuff here
    esp_timer_dump(stdout);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
