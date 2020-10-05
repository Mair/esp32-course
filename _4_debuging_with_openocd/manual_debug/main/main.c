#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

void callingFunk()
{
  char buffer[30];
  memset(buffer, 'x', sizeof(buffer));
  buffer[29] = 0;
  printf("buffer is %s\n", buffer);
  for (int i = 0; i < 30; i++)
  {
    int num1 = rand() % 5;
    int known = 100;
    printf("result is %d\n", known / num1);
  }
}

void task1(void *params)
{
  callingFunk();
  vTaskDelete(NULL);
}

void app_main(void)
{
  xTaskCreate(task1, "task1,", 1024 * 2, NULL, 5, NULL);
}
