#include <stdio.h>
#include "nvs_flash.h"
#include "wifi_connect.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    nvs_flash_init();
    wifi_connect_init();

    while (true)
    {

        wifi_connect_sta("POCO", "password", 10000, 0);
        vTaskDelay(pdMS_TO_TICKS(15000));
        wifi_disconnect();

        wifi_connect_ap("myEsp32AP", "password");
        vTaskDelay(pdMS_TO_TICKS(15000));
        wifi_disconnect();
    }
}
