#include <stdio.h>
#include "connect.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char* TAG="wifi_connect";

void wifi_connect(void *params)
{
   wifi_connect_ap("my-esp-ssid","password");
   for (size_t i = 45; i > 0; i--)
   {
      printf("disconnecting ap %d\n",i);
      vTaskDelay(pdMS_TO_TICKS(1000));
   }
   wifi_disconnect();

   esp_err_t err = wifi_connect_sta("POCO", "password", 10000);
   if(err){
      ESP_LOGE(TAG, "Failed to connect");
      vTaskDelete(NULL);
   }
   for (size_t i = 5; i > 0; i--)
   {
      printf("disconnecting in %d\n",i);
      vTaskDelay(pdMS_TO_TICKS(1000));
   }
   
   wifi_disconnect();
   vTaskDelete(NULL);
}

void app_main(void)
{
   ESP_ERROR_CHECK(nvs_flash_init());
   wifi_init();
   xTaskCreate(wifi_connect, "wifi_connect", 1024 * 5, NULL, 5, NULL);
}
