````c

int mqtt_send(const char *topic, const char *payload)
{
    return esp_mqtt_client_publish(client, topic, payload, strlen(payload), 1, 0);
}

static void test_send_messages(void *param)
{
    int count = 0;
    char message[50];
    while (true)
    {
        sprintf(message, "hello from ESP32 count %d", count++);
        mqtt_send("animal/message/from/esp32", message);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}```
````
