#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_http_client.h"
#include "esp_log.h"

#define TAG "HTTPCLIENT"

char *buffer = NULL;
int indexBuffer = 0;

void OnGotData(char * incomingBuffer);

esp_err_t clientEventHandler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA Len=%d", evt->data_len);
        printf("%.*s\n", evt->data_len, (char *)evt->data);
        if (buffer == NULL)
        {
            buffer = (char *)malloc(evt->data_len);
        }
        else
        {
            buffer = (char *)realloc(buffer, evt->data_len + indexBuffer);
        }
        memcpy(&buffer[indexBuffer], evt->data, evt->data_len);
        indexBuffer += evt->data_len;
        break;

    case HTTP_EVENT_ON_FINISH:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
        buffer = (char *)realloc(buffer, indexBuffer + 1);
        memcpy(&buffer[indexBuffer], "\0", 1);
        ESP_LOGI(TAG,"%s", buffer);
        OnGotData(buffer);
        free(buffer);
        break;

    default:
        break;
    }

    return ESP_OK;
}

void OnGotData(char * incomingBuffer)
{

}

void fetch(char *url)
{
    esp_http_client_config_t clientConfig = {
        .url = url,
        .event_handler = clientEventHandler};

    esp_http_client_handle_t client = esp_http_client_init(&clientConfig);
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
}