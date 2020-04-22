#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#include "esp_event_loop.h"
//#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_now.h"

#define TAG "ESP_NOW"

uint8_t esp_1[6] = {0x24, 0x6f, 0x28, 0x95, 0xa7, 0xb0};
uint8_t esp_2[6] = {0x30, 0xae, 0xa4, 0x25, 0x15, 0xc8};

char *macToStr(char *buffer, uint8_t *mac)
{
	sprintf(buffer, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return buffer;
}

void send_cb(const uint8_t *mac_addr, esp_now_send_status_t status)
{
	char mac_buffer[13];
	switch (status)
	{
	case ESP_NOW_SEND_SUCCESS:
		ESP_LOGI(TAG, "sent OK to %s",macToStr(mac_buffer ,mac_addr));
		break;
	case ESP_NOW_SEND_FAIL:
		ESP_LOGE(TAG, "sent Failed  to %s", macToStr(mac_buffer, mac_addr));
		break;
	}
}

void receive_cb(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{
	char mac_buffer[13];
	ESP_LOGI(TAG, "received message");
	printf("%s received %.*s\n", macToStr(mac_buffer, mac_addr), data_len, data);
}

void app_main(void)
{
	uint8_t my_mac[6];
	esp_efuse_mac_get_default(my_mac);
	bool isCurrentEsp1 = memcmp(&my_mac, &esp_1, 3) == 0;
	uint8_t *peers_mac = isCurrentEsp1 ? esp_2 : esp_1;
	char my_buffer[13], peer_buffer[13];
	ESP_LOGI(TAG, "myMac %s peersMac %s\n", macToStr(my_buffer, my_mac), macToStr(peer_buffer, peers_mac));

	nvs_flash_init();
	tcpip_adapter_init();
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_start());

	esp_now_init();
	ESP_ERROR_CHECK(esp_now_register_send_cb(send_cb));
	ESP_ERROR_CHECK(esp_now_register_recv_cb(receive_cb));

	esp_now_peer_info_t peer;
	memset(&peer, 0, sizeof(esp_now_peer_info_t));
	memcpy(peer.peer_addr, peers_mac, 6);
	
	// peer.channel = 1;
	peer.encrypt = true;
	memcpy(peer.lmk,"lmk1234567890123", strlen("lmk1234567890123"));
	peer.ifidx = ESP_IF_WIFI_STA;

	ESP_ERROR_CHECK(esp_now_add_peer(&peer));


	char send_buffer[250];
	for (int i = 0; i < 100; i++)
	{
		sprintf(send_buffer, "Hello ESP-NOW message %d from %s ", i, macToStr(my_buffer, my_mac));
		ESP_LOGI(TAG, "sending message len %d", strlen(send_buffer));
		ESP_ERROR_CHECK(esp_now_send(NULL, (uint8_t *)send_buffer, strlen(send_buffer)));
		vTaskDelay(pdMS_TO_TICKS(1000));
	}

	esp_now_deinit();
	ESP_ERROR_CHECK(esp_wifi_stop());
}
