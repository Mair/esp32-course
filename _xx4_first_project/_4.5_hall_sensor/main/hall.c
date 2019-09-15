#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
 
 
uint32_t hall_sens_read();

void app_main()
{
     while (1) {
        printf("ESP32 on chip Hall sensor = %d\n", hall_sens_read());
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}