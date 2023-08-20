```c
int mqtt_send(const char *topic, const char *payload, bool retain)
{
    return esp_mqtt_client_publish(client, topic, payload, strlen(payload), 1, retain);
}
```
