```c 

void event_handler(void *event_handler_arg, esp_event_base_t event_base,
                   int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
        esp_wifi_connect();
        break;
    case WIFI_EVENT_STA_CONNECTED:
        ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");
        xEventGroupSetBits(wifi_events, DISCONNECTED);
        break;
    case IP_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");
        xEventGroupSetBits(wifi_events, CONNECTED);
        break;
    default:
        break;
    }
}

void wifi_connect_init(void)
{
    ...
}

esp_err_t wifi_connect_sta(char *ssid, char *pass, int timeout)
{
    wifi_events = xEventGroupCreate();
    esp_netif = esp_netif_create_default_wifi_sta();
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    // for static ip...
    // ESP_ERROR_CHECK(esp_netif_dhcpc_stop(esp_netif));
    // esp_netif_ip_info_t ip_info;
    // ip_info.ip.addr = ipaddr_addr("192.168.43.150");
    // ip_info.gw.addr = ipaddr_addr("192.168.43.1");
    // ip_info.netmask.addr = ipaddr_addr("255.255.255.0");
    // ESP_ERROR_CHECK(esp_netif_set_ip_info(esp_netif, &ip_info));

    wifi_config_t wifi_config = {};
    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char *)wifi_config.sta.password, pass, sizeof(wifi_config.sta.password) - 1);
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    EventBits_t result = xEventGroupWaitBits(wifi_events, CONNECTED | DISCONNECTED, true, false, pdMS_TO_TICKS(timeout));
    if (result == CONNECTED)
        return ESP_OK;
    return ESP_FAIL;
}
```