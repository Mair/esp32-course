#include <stdio.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "esp_heap_caps.h"

#define DEVICE_NAME "MY BLE SERVER"
#define MANUFACTURER_NAME 0x2A29


uint8_t ble_addr_type;

static void ble_app_scan(void);

int attr_read(uint16_t conn_handle,  const struct ble_gatt_error *error,   struct ble_gatt_attr *attr, void *arg)
{
    if (error->status == 0)
    {
        printf("printing val\n");
        //printf("value was %.*s\n", attr->om->om_len, attr->om->om_data);
        return BLE_HS_EDONE;
    }
    return 0;
}

static int get_val(uint16_t conn_handle,  const struct ble_gatt_error *error,   const struct ble_gatt_chr *chr, void *arg)
{
    if (error->status == 0)
    {
        char buffer[80];
        memset(buffer, 0, sizeof(buffer));
        ble_uuid_to_str(&chr->uuid.u, buffer);
        ESP_LOGI("CONNECT", "char %s found", buffer);
        if(strcmp(buffer,"0x2a29") == 0)
        {
            printf("got name\n");
            ble_gattc_read(conn_handle, chr->val_handle,  attr_read, NULL);
            
             //return BLE_HS_EDONE;
        }
    } 
    return 0;
}


static int findPrimaryServices(uint16_t conn_handle, const struct ble_gatt_error *error, const struct ble_gatt_svc *service, void *arg)
{
    if (error->status == 0)
    {
        char buffer[80];
        memset(buffer, 0, sizeof(buffer));
        ble_uuid_to_str(&service->uuid.u, buffer);
        ESP_LOGI("CONNECT", "SERVICE %s found", buffer);
        if (strcmp(buffer, "0x180a") == 0)
        {
            ESP_LOGI("CONNECT", "Found DEVICE_INFO_SERVICE!!!");
            int res = ble_gattc_disc_all_chrs(conn_handle, service->start_handle, service->end_handle, get_val, NULL);
            ESP_LOGI("deb", "res= %d", res);
            return BLE_HS_EDONE;
        }
    } 
    return 0;
}


static int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_hs_adv_fields fields;

    switch (event->type)
    {
    case BLE_GAP_EVENT_DISC:
        ble_hs_adv_parse_fields(&fields, event->disc.data, event->disc.length_data);
        printf("Discovered device with name: \"%.*s\" Looking for device with name \"Blank\"\n", fields.name_len, fields.name);

        if (fields.name_len == strlen(DEVICE_NAME) && memcmp(fields.name, DEVICE_NAME, fields.name_len) == 0)
        {
            printf("FOUND BLE Device\n");
            /* Scanning must be stopped before a connection can be initiated. */
            ble_gap_disc_cancel();
            ble_gap_connect(BLE_OWN_ADDR_PUBLIC, &event->disc.addr, 10000, NULL, ble_gap_event, NULL);
        }

        break;

    case BLE_GAP_EVENT_CONNECT:
        ESP_LOGI("GAP", "BLE_GAP_EVENT_CONNECT %s", event->connect.status == 0 ? "OK" : "Failed");
        if (event->connect.status == 0)
        {
            ESP_LOGI("CONNECT", "Finding Device");
            ble_gap_conn_find(event->connect.conn_handle, NULL);
            ble_gattc_disc_all_svcs(event->connect.conn_handle, findPrimaryServices, NULL);
        }
        break;
    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI("GAP", "BLE_GAP_EVENT_DISCONNECT");
        break;
    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI("GAP", "BLE_GAP_EVENT_ADV_COMPLETE");
        break;
    case BLE_GAP_EVENT_SUBSCRIBE:
        ESP_LOGI("GAP", "BLE_GAP_EVENT_SUBSCRIBE");
        break;
    default:
        break;
    }
    return 0;
}

void ble_app_scan(void)
{
    uint8_t own_addr_type;
    struct ble_gap_disc_params disc_params;

    ble_hs_id_infer_auto(0, &own_addr_type);

    /* Tell the controller to filter duplicates; we don't want to process
     * repeated advertisements from the same device.
     */
    disc_params.filter_duplicates = 1;

    /**
     * Perform a passive scan.  I.e., don't send follow-up scan requests to
     * each advertiser.
     */
    disc_params.passive = 1;

    /* Use defaults for the rest of the parameters. */
    disc_params.itvl = 0;
    disc_params.window = 0;
    disc_params.filter_policy = 0;
    disc_params.limited = 0;

    ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &disc_params, ble_gap_event, NULL);
}

static void ble_app_on_sync(void)
{
    ble_hs_id_infer_auto(0, &ble_addr_type);
    ble_app_scan();
}

void host_task(void *param)
{
    nimble_port_run();
}

void app_main(void)
{
    nvs_flash_init();

    esp_nimble_hci_and_controller_init();
    nimble_port_init();
    ble_svc_gap_device_name_set(DEVICE_NAME);
    ble_svc_gap_init();
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;
    ble_hs_cfg.sync_cb = ble_app_on_sync;
    nimble_port_freertos_init(host_task);
}
