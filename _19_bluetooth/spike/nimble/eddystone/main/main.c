#include <stdio.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "console/console.h"
#include "services/gap/ble_svc_gap.h"

#define TAG "BLE"


static void
ble_app_set_addr(void)
{
    ble_addr_t addr;
    int rc;

    rc = ble_hs_id_gen_rnd(1, &addr);
    assert(rc == 0);

    rc = ble_hs_id_set_rnd(addr.val);
    assert(rc == 0);
}

  static void
ble_app_advertise(void)
{
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;
    int rc;

    /* Configure an eddystone URL beacon to be advertised;
     * URL: https://apache.mynewt.org
     */
    fields = (struct ble_hs_adv_fields){ 0 };
    
    rc = ble_eddystone_set_adv_data_url(&fields,
                                        BLE_EDDYSTONE_URL_SCHEME_HTTPS,
                                        "learnesp32",
                                        strlen("learnesp32"),
                                        BLE_EDDYSTONE_URL_SUFFIX_COM, -50);
    assert(rc == 0);
   
    /* Begin advertising. */
    adv_params = (struct ble_gap_adv_params){ 0 };
    rc = ble_gap_adv_start(BLE_OWN_ADDR_RANDOM, NULL, BLE_HS_FOREVER,
                           &adv_params, NULL, NULL);
    assert(rc == 0);
}

static void
ble_app_on_sync(void)
{
    /* Generate a non-resolvable private address. */
    ble_app_set_addr();

    /* Advertise indefinitely. */
    ble_app_advertise();
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
