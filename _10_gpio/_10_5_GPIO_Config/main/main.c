#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define PIN_SWITCH 15

xQueueHandle interputQueue;

static void IRAM_ATTR gpio_isr_handler(void *args)
{
    int pinNumber = (int)args;
    xQueueSendFromISR(interputQueue, &pinNumber, NULL);
}

void buttonPushedTask(void *params)
{
    int pinNumber, count = 0;
    while (true)
    {
        if (xQueueReceive(interputQueue, &pinNumber, portMAX_DELAY))
        {
            // disable the interrupt
            gpio_isr_handler_remove(pinNumber);

            // wait some time while we check for the button to be released
            do 
            {
                vTaskDelay(20 / portTICK_PERIOD_MS);
            } while(gpio_get_level(pinNumber) == 1);

            //do some work
            printf("GPIO %d was pressed %d times. The state is %d\n", pinNumber, count++, gpio_get_level(PIN_SWITCH));

            // re-enable the interrupt 
             gpio_isr_handler_add(pinNumber, gpio_isr_handler, (void *)pinNumber);

        }
    }
}

void app_main()
{
    // gpio_pad_select_gpio(PIN_SWITCH);
    // gpio_set_direction(PIN_SWITCH, GPIO_MODE_INPUT);
    // gpio_pulldown_en(PIN_SWITCH);
    // gpio_pullup_dis(PIN_SWITCH);
    // gpio_set_intr_type(PIN_SWITCH, GPIO_INTR_POSEDGE);

    gpio_config_t config;
    config.intr_type = GPIO_INTR_POSEDGE;
    config.mode = GPIO_MODE_INPUT;
    config.pull_down_en = true;
    config.pull_up_en = false;
    config.pin_bit_mask = ((1ULL<<PIN_SWITCH) | (1ULL<<5));

    gpio_config(&config);

    interputQueue = xQueueCreate(10, sizeof(int));
    xTaskCreate(buttonPushedTask, "buttonPushedTask", 2048, NULL, 1, NULL);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(PIN_SWITCH, gpio_isr_handler, (void *)PIN_SWITCH);
}
