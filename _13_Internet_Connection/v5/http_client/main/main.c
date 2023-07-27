#include <stdio.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "protocol_examples_common.h"
#include "esp_http_client.h"

esp_err_t client_event(esp_http_client_event_t *evt)
{
    if (evt->event_id == HTTP_EVENT_ON_DATA)
    {
        printf("%.*s\n", evt->data_len, (char *)evt->data);
    }
    return ESP_OK;
}

void app_main(void)
{
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();

    example_connect();

    esp_http_client_config_t esp_http_client_config = {
        .url = "https://google.com",
        .event_handler = client_event};
    esp_http_client_handle_t client = esp_http_client_init(&esp_http_client_config);
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}
