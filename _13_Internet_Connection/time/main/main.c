#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "esp_sntp.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "esp_log.h"

#define TAG "NTP_TIME"

void print_time(long time, const char *message)
{
  setenv("TZ", "EST-10EDT-11,M10.5.0/02:00:00,M3.5.0/03:00:00", 1);
  tzset();
  struct tm *timeinfo = localtime(&time);

  char buffer[50];
  strftime(buffer, sizeof(buffer), "%c", timeinfo);
  ESP_LOGI(TAG, "message: %s: %s", message, buffer);
}

void on_got_time(struct timeval *tv)
{
  printf("secs %lld\n", tv->tv_sec);
  print_time(tv->tv_sec, "time at callback");

  for (int i = 0; i < 5; i++)
  {
    time_t now = 0;
    time(&now);
    print_time(now, "in loop");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }

  esp_restart();
}

void app_main(void)
{
  time_t now = 0;
  time(&now);
  print_time(now, "Beginning of application");

  nvs_flash_init();
  tcpip_adapter_init();
  esp_event_loop_create_default();
  example_connect();

  sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED);
  sntp_setservername(0, "pool.ntp.org");
  sntp_init();
  sntp_set_time_sync_notification_cb(on_got_time);
}
