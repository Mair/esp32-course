```c
...
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// IDF 4 / 5
QueueHandle_t queue;

// IDF 4
// xQueueHandle queue;

void listenForHTTP(void *params)
{
  int count = 0;
  while (true)
  {
  
    long ok = xQueueSend(queue, &count, 1000 / portTICK_PERIOD_MS);
    if (ok)
    {
        ...
    }

  }
}

void task1(void *params)
{
  while (true)
  {
    int rxInt;
    if (xQueueReceive(queue, &rxInt, 5000 / portTICK_PERIOD_MS))
    {
      ...
    }
  }
}

void app_main(void)
{
  queue = xQueueCreate(3, sizeof(int));
  xTaskCreate(&listenForHTTP, "get http", 2048, NULL, 2, NULL);
  xTaskCreate(&task1, "do something with http", 2048, NULL, 1, NULL);
}```