#include <stdio.h>
#include <string.h>
#include "connect.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "cJSON.h"

static const char *TAG = "REST";
extern const uint8_t cert[] asm("_binary_cert_cer_start");

typedef struct chunk_payload_t
{
    uint8_t *buffer;
    int buffer_index;
} chunk_payload_t;

esp_err_t on_client_data(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
    {
        // ESP_LOGI(TAG, "Length=%d", evt->data_len);
        // printf("%.*s\n", evt->data_len, (char *)evt->data);
        chunk_payload_t *chunk_payload = evt->user_data;
        chunk_payload->buffer = realloc(chunk_payload->buffer, chunk_payload->buffer_index + evt->data_len + 1);
        memcpy(&chunk_payload->buffer[chunk_payload->buffer_index], (uint8_t *)evt->data, evt->data_len);
        chunk_payload->buffer_index = chunk_payload->buffer_index + evt->data_len;
        chunk_payload->buffer[chunk_payload->buffer_index] = 0;

        //printf("buffer******** %s\n",chunk_payload->buffer);
    }
    break;

    default:
        break;
    }
    return ESP_OK;
}

char *create_email_body()
{
    cJSON *jason_payload = cJSON_CreateObject();
    cJSON *personalizations = cJSON_CreateArray();
    cJSON_AddItemToObject(jason_payload, "personalizations", personalizations);

    cJSON *personalization_0 = cJSON_CreateObject();
    cJSON_AddItemToArray(personalizations, personalization_0);

    cJSON *to = cJSON_CreateArray();
    cJSON_AddItemToObject(personalization_0, "to", to);

    cJSON *to_0 = cJSON_CreateObject();
    cJSON_AddStringToObject(to_0, "email", "info@learnesp32.com");
    cJSON_AddStringToObject(to_0, "name", "Mair");
    cJSON_AddItemToArray(to, to_0);

    cJSON_AddStringToObject(personalization_0, "subject", "Hello, World!");

    cJSON *content = cJSON_CreateArray();
    cJSON_AddItemToObject(jason_payload, "content", content);

    cJSON *content_0 = cJSON_CreateObject();
    cJSON_AddStringToObject(content_0, "type", "text/html");
    cJSON_AddStringToObject(content_0, "value", "<h1>Heya!</h1>");
    cJSON_AddItemToArray(content, content_0);

    cJSON *from = cJSON_CreateObject();
    cJSON_AddItemToObject(jason_payload, "from", from);
    cJSON_AddStringToObject(from, "email", "info@learnesp32.com");
    cJSON_AddStringToObject(from, "name", "Mair");

    cJSON *reply_to = cJSON_CreateObject();
    cJSON_AddItemToObject(jason_payload, "reply_to", reply_to);
    cJSON_AddStringToObject(reply_to, "email", "info@learnesp32.com");
    cJSON_AddStringToObject(reply_to, "name", "Mair");

    char *payload_body = cJSON_Print(jason_payload);
    printf("body: %s\n", payload_body);
    cJSON_Delete(jason_payload);
    return payload_body;
}

void send_email()
{
    chunk_payload_t chunk_payload = {0};

    esp_http_client_config_t esp_http_client_config = {
        .url = "https://api.sendgrid.com/v3/mail/send",
        .method = HTTP_METHOD_POST,
        .event_handler = on_client_data,
        .user_data = &chunk_payload,
        .cert_pem = (char *)cert};
    esp_http_client_handle_t client = esp_http_client_init(&esp_http_client_config);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_header(client, "Authorization", "Bearer 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");
    char *payload_body = create_email_body();
    esp_http_client_set_post_field(client, payload_body, strlen(payload_body));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "HTTP GET status = %d\n", esp_http_client_get_status_code(client));
    }
    else
    {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    if (chunk_payload.buffer != NULL)
    {
        free(chunk_payload.buffer);
    }
    if (payload_body != NULL)
    {
        free(payload_body);
    }
    esp_http_client_cleanup(client);
    wifi_disconnect();
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init();
    ESP_ERROR_CHECK(wifi_connect_sta("POCO", "password", 10000));
    send_email();
}
