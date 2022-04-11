#include <stdio.h>
#include <string.h>
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

char *TAG = "CONNECTION";
static EventGroupHandle_t Wifi_events;
const int CONNECTED_GOT_IP = BIT0;
const int DISCONNECTED = BIT1;

static esp_netif_t *esp_netif;

static char *print_disconnection_error(wifi_err_reason_t reason)
{
    switch (reason)
    {
    case WIFI_REASON_UNSPECIFIED:
        return "WIFI_REASON_UNSPECIFIED";
    case WIFI_REASON_AUTH_EXPIRE:
        return "WIFI_REASON_AUTH_EXPIRE";
    case WIFI_REASON_AUTH_LEAVE:
        return "WIFI_REASON_AUTH_LEAVE";
    case WIFI_REASON_ASSOC_EXPIRE:
        return "WIFI_REASON_ASSOC_EXPIRE";
    case WIFI_REASON_ASSOC_TOOMANY:
        return "WIFI_REASON_ASSOC_TOOMANY";
    case WIFI_REASON_NOT_AUTHED:
        return "WIFI_REASON_NOT_AUTHED";
    case WIFI_REASON_NOT_ASSOCED:
        return "WIFI_REASON_NOT_ASSOCED";
    case WIFI_REASON_ASSOC_LEAVE:
        return "WIFI_REASON_ASSOC_LEAVE";
    case WIFI_REASON_ASSOC_NOT_AUTHED:
        return "WIFI_REASON_ASSOC_NOT_AUTHED";
    case WIFI_REASON_DISASSOC_PWRCAP_BAD:
        return "WIFI_REASON_DISASSOC_PWRCAP_BAD";
    case WIFI_REASON_DISASSOC_SUPCHAN_BAD:
        return "WIFI_REASON_DISASSOC_SUPCHAN_BAD";
    case WIFI_REASON_IE_INVALID:
        return "WIFI_REASON_IE_INVALID";
    case WIFI_REASON_MIC_FAILURE:
        return "WIFI_REASON_MIC_FAILURE";
    case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
        return "WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT";
    case WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT:
        return "WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT";
    case WIFI_REASON_IE_IN_4WAY_DIFFERS:
        return "WIFI_REASON_IE_IN_4WAY_DIFFERS";
    case WIFI_REASON_GROUP_CIPHER_INVALID:
        return "WIFI_REASON_GROUP_CIPHER_INVALID";
    case WIFI_REASON_PAIRWISE_CIPHER_INVALID:
        return "WIFI_REASON_PAIRWISE_CIPHER_INVALID";
    case WIFI_REASON_AKMP_INVALID:
        return "WIFI_REASON_AKMP_INVALID";
    case WIFI_REASON_UNSUPP_RSN_IE_VERSION:
        return "WIFI_REASON_UNSUPP_RSN_IE_VERSION";
    case WIFI_REASON_INVALID_RSN_IE_CAP:
        return "WIFI_REASON_INVALID_RSN_IE_CAP";
    case WIFI_REASON_802_1X_AUTH_FAILED:
        return "WIFI_REASON_802_1X_AUTH_FAILED";
    case WIFI_REASON_CIPHER_SUITE_REJECTED:
        return "WIFI_REASON_CIPHER_SUITE_REJECTED";
    case WIFI_REASON_INVALID_PMKID:
        return "WIFI_REASON_INVALID_PMKID";
    case WIFI_REASON_BEACON_TIMEOUT:
        return "WIFI_REASON_BEACON_TIMEOUT";
    case WIFI_REASON_NO_AP_FOUND:
        return "WIFI_REASON_NO_AP_FOUND";
    case WIFI_REASON_AUTH_FAIL:
        return "WIFI_REASON_AUTH_FAIL";
    case WIFI_REASON_ASSOC_FAIL:
        return "WIFI_REASON_ASSOC_FAIL";
    case WIFI_REASON_HANDSHAKE_TIMEOUT:
        return "WIFI_REASON_HANDSHAKE_TIMEOUT";
    case WIFI_REASON_CONNECTION_FAIL:
        return "WIFI_REASON_CONNECTION_FAIL";
    case WIFI_REASON_AP_TSF_RESET:
        return "WIFI_REASON_AP_TSF_RESET";
    case WIFI_REASON_ROAMING:
        return "WIFI_REASON_ROAMING";
    }
    return "";
}

static void event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case SYSTEM_EVENT_STA_START:
        ESP_LOGI(TAG, "connecting...\n");
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        ESP_LOGI(TAG, "connected\n");
        break;
    case IP_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "got ip\n");
        xEventGroupSetBits(Wifi_events, CONNECTED_GOT_IP);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
    {
        wifi_event_sta_disconnected_t *wifi_event_sta_disconnected = event_data;
        // from wifi_err_reason_t
        ESP_LOGW(TAG, "disconnected code %d: %s \n", wifi_event_sta_disconnected->reason,
                 print_disconnection_error(wifi_event_sta_disconnected->reason));
        if (wifi_event_sta_disconnected->reason != WIFI_REASON_ASSOC_LEAVE)
        {
            esp_wifi_connect();
            break;
        }
        xEventGroupSetBits(Wifi_events, DISCONNECTED);
    }
    break;
    case SYSTEM_EVENT_AP_STACONNECTED:
    {
        wifi_event_ap_staconnected_t *connected_event = (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(TAG, "station " MACSTR " joined, AID=%d", MAC2STR(connected_event->mac), connected_event->aid);
    }
    break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
    {
        wifi_event_ap_stadisconnected_t *disconnected_event = (wifi_event_ap_stadisconnected_t *)event_data;
        ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d", MAC2STR(disconnected_event->mac), disconnected_event->aid);
    }
    break;
    case SYSTEM_EVENT_AP_START:
        ESP_LOGI(TAG, "AP Starting...\n");
        break;

    case SYSTEM_EVENT_AP_STOP:
        ESP_LOGI(TAG, "AP Stopping...\n");
        esp_netif_destroy(esp_netif);
        break;

    default:
        break;
    }
}

void wifi_init(void)
{

    esp_netif_init();
    esp_event_loop_create_default();
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, event_handler, NULL));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
}

esp_err_t wifi_connect_sta(const char *ssid, const char *pass, int timeout)
{
    Wifi_events = xEventGroupCreate();
    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config_t));
    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char *)wifi_config.sta.password, pass, sizeof(wifi_config.sta.password) - 1);

    esp_netif = esp_netif_create_default_wifi_sta();

    // for static ip...
    // esp_netif_dhcpc_stop(esp_netif);
    // esp_netif_ip_info_t ip_info;
    // ip_info.ip.addr = ipaddr_addr("192.168.0.222");
    // ip_info.gw.addr = ipaddr_addr("192.168.0.1");
    // ip_info.netmask.addr = ipaddr_addr("255.255.255.0");
    // esp_netif_set_ip_info(esp_netif, &ip_info);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    EventBits_t result = xEventGroupWaitBits(Wifi_events, CONNECTED_GOT_IP | DISCONNECTED, pdTRUE, pdFALSE, pdMS_TO_TICKS(timeout));
    if (result == CONNECTED_GOT_IP)
        return 0;
    return -1;
}

void wifi_connect_ap(const char *ssid, const char *pass)
{
    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config_t));
    strncpy((char *)wifi_config.ap.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char *)wifi_config.ap.password, pass, sizeof(wifi_config.sta.password) - 1);
    wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    wifi_config.ap.max_connection = 4;
    wifi_config.ap.beacon_interval = 100;

    esp_netif = esp_netif_create_default_wifi_ap();
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void wifi_disconnect(void)
{
    ESP_LOGI(TAG, "**********DISCONNECTING*********");
    esp_wifi_disconnect();
    esp_wifi_stop();
    ESP_LOGI(TAG, "***********DISCONNECTING COMPLETE*********");
}
