#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_http_server.h"
#include "cJSON.h"
#include "driver/gpio.h"

#define TAG "SERVER"
#define PIN 2

double getBMP280();
void initi2c();

extern const char index_html[] asm("_binary_index_html_start");

static esp_err_t on_url_hit(httpd_req_t *req)
{

    ESP_LOGI(TAG, "url %s was hit", req->uri);

    httpd_resp_send(req, (const char *)index_html, strlen((const char *)index_html));
    return ESP_OK;
}

static esp_err_t on_get_temperature(httpd_req_t *req)
{
    ESP_LOGI(TAG, "url %s was hit", req->uri);
    double temperature = getBMP280();
    char message[100];
    sprintf(message, "{\"temperature\":%f}", temperature);
    httpd_resp_send(req, message, strlen(message));
    return ESP_OK;
}

static esp_err_t on_led_set(httpd_req_t *req)
{
    char *buffer = (char *)malloc(req->content_len + 1);
    memset(buffer, 0, sizeof(req->content_len + 1));
    httpd_req_recv(req, buffer, req->content_len);
    cJSON *payload = cJSON_Parse(buffer);

    int led = cJSON_GetObjectItem(payload, "led_on")->valueint;

    gpio_pad_select_gpio(PIN);
    gpio_set_direction(PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN, led);

    ESP_LOGI(TAG, "url %s was hit", req->uri);
    httpd_resp_set_status(req, "204 NO CONTENT");
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

void RegisterEndPoints(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    ESP_LOGI(TAG, "starting server");
    if (httpd_start(&server, &config) != ESP_OK)
    {
        ESP_LOGE(TAG, "COULD NOT START SERVER");
    }

    httpd_uri_t first_end_point_config = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = on_url_hit};
    httpd_register_uri_handler(server, &first_end_point_config);

    httpd_uri_t temperature_end_point_config = {
        .uri = "/api/temperature",
        .method = HTTP_GET,
        .handler = on_get_temperature};
    httpd_register_uri_handler(server, &temperature_end_point_config);

    httpd_uri_t led_end_point_config = {
        .uri = "/api/led",
        .method = HTTP_POST,
        .handler = on_led_set};
    httpd_register_uri_handler(server, &led_end_point_config);

    initi2c();
}