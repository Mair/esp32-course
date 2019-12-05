#include <string.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "esp_http_client.h"

esp_err_t clientEventHandler(esp_http_client_event_t *evt)
{
  switch (evt->event_id)
  {
  case HTTP_EVENT_ON_DATA:
    printf("%.*s\n", evt->data_len, (char *)evt->data);
  default:
    break;
  }
  return ESP_OK;
}

void app_main(void)
{
  ESP_ERROR_CHECK(nvs_flash_init());
  tcpip_adapter_init();
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  ESP_ERROR_CHECK(example_connect());

  esp_http_client_config_t clientConfig = {
      .url = "http://google.com",
      .event_handler = clientEventHandler};

  esp_http_client_handle_t client = esp_http_client_init(&clientConfig);
  esp_err_t err = esp_http_client_perform(client);
  esp_http_client_cleanup(client);
}
