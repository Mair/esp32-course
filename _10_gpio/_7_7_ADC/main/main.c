#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"

void app_main()
{
   adc1_config_width(ADC_WIDTH_BIT_12);
   adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_0);
   while (true)
   {
       int val = adc1_get_raw(ADC1_CHANNEL_0);
       printf("value is %d\n", val);
       vTaskDelay(1000 / portTICK_PERIOD_MS);
   }
   
}
