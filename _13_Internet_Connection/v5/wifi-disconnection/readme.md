```c 

void event_handler(void *event_handler_arg, esp_event_base_t event_base,
                   int32_t event_id, void *event_data)
{
    switch (event_id)
    {
   ...
    case WIFI_EVENT_STA_DISCONNECTED:
    {
      ...

        if (attempt_reconnect)
        {
            if (wifi_event_sta_disconnected->reason == WIFI_REASON_NO_AP_FOUND ||
                wifi_event_sta_disconnected->reason == WIFI_REASON_ASSOC_LEAVE ||
                wifi_event_sta_disconnected->reason == WIFI_REASON_AUTH_EXPIRE)
            {
                if (disconnection_err_count++ < 5)
                {
                    vTaskDelay(pdMS_TO_TICKS(5000));
                    esp_wifi_connect();
                    break;
                }
                else
                {
                    ESP_LOGE(TAG, "WIFI retries exceeded");
                }
            }
        }

        xEventGroupSetBits(wifi_events, DISCONNECTED);
        break;
    }
   ...
}

void wifi_disconnect(void)
{
    attempt_reconnect = false;
    esp_wifi_stop();
    esp_netif_destroy(esp_netif);
}

```
