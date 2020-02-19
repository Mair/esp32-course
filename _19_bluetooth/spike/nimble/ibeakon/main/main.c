#include <stdio.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"

#define TAG "BLE"

static void ble_app_on_sync(void)
{
  /* Generate a non-resolvable private address. */
  ble_addr_t addr;
  ble_hs_id_gen_rnd(1, &addr);
  ble_hs_id_set_rnd(addr.val);

  /* Advertise indefinitely. */
  uint8_t uuid128[16];
  memset(uuid128, 0x11, sizeof uuid128);

  /* Major version=2; minor version=10. */
  ble_ibeacon_set_adv_data(uuid128, 2, 10, 0);

  /* TODO: Begin advertising. */
  struct ble_gap_adv_params adv_params = (struct ble_gap_adv_params){0};
  ble_gap_adv_start(BLE_OWN_ADDR_RANDOM, NULL, BLE_HS_FOREVER, &adv_params, NULL, NULL);
}

void blehr_host_task(void *param)
{
  ESP_LOGI(TAG, "BLE Host Task Started");
  nimble_port_run();
}

void app_main(void)
{
  nvs_flash_init();
  esp_nimble_hci_and_controller_init();
  nimble_port_init();
  ble_hs_cfg.sync_cb = ble_app_on_sync;
  nimble_port_freertos_init(blehr_host_task);
}
