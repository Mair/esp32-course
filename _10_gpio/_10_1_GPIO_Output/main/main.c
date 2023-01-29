#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define PIN 2

void app_main(void)
{
  //gpio_pad_select_gpio(PIN);
  gpio_set_direction(PIN, GPIO_MODE_OUTPUT);
  int isOn = 0;
  while (true)
  {
    isOn = !isOn;
    gpio_set_level(PIN, isOn);
    vTaskDelay(1000 / portTICK_PERIOD_MS);    
  }
}

// 3.3 - 2.75 = 0.55

// v = I * R
// R = V / I
// R = 0.55 / 0.02
// R = 30 OHMS