#include <string.h>

#include "esp_log.h"

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#include "wifi_station.h"
#include "common.h"

#define TAG "WIFI_CONF_BLE"
#define DEFAULT_BLE_NAME "IoT-"

#define NUM_SERVICES 1
#define SYSTEM_SETTINGS_SERVICE 0

#define SYSTEM_SETTINGS_SERVICE_UUID    0x00EE
#define WIFI_SSID_CHAR_UUID             0xEE00
#define WIFI_PASSWORD_CHAR_UUID         0xEE01
#define CONNECT_WIFI_CHAR_UUID          0xEE02

#define NUM_HANDLE_SYSTEM_SETTINGS 20

// SYSTEM_SETTINGS_SERVICE
#define WIFI_SSID_ID        0
#define WIFI_PASSWORD_ID    1
#define CONNECT_WIFI_ID     2

#define adv_config_flag (1 << 0)
#define scan_rsp_config_flag (1 << 1)

static void gap_setup_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
static void gatts_setup_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
static void system_settings_gatts_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
esp_err_t enable_ble();
esp_err_t disable_ble();
esp_err_t setup_lead_advertising();

esp_gatt_char_prop_t SYSTEM_SETTINGS_SERVICE_PROPERTY = 0;
esp_gatt_if_t current_gatts_if;
static uint8_t adv_config_done = 0;

static esp_ble_adv_params_t adv_params = {
    .adv_int_min = 0x20,
    .adv_int_max = 0x40,
    .adv_type = ADV_TYPE_IND,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static uint8_t adv_service_uuid128[32] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    //first uuid, 16bit, [12],[13] is the value
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xEE, 0x00, 0x00, 0x00,
    //second uuid, 32bit, [12], [13], [14], [15] is the value
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
    };

esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x20,
    .max_interval = 0x40,
    .appearance = 0x00,
    .manufacturer_len = 0,
    .p_manufacturer_data =  NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = 32,
    .p_service_uuid = adv_service_uuid128,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

// scan response data
esp_ble_adv_data_t scan_rsp_data = {
    .set_scan_rsp = true,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x20,
    .max_interval = 0x40,
    .appearance = 0x00,
    .manufacturer_len = 4,
    .p_manufacturer_data = NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = 32,
    .p_service_uuid = adv_service_uuid128,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

struct gatts_characteristic_inst
{
    esp_bt_uuid_t char_uuid;
    esp_bt_uuid_t descr_uuid;
    uint16_t char_handle;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
};

struct gatts_service_inst
{
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    struct gatts_characteristic_inst chars[16];
};

static struct gatts_service_inst gatts_profile_array[NUM_SERVICES] = {
    [SYSTEM_SETTINGS_SERVICE] = {
        .gatts_cb = system_settings_gatts_handler,
        .gatts_if = ESP_GATT_IF_NONE, /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
};

static void gap_setup_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event)
    {
#ifdef CONFIG_SET_RAW_ADV_DATA
    case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
        adv_config_done &= (~adv_config_flag);
        if (adv_config_done == 0)
        {
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;
    case ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT:
        adv_config_done &= (~scan_rsp_config_flag);
        if (adv_config_done == 0)
        {
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;
#else
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        adv_config_done &= (~adv_config_flag);
        if (adv_config_done == 0)
        {
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;
    case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
        adv_config_done &= (~scan_rsp_config_flag);
        if (adv_config_done == 0)
        {
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;
#endif
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        //advertising start complete event to indicate advertising start successfully or failed
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS)
        {
            ESP_LOGE(TAG, "Advertising start failed");
        }
        break;
    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS)
        {
            ESP_LOGE(TAG, "Advertising stop failed");
        }
        else
        {
            ESP_LOGI(TAG, "Stop adv successfully");
        }
        break;
    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
        ESP_LOGI(TAG, "update connection params status = %d, min_int = %d, max_int = %d,conn_int = %d,latency = %d, timeout = %d",
                 param->update_conn_params.status,
                 param->update_conn_params.min_int,
                 param->update_conn_params.max_int,
                 param->update_conn_params.conn_int,
                 param->update_conn_params.latency,
                 param->update_conn_params.timeout);
        break;
    default:
        break;
    }
}

static void gatts_setup_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    /* If event is register event, store the gatts_if for each profile */
    if (event == ESP_GATTS_REG_EVT)
    {
        if (param->reg.status == ESP_GATT_OK)
        {
            gatts_profile_array[param->reg.app_id].gatts_if = gatts_if;
        }
        else
        {
            ESP_LOGE(TAG, "Reg app failed, app_id %04x, status %d",
                     param->reg.app_id,
                     param->reg.status);
            return;
        }
    }

    /* If the gatts_if equal to profile A, call profile A cb handler,
     * so here call each profile's callback */
    do
    {
        int idx;
        for (idx = 0; idx < NUM_SERVICES; idx++)
        {
            if (gatts_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
                gatts_if == gatts_profile_array[idx].gatts_if)
            {
                if (gatts_profile_array[idx].gatts_cb)
                {
                    gatts_profile_array[idx].gatts_cb(event, gatts_if, param);
                }
            }
        }
    } while (0);
}

static void system_settings_gatts_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    switch (event)
    {
    case ESP_GATTS_REG_EVT:
        ESP_LOGI(TAG, "REGISTER_APP_EVT, status %d, app_id %d", param->reg.status, param->reg.app_id);
        gatts_profile_array[SYSTEM_SETTINGS_SERVICE].service_id.is_primary = true;
        gatts_profile_array[SYSTEM_SETTINGS_SERVICE].service_id.id.inst_id = 0x00;
        gatts_profile_array[SYSTEM_SETTINGS_SERVICE].service_id.id.uuid.len = ESP_UUID_LEN_16;
        gatts_profile_array[SYSTEM_SETTINGS_SERVICE].service_id.id.uuid.uuid.uuid16 = SYSTEM_SETTINGS_SERVICE_UUID;
        current_gatts_if = gatts_if;
        setup_lead_advertising();
        esp_ble_gatts_create_service(gatts_if, &gatts_profile_array[SYSTEM_SETTINGS_SERVICE].service_id, NUM_HANDLE_SYSTEM_SETTINGS);
        break;

    case ESP_GATTS_READ_EVT:
    {
        ESP_LOGI(TAG, "GATT_READ_EVT, conn_id %d, trans_id %d, handle %d", param->read.conn_id, param->read.trans_id, param->read.handle);
        esp_gatt_rsp_t rsp;
        memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
        rsp.attr_value.handle = param->read.handle;
        rsp.attr_value.len = 4;
        uint16_t currentHandle = param->read.handle;

        if (currentHandle == gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[WIFI_SSID_ID].char_handle)
        {
            rsp.attr_value.len = strlen(wifi_params_sta.network_ssid);
            memcpy(rsp.attr_value.value, wifi_params_sta.network_ssid, sizeof(wifi_params_sta.network_ssid));
            ESP_LOGI(TAG, "Sending BLE read response: %s", rsp.attr_value.value);
        }
        else if (currentHandle == gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[WIFI_PASSWORD_ID].char_handle)
        {
            rsp.attr_value.len = strlen(wifi_params_sta.network_password);
            memcpy(rsp.attr_value.value, wifi_params_sta.network_password, sizeof(wifi_params_sta.network_password));
            ESP_LOGI(TAG, "Sending BLE read response: %s", rsp.attr_value.value);
        }
        esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id,
                                    ESP_GATT_OK, &rsp);
        break;
    }
    case ESP_GATTS_WRITE_EVT:
    {
        ESP_LOGI(TAG, "System Settings: GATT_WRITE_EVT, conn_id %d, trans_id %d, handle %d", param->write.conn_id, param->write.trans_id, param->write.handle);
        if (!param->write.is_prep)
        {
            esp_log_buffer_hex(TAG, param->write.value, param->write.len);
            uint16_t writeHandle = param->write.handle;

            if (writeHandle == gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[WIFI_SSID_ID].char_handle)
            {
                ESP_LOGI(TAG, "String written is %s", (char *) param->write.value);
                memset(wifi_params_sta.network_ssid, 0, sizeof(wifi_params_sta.network_ssid));
                for (int i=0; i < param->write.len; i++) 
                {
                    wifi_params_sta.network_ssid[i] = param->write.value[i];
                }
            }
            else if (writeHandle == gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[WIFI_PASSWORD_ID].char_handle)
            {
                ESP_LOGI(TAG, "String written is %s", (char *) param->write.value);
                memset(wifi_params_sta.network_password, 0, sizeof(wifi_params_sta.network_password));
                for (int i=0; i < param->write.len; i++) 
                {
                    wifi_params_sta.network_password[i] = param->write.value[i];
                }
            }
            else if (writeHandle == gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[CONNECT_WIFI_ID].char_handle) 
            {
                wifi_init_station(wifi_params_sta.network_ssid, wifi_params_sta.network_password);
            }
        }
        else
        {
            ESP_LOGI(TAG, "Needs write prep?");
        }
        esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
        break;
    }
    case ESP_GATTS_EXEC_WRITE_EVT:
        ESP_LOGI(TAG, "ESP_GATTS_EXEC_WRITE_EVT");
        esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
        break;
    case ESP_GATTS_MTU_EVT:
        ESP_LOGI(TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
        break;
    case ESP_GATTS_UNREG_EVT:
        break;
    case ESP_GATTS_CREATE_EVT:  
        ESP_LOGI(TAG, "CREATE_SERVICE_EVT, status %d,  service_handle %d", param->create.status, param->create.service_handle);
        gatts_profile_array[SYSTEM_SETTINGS_SERVICE].service_handle = param->create.service_handle;
        SYSTEM_SETTINGS_SERVICE_PROPERTY = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY;

        //WiFI SSID CHARACTERISTIC
        gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[WIFI_SSID_ID].char_uuid.len = ESP_UUID_LEN_16;
        gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[WIFI_SSID_ID].char_uuid.uuid.uuid16 = WIFI_SSID_CHAR_UUID;
        gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[WIFI_SSID_ID].property = SYSTEM_SETTINGS_SERVICE_PROPERTY;
        gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[WIFI_SSID_ID].perm = ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE;

        //WiFI Password CHARACTERISTIC
        gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[WIFI_PASSWORD_ID].char_uuid.len = ESP_UUID_LEN_16;
        gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[WIFI_PASSWORD_ID].char_uuid.uuid.uuid16 = WIFI_PASSWORD_CHAR_UUID;
        gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[WIFI_PASSWORD_ID].property = SYSTEM_SETTINGS_SERVICE_PROPERTY;
        gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[WIFI_PASSWORD_ID].perm = ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE;

        //  Connect Wifi CHARACTERISTIC
        gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[CONNECT_WIFI_ID].char_uuid.len = ESP_UUID_LEN_16;
        gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[CONNECT_WIFI_ID].char_uuid.uuid.uuid16 = CONNECT_WIFI_CHAR_UUID;
        gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[CONNECT_WIFI_ID].property = SYSTEM_SETTINGS_SERVICE_PROPERTY;
        gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[CONNECT_WIFI_ID].perm = ESP_GATT_PERM_READ| ESP_GATT_PERM_WRITE;

        //ADD WiFi SSID  characteristic
        esp_ble_gatts_add_char(gatts_profile_array[SYSTEM_SETTINGS_SERVICE].service_handle,
                               &gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[WIFI_SSID_ID].char_uuid,
                               gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[WIFI_SSID_ID].perm,
                               gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[WIFI_SSID_ID].property,
                               NULL, NULL);

        //ADD WiFi Password  characteristic
        esp_ble_gatts_add_char(gatts_profile_array[SYSTEM_SETTINGS_SERVICE].service_handle,
                               &gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[WIFI_PASSWORD_ID].char_uuid,
                               gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[WIFI_PASSWORD_ID].perm,
                               gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[WIFI_PASSWORD_ID].property,
                               NULL, NULL);

        // ADD Connect WiFi ID 
        esp_ble_gatts_add_char(gatts_profile_array[SYSTEM_SETTINGS_SERVICE].service_handle, 
                               &gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[CONNECT_WIFI_ID].char_uuid,
                               gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[CONNECT_WIFI_ID].perm,
                               gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[CONNECT_WIFI_ID].property,
                               NULL, NULL);
        esp_ble_gatts_start_service(gatts_profile_array[SYSTEM_SETTINGS_SERVICE].service_handle);

    case ESP_GATTS_ADD_CHAR_EVT:
    {
        ESP_LOGI(TAG, "ADD_CHAR_EVT, status %d,  attr_handle %x, service_handle %x, char uuid %x",
                param->add_char.status, param->add_char.attr_handle, param->add_char.service_handle, param->add_char.char_uuid.uuid.uuid16);
        /* store characteristic handles for later usage */
        if (param->add_char.char_uuid.uuid.uuid16 == WIFI_SSID_CHAR_UUID)
        {
            gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[WIFI_SSID_ID].char_handle = param->add_char.attr_handle;
        }
        else if (param->add_char.char_uuid.uuid.uuid16 == WIFI_PASSWORD_CHAR_UUID)
        {
            gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[WIFI_PASSWORD_ID].char_handle = param->add_char.attr_handle;
        }
        else if(param->add_char.char_uuid.uuid.uuid16 == CONNECT_WIFI_CHAR_UUID){
            gatts_profile_array[SYSTEM_SETTINGS_SERVICE].chars[CONNECT_WIFI_ID].char_handle = param->add_char.attr_handle;
        }
        break;
    }
    case ESP_GATTS_ADD_INCL_SRVC_EVT:
        break;
    case ESP_GATTS_DELETE_EVT:
        break;
    case ESP_GATTS_START_EVT:
        ESP_LOGI(TAG, "SERVICE_START_EVT, status %d, service_handle %d",
                 param->start.status, param->start.service_handle);
        break;
    case ESP_GATTS_STOP_EVT:
        break;
    case ESP_GATTS_CONNECT_EVT:{
        esp_ble_conn_update_params_t conn_params = {0};
        memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
        /* For the IOS system, please reference the apple official documents about the ble connection parameters restrictions. */
        conn_params.latency = 0;
        conn_params.max_int = 0x20;    // max_int = 0x20*1.25ms = 40ms
        conn_params.min_int = 0x10;    // min_int = 0x10*1.25ms = 20ms
        conn_params.timeout = 400;    // timeout = 400*10ms = 4000ms
        ESP_LOGI(TAG, "CONNECT_EVT, conn_id %d, remote %02x:%02x:%02x:%02x:%02x:%02x:",
                 param->connect.conn_id,
                 param->connect.remote_bda[0], param->connect.remote_bda[1], param->connect.remote_bda[2],
                 param->connect.remote_bda[3], param->connect.remote_bda[4], param->connect.remote_bda[5]);
        gatts_profile_array[SYSTEM_SETTINGS_SERVICE].conn_id = param->connect.conn_id;
        //start sent the update connection parameters to the peer device.
        esp_ble_gap_update_conn_params(&conn_params);
        esp_ble_gap_start_advertising(&adv_params);
        break;
    }
    case ESP_GATTS_CONF_EVT:
        ESP_LOGI(TAG, "ESP_GATTS_CONF_EVT status %d", param->conf.status);
        if (param->conf.status != ESP_GATT_OK)
        {
            esp_log_buffer_hex(TAG, param->conf.value, param->conf.len);
        }
        break;
    case ESP_GATTS_DISCONNECT_EVT:
    case ESP_GATTS_OPEN_EVT:
    case ESP_GATTS_CANCEL_OPEN_EVT:
    case ESP_GATTS_CLOSE_EVT:
    case ESP_GATTS_LISTEN_EVT:
    case ESP_GATTS_CONGEST_EVT:
    default:
        break;
    }
}

esp_err_t init_ble()
{
    esp_err_t err;
    ESP_LOGI(TAG, "Now initialising BLE");
    esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    err = esp_bt_controller_init(&bt_cfg);
    if (err) {
        ESP_LOGE(TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(err));
        return err;
    }
    err = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (err) {
        ESP_LOGE(TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(err));
        return err;
    }
    err = esp_bluedroid_init();
    if (err) {
        ESP_LOGE(TAG, "%s init bluetooth failed: %s\n", __func__, esp_err_to_name(err));
        return err;
    }
    err = esp_bluedroid_enable();
    if (err) {
        ESP_LOGE(TAG, "%s enable bluetooth failed: %s\n", __func__, esp_err_to_name(err));
        return err;
    }
    return err;
}

esp_err_t enable_ble()
{
    esp_err_t err;

    err = esp_ble_gatts_register_callback(gatts_setup_handler);
    if (err)
    {
        ESP_LOGE(TAG, "Could not register lead gattss handler, error code = %x\n", err);
        return err;
    }

    err = setup_lead_advertising();
    if (err)
    {
        ESP_LOGE(TAG, "Could not set up lead advertising parameters, error code = %x\n", err);
        return err;
    }

    err = esp_ble_gap_register_callback(gap_setup_handler);
    if (err)
    {
        ESP_LOGE(TAG, "Could not register lead gap handler, error code = %x\n", err);
        return err;
    }

    err = esp_ble_gatts_app_register(SYSTEM_SETTINGS_SERVICE);
    if (err)
    {
        ESP_LOGE(TAG, "Could not register System Settings Service, error code = %x\n", err);
    }

    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(512);
    if (local_mtu_ret)
    {
        ESP_LOGE(TAG, "set local  MTU failed, error code = %x", local_mtu_ret);
    }

    return err;
}

esp_err_t disable_ble()
{
    esp_err_t err;
    esp_ble_gap_stop_advertising();
    err = esp_ble_gatts_app_unregister(current_gatts_if);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Could not shut down lead mode, error code = %x\n", err);
    }
    return err;
}

esp_err_t setup_lead_advertising()
{
    esp_err_t err;
    // The hex and string representations WILL look different. They are both unique to each device.
    // We choose to use the string representation as it is easier to handle
    const char *device_id = get_device_id();
    ESP_LOGI(TAG, "Adding Device ID to BLE Name: %s", device_id);
    char ble_adv_name[20] = DEFAULT_BLE_NAME;
    strcat(ble_adv_name, device_id);
    err = ESP_OK;

    err = esp_ble_gap_set_device_name(ble_adv_name);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "set device name failed, error code = %x", err);
    }
    //config adv data
    err = esp_ble_gap_config_adv_data(&adv_data);
    if (err)
    {
        ESP_LOGE(TAG, "config adv data failed, error code = %x", err);
    }
    adv_config_done |= adv_config_flag;
    err = esp_ble_gap_config_adv_data(&scan_rsp_data);
    if (err){
        ESP_LOGE(TAG, "Could not configure scan response data, error code = %x", err);
    }
    adv_config_done |= scan_rsp_config_flag;

    return err;
}