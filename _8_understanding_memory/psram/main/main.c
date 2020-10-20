#include <stdio.h>
#include <string.h>
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void print_memory()
{
  ESP_LOGI("memory","stack %d, total ram %d, internal memory %d, external memory %d\n",
     uxTaskGetStackHighWaterMark(NULL), heap_caps_get_free_size(MALLOC_CAP_8BIT),
     heap_caps_get_free_size(MALLOC_CAP_INTERNAL), heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
}

void app_main(void)
{
  print_memory();
  char * buffer = heap_caps_malloc(10,MALLOC_CAP_SPIRAM);
}
