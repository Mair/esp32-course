#ifndef WIFI_CONNECT_H
#define WIFI_CONNECT_H

void wifi_connect_init(void);
esp_err_t wifi_connect_sta(char *ssid, char *pass, int timeout, int failed_retry_count);
esp_err_t wifi_connect_sta_with_retry(char *ssid, char *pass, int timeout, int retry_count);
void wifi_connect_ap(const char *ssid, const char *pass);
void wifi_disconnect(void);
#endif