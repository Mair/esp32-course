#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

QueueHandle_t queue;
// xQueueHandle queue;

void listenForHTTP(void *params)
{
  int count = 0;
  while (true)
  {
    count++;
    printf("received http message\n");
    long ok = xQueueSend(queue, &count, 1000 / portTICK_PERIOD_MS);
    if (ok)
    {
      printf("added message to queue\n");
    }
    else
    {
      printf("failed to add message to queue\n");
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void task1(void *params)
{
  while (true)
  {
    int rxInt;
    if (xQueueReceive(queue, &rxInt, 5000 / portTICK_PERIOD_MS))
    {
      printf("doing something with http %d\n", rxInt);
    }
  }
}

void app_main(void)
{
  queue = xQueueCreate(3, sizeof(int));
  xTaskCreate(&listenForHTTP, "get http", 2048, NULL, 2, NULL);
  xTaskCreate(&task1, "do something with http", 2048, NULL, 1, NULL);
}