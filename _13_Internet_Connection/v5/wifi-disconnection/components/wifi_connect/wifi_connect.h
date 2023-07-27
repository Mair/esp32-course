#ifndef WIFI_CONNECT_H
#define WIFI_CONNECT_H

#include "esp_err.h"
void wifi_connect_init(void);
esp_err_t wifi_connect_sta(char *ssid, char *pass, int timeout);
void wifi_connect_ap(const char *ssid, const char *pass);
void wifi_disconnect(void);

#endif