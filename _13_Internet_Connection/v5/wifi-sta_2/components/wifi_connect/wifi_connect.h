#ifndef WIFI_CONNECT_H
#define WIFI_CONNECT_H

void wifi_connect_init(void);
esp_err_t wifi_connect_sta(char *ssid, char *pass, int timeout);

#endif