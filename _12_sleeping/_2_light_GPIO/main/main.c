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
#include "driver/rtc_io.h"


#define INPUT_PIN 0

void app_main()
{
    gpio_pad_select_gpio(INPUT_PIN);
    gpio_set_direction(INPUT_PIN, GPIO_MODE_INPUT);
    gpio_wakeup_enable(INPUT_PIN, GPIO_INTR_LOW_LEVEL);

    esp_sleep_enable_gpio_wakeup();
    esp_sleep_enable_timer_wakeup(5000000);

    while (true)
    {
        if (rtc_gpio_get_level(INPUT_PIN) == 0)
        {
            printf("please release button\n");
            do
            {
                vTaskDelay(pdMS_TO_TICKS(10));
            } while (rtc_gpio_get_level(INPUT_PIN) == 0);
        }

        printf("going for a nap\n");
        uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);

        int64_t before = esp_timer_get_time();

        esp_light_sleep_start();

        int64_t after = esp_timer_get_time();

        esp_sleep_wakeup_cause_t reason = esp_sleep_get_wakeup_cause();

        printf("napped for %lld, reason was %s\n", (after - before) / 1000,
               reason == ESP_SLEEP_WAKEUP_TIMER ? "timer" : "button");
    }
}
