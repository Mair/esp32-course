#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
void app_main(void)
{
    nvs_flash_init();
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BLE);
    esp_bluedroid_init();
    esp_bluedroid_enable();
    // esp_ble_gatts_register_callback(gatts_event_handler);
    // esp_ble_gap_register_callback(gap_event_handler);
    // esp_ble_gatts_app_register(PROFILE_A_APP_ID);
    // esp_ble_gatts_app_register(PROFILE_B_APP_ID);
    esp_ble_gatt_set_local_mtu(512);
}
