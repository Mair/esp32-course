```c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define PIN_SWITCH 15
// idf V4
//  xQueueHandle interruptQueue;

// idf V4 / V5
QueueHandle_t interruptQueue;

static void IRAM_ATTR gpio_isr_handler(void *args)
{
    int pinNumber = (int)args;
    xQueueSendFromISR(interruptQueue, &pinNumber, NULL);
}

void buttonPushedTask(void *params)
{
    int pinNumber, count = 0;
    while (true)
    {
        if (xQueueReceive(interruptQueue, &pinNumber, portMAX_DELAY))
        {
            printf("GPIO %d was pressed %d times. The state is %d\n", pinNumber, count++, gpio_get_level(PIN_SWITCH));
        }
    }
}

void app_main()
{
    // gpio_pad_select_gpio(PIN_SWITCH);
    gpio_set_direction(PIN_SWITCH, GPIO_MODE_INPUT);
    gpio_pulldown_en(PIN_SWITCH);
    gpio_pullup_dis(PIN_SWITCH);
    gpio_set_intr_type(PIN_SWITCH, GPIO_INTR_POSEDGE);

    interruptQueue = xQueueCreate(10, sizeof(int));
    xTaskCreate(buttonPushedTask, "buttonPushedTask", 2048, NULL, 1, NULL);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(PIN_SWITCH, gpio_isr_handler, (void *)PIN_SWITCH);
}
```