#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void task1(void * params)
{
  while (true)
  {
    printf("reading temperature from %s\n", (char *) params);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void task2(void * params) 
{
  while (true)
  {
    printf("reading humidity from %s\n", (char *) params);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void app_main(void)
{
   xTaskCreate(&task1, "temperature reading", 2048, "task 1", 2, NULL);
   xTaskCreate(&task2, "humidity reading", 2048, "task 2", 2, NULL);
}