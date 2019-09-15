#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_http_client.h"
#include "esp_log.h"
#include "cJSON.h"
#include "fetch.h"

#define TAG "HTTPCLIENT"

char *buffer = NULL;
int indexBuffer = 0;

esp_err_t clientEventHandler(esp_http_client_event_t *evt)
{
    struct FetchParms *fetchParms = (struct FetchParms *)evt->user_data;
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
        if (fetchParms->OnGotData != NULL)
        {
            fetchParms->OnGotData(buffer, fetchParms->message);
        }
        free(buffer);
        buffer = NULL;
        indexBuffer = 0;
        break;

    default:
        break;
    }

    return ESP_OK;
}

void fetch(char *url, struct FetchParms *fetchParms)
{
    esp_http_client_config_t clientConfig = {
        .url = url,
        .event_handler = clientEventHandler,
        .user_data = fetchParms};

    esp_http_client_handle_t client = esp_http_client_init(&clientConfig);
    if (fetchParms->method == Post)
    {
        esp_http_client_set_method(client, HTTP_METHOD_POST);
    }
    for (int i = 0; i < fetchParms->headerCount; i++)
    {
        esp_http_client_set_header(client, fetchParms->header[i].key, fetchParms->header[i].val);
    }
    if(fetchParms->body != NULL)
    {
        esp_http_client_set_post_field(client, fetchParms->body, strlen(fetchParms->body));
    }
    esp_err_t err = esp_http_client_perform(client);
    fetchParms->status = esp_http_client_get_status_code(client); 
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "HTTP GET status = %d",
                 esp_http_client_get_status_code(client));
    }
    else
    {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}
