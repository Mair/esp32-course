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

// static const uint8_t hidReportMap[] = {
//     0x05, 0x01,  // Usage Page (Generic Desktop)
//     0x09, 0x02,  // Usage (Mouse)
//     0xA1, 0x01,  // Collection (Application)
//     0x85, 0x01,  // Report Id (1)
//     0x09, 0x01,  //   Usage (Pointer)
//     0xA1, 0x00,  //   Collection (Physical)
//     0x05, 0x09,  //     Usage Page (Buttons)
//     0x19, 0x01,  //     Usage Minimum (01) - Button 1
//     0x29, 0x03,  //     Usage Maximum (03) - Button 3
//     0x15, 0x00,  //     Logical Minimum (0)
//     0x25, 0x01,  //     Logical Maximum (1)
//     0x75, 0x01,  //     Report Size (1)
//     0x95, 0x03,  //     Report Count (3)
//     0x81, 0x02,  //     Input (Data, Variable, Absolute) - Button states
//     0x75, 0x05,  //     Report Size (5)
//     0x95, 0x01,  //     Report Count (1)
//     0x81, 0x01,  //     Input (Constant) - Padding or Reserved bits
//     0x05, 0x01,  //     Usage Page (Generic Desktop)
//     0x09, 0x30,  //     Usage (X)
//     0x09, 0x31,  //     Usage (Y)
//     0x09, 0x38,  //     Usage (Wheel)
//     0x15, 0x81,  //     Logical Minimum (-127)
//     0x25, 0x7F,  //     Logical Maximum (127)
//     0x75, 0x08,  //     Report Size (8)
//     0x95, 0x03,  //     Report Count (3)
//     0x81, 0x06,  //     Input (Data, Variable, Relative) - X & Y coordinate
//     0xC0,        //   End Collection
//     0xC0,        // End Collection

//     0x05, 0x01,  // Usage Pg (Generic Desktop)
//     0x09, 0x06,  // Usage (Keyboard)
//     0xA1, 0x01,  // Collection: (Application)
//     0x85, 0x02,  // Report Id (2)
//     //
//     0x05, 0x07,  //   Usage Pg (Key Codes)
//     0x19, 0xE0,  //   Usage Min (224)
//     0x29, 0xE7,  //   Usage Max (231)
//     0x15, 0x00,  //   Log Min (0)
//     0x25, 0x01,  //   Log Max (1)
//     //
//     //   Modifier byte
//     0x75, 0x01,  //   Report Size (1)
//     0x95, 0x08,  //   Report Count (8)
//     0x81, 0x02,  //   Input: (Data, Variable, Absolute)
//     //
//     //   Reserved byte
//     0x95, 0x01,  //   Report Count (1)
//     0x75, 0x08,  //   Report Size (8)
//     0x81, 0x01,  //   Input: (Constant)
//     //
//     //   LED report
//     0x95, 0x05,  //   Report Count (5)
//     0x75, 0x01,  //   Report Size (1)
//     0x05, 0x08,  //   Usage Pg (LEDs)
//     0x19, 0x01,  //   Usage Min (1)
//     0x29, 0x05,  //   Usage Max (5)
//     0x91, 0x02,  //   Output: (Data, Variable, Absolute)
//     //
//     //   LED report padding
//     0x95, 0x01,  //   Report Count (1)
//     0x75, 0x03,  //   Report Size (3)
//     0x91, 0x01,  //   Output: (Constant)
//     //
//     //   Key arrays (6 bytes)
//     0x95, 0x06,  //   Report Count (6)
//     0x75, 0x08,  //   Report Size (8)
//     0x15, 0x00,  //   Log Min (0)
//     0x25, 0x65,  //   Log Max (101)
//     0x05, 0x07,  //   Usage Pg (Key Codes)
//     0x19, 0x00,  //   Usage Min (0)
//     0x29, 0x65,  //   Usage Max (101)
//     0x81, 0x00,  //   Input: (Data, Array)
//     //
//     0xC0,        // End Collection
//     //
//     0x05, 0x0C,   // Usage Pg (Consumer Devices)
//     0x09, 0x01,   // Usage (Consumer Control)
//     0xA1, 0x01,   // Collection (Application)
//     0x85, 0x03,   // Report Id (3)
//     0x09, 0x02,   //   Usage (Numeric Key Pad)
//     0xA1, 0x02,   //   Collection (Logical)
//     0x05, 0x09,   //     Usage Pg (Button)
//     0x19, 0x01,   //     Usage Min (Button 1)
//     0x29, 0x0A,   //     Usage Max (Button 10)
//     0x15, 0x01,   //     Logical Min (1)
//     0x25, 0x0A,   //     Logical Max (10)
//     0x75, 0x04,   //     Report Size (4)
//     0x95, 0x01,   //     Report Count (1)
//     0x81, 0x00,   //     Input (Data, Ary, Abs)
//     0xC0,         //   End Collection
//     0x05, 0x0C,   //   Usage Pg (Consumer Devices)
//     0x09, 0x86,   //   Usage (Channel)
//     0x15, 0xFF,   //   Logical Min (-1)
//     0x25, 0x01,   //   Logical Max (1)
//     0x75, 0x02,   //   Report Size (2)
//     0x95, 0x01,   //   Report Count (1)
//     0x81, 0x46,   //   Input (Data, Var, Rel, Null)
//     0x09, 0xE9,   //   Usage (Volume Up)
//     0x09, 0xEA,   //   Usage (Volume Down)
//     0x15, 0x00,   //   Logical Min (0)
//     0x75, 0x01,   //   Report Size (1)
//     0x95, 0x02,   //   Report Count (2)
//     0x81, 0x02,   //   Input (Data, Var, Abs)
//     0x09, 0xE2,   //   Usage (Mute)
//     0x09, 0x30,   //   Usage (Power)
//     0x09, 0x83,   //   Usage (Recall Last)
//     0x09, 0x81,   //   Usage (Assign Selection)
//     0x09, 0xB0,   //   Usage (Play)
//     0x09, 0xB1,   //   Usage (Pause)
//     0x09, 0xB2,   //   Usage (Record)
//     0x09, 0xB3,   //   Usage (Fast Forward)
//     0x09, 0xB4,   //   Usage (Rewind)
//     0x09, 0xB5,   //   Usage (Scan Next)
//     0x09, 0xB6,   //   Usage (Scan Prev)
//     0x09, 0xB7,   //   Usage (Stop)
//     0x15, 0x01,   //   Logical Min (1)
//     0x25, 0x0C,   //   Logical Max (12)
//     0x75, 0x04,   //   Report Size (4)
//     0x95, 0x01,   //   Report Count (1)
//     0x81, 0x00,   //   Input (Data, Ary, Abs)
//     0x09, 0x80,   //   Usage (Selection)
//     0xA1, 0x02,   //   Collection (Logical)
//     0x05, 0x09,   //     Usage Pg (Button)
//     0x19, 0x01,   //     Usage Min (Button 1)
//     0x29, 0x03,   //     Usage Max (Button 3)
//     0x15, 0x01,   //     Logical Min (1)
//     0x25, 0x03,   //     Logical Max (3)
//     0x75, 0x02,   //     Report Size (2)
//     0x81, 0x00,   //     Input (Data, Ary, Abs)
//     0xC0,           //   End Collection
//     0x81, 0x03,   //   Input (Const, Var, Abs)
//     0xC0,            // End Collectionq
// };


static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {/* Service: Device Information */
     .type = BLE_GATT_SVC_TYPE_PRIMARY,
     .uuid = BLE_UUID16_DECLARE(0x1812),
     .characteristics = (struct ble_gatt_chr_def[]){
        //  {
        //      /* Characteristic: * Manufacturer name */
        //      .uuid = BLE_UUID16_DECLARE(0x2A33),
        //      .access_cb = gatt_mouse,
        //      .flags = BLE_GATT_CHR_F_READ
        //  },
         {
             0, /* No more characteristics in this service */
         },
     }},

    {
        0, /* No more services */
    },
};


static int gatt_mouse(uint16_t conn_handle, uint16_t attr_handle,
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
