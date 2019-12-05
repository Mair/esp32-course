#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "esp32/rom/uart.h"


void app_main()
{
    esp_sleep_enable_timer_wakeup(5000000);
    printf("going for a nap\n");
    uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);

    int64_t before = esp_timer_get_time();

    esp_light_sleep_start();

    int64_t after = esp_timer_get_time();

    printf("napped for %lld\n", (after - before) / 1000);
}
