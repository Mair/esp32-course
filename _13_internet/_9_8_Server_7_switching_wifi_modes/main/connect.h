#ifndef _CONNECT_H_
#define _CONNECT_H_
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event_loop.h"
void wifiInit(void *params);
extern xSemaphoreHandle initSemaphore;

#endif