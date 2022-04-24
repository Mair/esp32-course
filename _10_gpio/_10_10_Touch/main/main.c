#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/touch_pad.h"

#define TOUCH_PAD_GPIO13_CHANNEL TOUCH_PAD_NUM4

void app_main()
{
    touch_pad_init();
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
    touch_pad_config(TOUCH_PAD_GPIO13_CHANNEL, -1);
    touch_pad_filter_start(10);

    uint16_t val;
    uint16_t touch_filter_value = 0;
    uint16_t touch_rawValue = 0;

    while (true)
    {
        touch_pad_read_raw_data(TOUCH_PAD_GPIO13_CHANNEL, &touch_rawValue);
        touch_pad_read_filtered(TOUCH_PAD_GPIO13_CHANNEL, &touch_filter_value);
        touch_pad_read(TOUCH_PAD_GPIO13_CHANNEL, &val);
        printf("val = %d raw = %d filtered = %d\n", val, touch_rawValue, touch_filter_value);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
