#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "connect.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void initialize_nvs(void)
{
    // initialize the default NVS partition
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

void aTask(void *params)
{
    wifi_connect_ap("my-esp-ssid", "password");
    // vTaskDelay(portMAX_DELAY);
    for (size_t i = 45; i > 0; i--)
    {
        printf("disconnecting ap in %d\n", i);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    wifi_disconnect();
    esp_err_t err = wifi_connect_sta("POCO", "password", 10000);
    if (err)
    {
        ESP_LOGE("TEST", "FAILED TO CONNECT 2");
    }
    for (size_t i = 5; i > 0; i--)
    {
        printf("disconnecting sta in %d\n", i);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    wifi_disconnect();
    vTaskDelete(NULL);
}

void app_main(void)
{
    initialize_nvs();
    wifi_init();
    xTaskCreate(aTask, "aTask", 1024 * 5, NULL, 5, NULL);
}
