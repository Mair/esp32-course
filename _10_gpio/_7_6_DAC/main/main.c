#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/dac.h"

void app_main()
{
    dac_output_enable(DAC_CHANNEL_1);
    dac_out_voltage(DAC_CHANNEL_1,195);
}