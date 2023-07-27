#include <stdio.h>
#include "connect.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "mdns.h"
#include "toggleLed.h"
#include "cJSON.h"

static const char *TAG = "SERVER";

static esp_err_t on_default_url(httpd_req_t *req)
{
  ESP_LOGI(TAG, "URL: %s", req->uri);
  httpd_resp_sendstr(req, "hello world");
  return ESP_OK;
}

static esp_err_t on_toggle_led_url(httpd_req_t *req)
{
  char buffer[100];
  memset(&buffer,0, sizeof(buffer));
  httpd_req_recv(req,buffer,req->content_len);
  cJSON *payload = cJSON_Parse(buffer);
  cJSON *is_on_json = cJSON_GetObjectItem(payload,"is_on");
  bool is_on = cJSON_IsTrue(is_on_json);
  cJSON_Delete(payload);
  toggle_led(is_on);
  httpd_resp_set_status(req,"204 NO CONTENT");
  httpd_resp_send(req,NULL,0);
  return ESP_OK;
}

static void init_server()
{
  httpd_handle_t server = NULL;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  ESP_ERROR_CHECK(httpd_start(&server, &config));

  httpd_uri_t default_url = {
      .uri = "/",
      .method = HTTP_GET,
      .handler = on_default_url};
  httpd_register_uri_handler(server, &default_url);

  httpd_uri_t toggle_led_url = {
      .uri = "/api/toggle-led",
      .method = HTTP_POST,
      .handler = on_toggle_led_url};
  httpd_register_uri_handler(server, &toggle_led_url);
}

void start_mdns_service()
{
  mdns_init();
  mdns_hostname_set("my-esp32");
  mdns_instance_name_set("LEARN esp32 thing");
}

void app_main(void)
{
  ESP_ERROR_CHECK(nvs_flash_init());
  init_led();
  wifi_init();
  ESP_ERROR_CHECK(wifi_connect_sta("POCO", "password", 10000));

  start_mdns_service();
  init_server();
}
