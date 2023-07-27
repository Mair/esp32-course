#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "driver/gpio.h"
#include "app.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "cJSON.h"

#define BTN 0

static xSemaphoreHandle btn_sem;

static void IRAM_ATTR on_btn_pushed(void *arg)
{
  xSemaphoreGiveFromISR(btn_sem, NULL);
}

static void btn_push_task(void *params)
{
  while (true)
  {
    xSemaphoreTake(btn_sem, portMAX_DELAY);
    cJSON *payload = cJSON_CreateObject();
    cJSON_AddBoolToObject(payload, "btn_state", gpio_get_level(BTN));
    char *message = cJSON_Print(payload);
    send_web_socket_message(message);
    cJSON_Delete(payload);
    free(message);
  }
}

void init_btn(void)
{
  xTaskCreate(btn_push_task, "btn_push_task", 2048, NULL, 5, NULL);
  btn_sem = xSemaphoreCreateBinary();
  gpio_pad_select_gpio(BTN);
  gpio_set_direction(BTN, GPIO_MODE_INPUT);
  gpio_set_intr_type(BTN, GPIO_INTR_ANYEDGE);
  gpio_install_isr_service(0);
  gpio_isr_handler_add(BTN, on_btn_pushed, NULL);
}
