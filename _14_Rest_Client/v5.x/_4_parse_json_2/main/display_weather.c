#include <stdio.h>
#include <string.h>
#include "FreeRTOS/FreeRTOS.h"
#include "freertos/queue.h"
#include "parse_weather.h"

QueueHandle_t display_queue;

void weather_display_task(void *params)
{
  display_queue = xQueueCreate(3, sizeof(weather_t));

  while (true)
  {
    weather_t weather;
    xQueueReceive(display_queue, &weather, portMAX_DELAY);
    printf("location %s %s %s\n", weather.location.name, weather.location.region, weather.location.country);
    printf("current %f %s\n", weather.current.temp_c, weather.current.condion_text);
    for (size_t i = 0; i < NUM_DAYS; i++)
    {
      printf("DAY %d %f %s\n",i, weather.forecast[i].avgtemp_c, weather.forecast[i].condition_text);
    }
  }
}

void update_display(weather_t *weather)
{
  xQueueSend(display_queue, weather, portMAX_DELAY);
}