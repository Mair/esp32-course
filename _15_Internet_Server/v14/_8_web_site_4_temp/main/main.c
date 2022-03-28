#include <stdio.h>
#include "connect.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "mdns.h"
#include "toggleLed.h"
#include "cJSON.h"
#include "pushBtn.h"
#include "esp_spiffs.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "SERVER";
#define MAX_APs 20

static httpd_handle_t server = NULL;

static esp_err_t on_default_url(httpd_req_t *req)
{
  ESP_LOGI(TAG, "URL: %s", req->uri);

  esp_vfs_spiffs_conf_t esp_vfs_spiffs_conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true};
  esp_vfs_spiffs_register(&esp_vfs_spiffs_conf);

  char path[600];
  if (strcmp(req->uri, "/") == 0)
    strcpy(path, "/spiffs/index.html");
  else
    sprintf(path, "/spiffs%s", req->uri);
  char *ext = strrchr(path, '.');
  // if (ext == NULL)
  // {
  //   httpd_resp_set_status(req, "301 Moved Permanently");
  //   httpd_resp_set_hdr(req, "Location", "/");
  //   httpd_resp_send(req, NULL, 0);
  // }
  if (strcmp(ext, ".css") == 0)
    httpd_resp_set_type(req, "text/css");
  if (strcmp(ext, ".js") == 0)
    httpd_resp_set_type(req, "text/javascript");
  if (strcmp(ext, ".png") == 0)
    httpd_resp_set_type(req, "image/png");

  FILE *file = fopen(path, "r");
  if (file == NULL)
  {
    httpd_resp_send_404(req);
    esp_vfs_spiffs_unregister(NULL);
    return ESP_OK;
  }

  char lineRead[256];
  while (fgets(lineRead, sizeof(lineRead), file))
  {
    httpd_resp_sendstr_chunk(req, lineRead);
  }
  httpd_resp_sendstr_chunk(req, NULL);

  esp_vfs_spiffs_unregister(NULL);
  return ESP_OK;
}

static esp_err_t on_toggle_led_url(httpd_req_t *req)
{
  char buffer[100];
  memset(&buffer, 0, sizeof(buffer));
  httpd_req_recv(req, buffer, req->content_len);
  cJSON *payload = cJSON_Parse(buffer);
  cJSON *is_on_json = cJSON_GetObjectItem(payload, "is_on");
  bool is_on = cJSON_IsTrue(is_on_json);
  cJSON_Delete(payload);
  toggle_led(is_on);
  httpd_resp_set_status(req, "204 NO CONTENT");
  httpd_resp_send(req, NULL, 0);
  return ESP_OK;
}

static esp_err_t on_get_ap_list(httpd_req_t *req)
{
  wifi_scan_config_t scan_config = {
      .ssid = 0,
      .bssid = 0,
      .channel = 0,
      .show_hidden = true};

  esp_wifi_set_mode(WIFI_MODE_APSTA);
  ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));

  wifi_ap_record_t wifi_records[MAX_APs];

  uint16_t maxRecods = MAX_APs;
  ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&maxRecods, wifi_records));
  cJSON *wifi_scan_json = cJSON_CreateArray();
  for (size_t i = 0; i < maxRecods; i++)
  {
    cJSON *entry = cJSON_CreateObject();
    cJSON_AddStringToObject(entry, "ssid", (char *)wifi_records[i].ssid);
    cJSON_AddNumberToObject(entry, "rssi", wifi_records[i].rssi);
    cJSON_AddItemToArray(wifi_scan_json, entry);
  }
  char *json_string = cJSON_Print(wifi_scan_json);
  httpd_resp_set_type(req, "application/json");
  httpd_resp_send(req, json_string, strlen(json_string));
  cJSON_Delete(wifi_scan_json);
  free(json_string);
  return ESP_OK;
}

/**************** AP TO STA ********************/
typedef struct ap_config_t
{
  char ssid[32];
  char password[64];
} ap_config_t;

static void connect_to_ap(void *params)
{
  wifi_disconnect();
  wifi_destroy_netif();
  // wifi_init();

  ap_config_t *ap_config = (ap_config_t *)params;

  ESP_LOGI(TAG, "Connecting to AP %s %s", ap_config->ssid, ap_config->password);
  if (wifi_connect_sta(ap_config->ssid, ap_config->password, 10000) != ESP_OK)
  {
    ESP_LOGE(TAG, "Failed to connect to AP");
    wifi_connect_ap("esp32ap", "password");
  }
  else
  {
    ESP_LOGI(TAG, "Connected to AP");
  }
  vTaskDelete(NULL);
}

static esp_err_t on_ap_to_sta(httpd_req_t *req)
{

  char buffer[100];
  static ap_config_t ap_config;

  memset(&buffer, 0, sizeof(buffer));
  httpd_req_recv(req, buffer, req->content_len);

  cJSON *payload = cJSON_Parse(buffer);
  strcpy(ap_config.ssid, cJSON_GetObjectItem(payload, "ssid")->valuestring);
  strcpy(ap_config.password, cJSON_GetObjectItem(payload, "password")->valuestring);
  cJSON_Delete(payload);

  xTaskCreate(connect_to_ap, "connect_to_ap", 1024 * 5, &ap_config, 1, NULL);
  return ESP_OK;
}

/********************Web Socket *******************/

#define WS_MAX_SIZE 1024
static int client_session_id;

esp_err_t send_ws_message(char *message)
{
  if (!client_session_id)
  {
    ESP_LOGE(TAG, "no client_session_id");
    return -1;
  }
  httpd_ws_frame_t ws_message = {
      .final = true,
      .fragmented = false,
      .len = strlen(message),
      .payload = (uint8_t *)message,
      .type = HTTPD_WS_TYPE_TEXT};
  return httpd_ws_send_frame_async(server, client_session_id, &ws_message);
}

static esp_err_t on_web_socket_url(httpd_req_t *req)
{
  client_session_id = httpd_req_to_sockfd(req);
  if (req->method == HTTP_GET)
    return ESP_OK;

  httpd_ws_frame_t ws_pkt;
  memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
  ws_pkt.type = HTTPD_WS_TYPE_TEXT;
  ws_pkt.payload = malloc(WS_MAX_SIZE);
  httpd_ws_recv_frame(req, &ws_pkt, WS_MAX_SIZE);
  printf("ws payload: %.*s\n", ws_pkt.len, ws_pkt.payload);
  free(ws_pkt.payload);

  char *response = "connected OK 😊";
  httpd_ws_frame_t ws_responce = {
      .final = true,
      .fragmented = false,
      .type = HTTPD_WS_TYPE_TEXT,
      .payload = (uint8_t *)response,
      .len = strlen(response)};
  return httpd_ws_send_frame(req, &ws_responce);
}

/*******************************************/

static void init_server()
{

  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.uri_match_fn = httpd_uri_match_wildcard;

  ESP_ERROR_CHECK(httpd_start(&server, &config));

  httpd_uri_t get_ap_list_url = {
      .uri = "/api/get-ap-list",
      .method = HTTP_GET,
      .handler = on_get_ap_list};
  httpd_register_uri_handler(server, &get_ap_list_url);

  httpd_uri_t toggle_led_url = {
      .uri = "/api/toggle-led",
      .method = HTTP_POST,
      .handler = on_toggle_led_url};
  httpd_register_uri_handler(server, &toggle_led_url);

  httpd_uri_t ap_to_sta_url = {
      .uri = "/api/ap-sta",
      .method = HTTP_POST,
      .handler = on_ap_to_sta};
  httpd_register_uri_handler(server, &ap_to_sta_url);

  httpd_uri_t web_socket_url = {
      .uri = "/ws",
      .method = HTTP_GET,
      .handler = on_web_socket_url,
      .is_websocket = true};
  httpd_register_uri_handler(server, &web_socket_url);

  httpd_uri_t default_url = {
      .uri = "/*",
      .method = HTTP_GET,
      .handler = on_default_url};
  httpd_register_uri_handler(server, &default_url);
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

  init_btn();
  wifi_init();
  // ESP_ERROR_CHECK(wifi_connect_sta("POCO", "password", 10000));
  wifi_connect_ap("esp32ap", "password");

  start_mdns_service();
  init_server();
}
