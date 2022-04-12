#include <stdio.h>
#include "connect.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "nvs_flash.h"

static const char *TAG = "REST";

esp_err_t on_client_data(esp_http_client_event_t * evt)
{
    switch(evt->event_id)
    {
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG,"Length=%d",evt->data_len);
            printf("%.*s\n",evt->data_len, (char *)evt->data);
        break;

        default:
        break;
    }
    return ESP_OK;
}

void fetch_quote()
{
    esp_http_client_config_t esp_http_client_config = {
        .url = "http://quotes.rest/qod",
        .method = HTTP_METHOD_GET,
        .event_handler = on_client_data};
    esp_http_client_handle_t client = esp_http_client_init(&esp_http_client_config);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "HTTP GET status = %d, content_length = %d",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
    }
    else
    {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
    wifi_disconnect();
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init();
    ESP_ERROR_CHECK(wifi_connect_sta("POCO", "password", 10000));
    fetch_quote();
}
