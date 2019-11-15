#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOSConfig.h"
/* BLE */
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "console/console.h"
#include "services/gap/ble_svc_gap.h"
#include "blehr_sens.h"

static const char *device_name = "blehr_sensor_1.0";
static bool notify_state;
static uint8_t heartrate = 90;

static void blehr_tx_hrate(xTimerHandle ev)
{
    static uint8_t hrm[2];
    int rc;
    struct os_mbuf *om;

    if (!notify_state) {
        xTimerStop( blehr_tx_timer, 1000 / portTICK_PERIOD_MS );
        heartrate = 90;
        return;
    }

    hrm[0] = 0x06; /* contact of a sensor */
    hrm[1] = heartrate; /* storing dummy data */

    /* Simulation of heart beats */
    heartrate++;
    if (heartrate == 160) {
        heartrate = 90;
    }

    om = ble_hs_mbuf_from_flat(hrm, sizeof(hrm));
    rc = ble_gattc_notify_custom(conn_handle, hrs_hrm_handle, om);

    assert(rc == 0);

    blehr_tx_hrate_reset();
}

void blehr_host_task(void *param)
{
    ESP_LOGI(tag, "BLE Host Task Started");
    /* This function will return only when nimble_port_stop() is executed */
    nimble_port_run();
    nimble_port_freertos_deinit();
}
void app_main(void)
{
  nvs_flash_init();
  esp_nimble_hci_and_controller_init();
  nimble_port_init();

  //struct ble_hs_cfg;
  //  ble_hs_cfg.sync_cb = blehr_on_sync;
  //  ble_hs_cfg.reset_cb = blehr_on_reset;

  xTimerCreate("blehr_tx_timer", pdMS_TO_TICKS(1000), pdTRUE, (void *)0, blehr_tx_hrate);

  gatt_svr_init();
  ble_svc_gap_device_name_set(device_name);
  nimble_port_freertos_init(blehr_host_task);
}
