#include <stdio.h>
#include "nvs_flash.h"
#include "wifi_connect.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static char *TAG = "MQTT";
static void mqtt_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void test_send_messages(void *param);
int mqtt_send(const char *topic, const char *payload, bool retain);

static esp_mqtt_client_handle_t client;

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_connect_init();
    ESP_ERROR_CHECK(wifi_connect_sta("POCO", "password", 10000));
    esp_mqtt_client_config_t esp_mqtt_client_config = {
        .broker.address.uri = "mqtt://mqtt.eclipseprojects.io:1883",
        .session.last_will = {
            .topic = "animal/on-chip-death",
            .msg = "I died ☠️",
            .msg_len = strlen("I died ☠️"),
            .qos = 1,
        }};
    client = esp_mqtt_client_init(&esp_mqtt_client_config);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

    // xTaskCreate(test_send_messages, "test_send_messages", 1024 * 2, NULL, 5, NULL);
    vTaskDelay(pdMS_TO_TICKS(3000));
    mqtt_send("animal/a-message", "a message", true);

    // esp_mqtt_client_stop(client);
}

static void mqtt_event_handler(void *event_handler_arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        esp_mqtt_client_subscribe(client, "animal/mammal/cat/felix", 1);
        esp_mqtt_client_subscribe(client, "animal/reptiles/+/slither", 1);
        esp_mqtt_client_subscribe(client, "animal/fish/#", 1);
        esp_mqtt_client_subscribe(client, "animal/configuration", 1);

        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED");
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED");
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED");
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("topic: %.*s\n", event->topic_len, event->topic);
        printf("message: %.*s\n", event->data_len, event->data);

        if (strncmp(event->topic, "animal/configuration", strlen("animal/configuration")) == 0)
        {
            printf("got config data %.*s\n", event->data_len, event->data);
        }

        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "ERROR %s", strerror(event->error_handle->esp_transport_sock_errno));
        break;
    default:
        break;
    }
}

int mqtt_send(const char *topic, const char *payload, bool retain)
{
    return esp_mqtt_client_publish(client, topic, payload, strlen(payload), 1, retain);
}

static void test_send_messages(void *param)
{
    int count = 0;
    char message[50];
    while (true)
    {
        sprintf(message, "hello from ESP32 count %d", count++);
        mqtt_send("animal/message/from/esp32", message, false);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}