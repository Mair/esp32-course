#include <stdio.h>
#include "nvs_flash.h"
#include "wifi_connect.h"

void app_main(void)
{
    nvs_flash_init();
    wifi_connect_init();
    // esp_err_t err = wifi_connect_sta("POCO", "password", 10000);
}
