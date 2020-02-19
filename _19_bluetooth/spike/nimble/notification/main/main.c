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
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"


#define TAG "BLE"
#define GATT_DEVICE_INFO_UUID                   0x180A
#define GATT_MANUFACTURER_NAME_UUID             0x2A29
#define GATT_AGE_UUID                           0x2A80

#define GATT_DESCRIPTOR_USER_DESC_UUID          0x2901

// #define GATT_CUSTOM_UUID                       0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff

static uint8_t blehr_addr_type;
static const char *device_name = "blehr_sensor_1.0";
static void ble_app_advertise(void);

static int
gatt_svr_chr_access_device_info(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg);

static int gatt_svr_chr_custom(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg);

static int gatt_svr_chr_custom_descriptor(uint16_t conn_handle, uint16_t attr_handle,
struct ble_gatt_access_ctxt *ctxt, void *arg);

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {/* Service: Device Information */
     .type = BLE_GATT_SVC_TYPE_PRIMARY,
     .uuid = BLE_UUID16_DECLARE(GATT_DEVICE_INFO_UUID),
     .characteristics = (struct ble_gatt_chr_def[]){
         {
             /* Characteristic: * Manufacturer name */
             .uuid = BLE_UUID16_DECLARE(GATT_MANUFACTURER_NAME_UUID),
             .access_cb = gatt_svr_chr_access_device_info,
             .flags = BLE_GATT_CHR_F_READ
         },
         {
             /* Characteristic: * Manufacturer name */
             .uuid = BLE_UUID16_DECLARE(GATT_AGE_UUID),
             .access_cb = gatt_svr_chr_access_device_info,
             .flags = BLE_GATT_CHR_F_READ
         },
          {
             /* Characteristic: * Manufacturer name */
             .uuid = BLE_UUID128_DECLARE(0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff),
             .access_cb = gatt_svr_chr_custom,
             .flags = BLE_GATT_CHR_F_WRITE,
         },
         {
             0, /* No more characteristics in this service */
         },
     }},

    {
        0, /* No more services */
    },
};


static int gatt_svr_chr_custom(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg)
{
   printf("incoming message: %.*s\n",ctxt->om->om_len, ctxt->om->om_data);

  return 0;
}

static int gatt_svr_chr_access_device_info(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    uint16_t uuid;
    int rc;

    ESP_LOGI(TAG, "attr_handle %d", attr_handle);

    uuid = ble_uuid_u16(ctxt->chr->uuid);
    

    if (uuid == GATT_MANUFACTURER_NAME_UUID) {
        rc = os_mbuf_append(ctxt->om, "My manufacturer name", strlen("My manufacturer name"));
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    if (uuid == GATT_AGE_UUID) {
      static uint8_t body_sens_loc = 45;
        rc = os_mbuf_append(ctxt->om,  &body_sens_loc, sizeof(body_sens_loc));
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    //  if (uuid == GATT_CUSTOM_UUID) {
    //     rc = os_mbuf_append(ctxt->om,  "WHOS YOUR DADDY", strlen("WHOS YOUR DADDY"));
    //     return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    // }

    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}


static int blehr_gap_event(struct ble_gap_event *event, void *arg)
{
  switch (event->type)
  {
  case BLE_GAP_EVENT_CONNECT:
    ESP_LOGI(TAG, "connection %s\n", event->connect.status == 0 ? "established" : "failed");
    if (event->connect.status != 0)
    {
      ble_app_advertise();
    }
    break;

  case BLE_GAP_EVENT_DISCONNECT:
    ESP_LOGI(TAG, "disconnect; reason=%d\n", event->disconnect.reason);
    ble_app_advertise();
    break;

  case BLE_GAP_EVENT_ADV_COMPLETE:
    ESP_LOGI(TAG, "adv complete\n");
    ble_app_advertise();
    break;

  default:
    break;
  }

  return 0;
}

static void ble_app_set_addr(void)
{
  ble_hs_id_infer_auto(0, &blehr_addr_type);

  // uint8_t addr_val[6] = {0};
  // ble_hs_id_copy_addr(blehr_addr_type, addr_val, NULL);
  // ESP_LOGI(TAG, "address is %02x:%02x:%02x:%02x:%02x:%02x", addr_val[5], addr_val[4], addr_val[3], addr_val[2], addr_val[1], addr_val[0]);
}

static void ble_app_advertise(void)
{
  struct ble_hs_adv_fields fields;
  memset(&fields, 0, sizeof(fields));
  fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
  fields.tx_pwr_lvl_is_present = 1;
  fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;
  fields.name = (uint8_t *)ble_svc_gap_device_name();
  fields.name_len = strlen(device_name);
  fields.name_is_complete = 1;

  ble_gap_adv_set_fields(&fields);

  struct ble_gap_adv_params adv_params;
  memset(&adv_params, 0, sizeof(adv_params));
  adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
  adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
  ble_gap_adv_start(blehr_addr_type, NULL, BLE_HS_FOREVER,
                    &adv_params, blehr_gap_event, NULL);
}

static void
ble_app_on_sync(void)
{

  ble_app_set_addr();
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
  ble_svc_gap_device_name_set("xxxxxxx");
  
  ble_svc_gap_init();
  ble_svc_gatt_init();
  ble_gatts_count_cfg(gatt_svr_svcs);
  ble_gatts_add_svcs(gatt_svr_svcs);

  nimble_port_freertos_init(blehr_host_task);
}
