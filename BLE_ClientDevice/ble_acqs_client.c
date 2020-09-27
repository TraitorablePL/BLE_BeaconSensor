#include "sdk_common.h"
#include <stdlib.h>
#include "ble_acqs_client.h"
#include "ble_db_discovery.h"
#include "ble_types.h"
#include "ble_gattc.h"
#include "ble_srv_common.h"

/**@brief Function for intercepting the errors of GATTC and the BLE GATT Queue.
 *
 * @param[in] nrf_error   Error code.
 * @param[in] p_ctx       Parameter from the event handler.
 * @param[in] conn_handle Connection handle.
 */
static void gatt_error_handler(uint32_t nrf_error,
                               void     *p_ctx,
                               uint16_t conn_handle) {

    ble_acqs_t *p_ble_acqs = (ble_acqs_t*)p_ctx;

    if (p_ble_acqs->error_handler != NULL) {
        p_ble_acqs->error_handler(nrf_error);
    }
}

void ble_acqs_on_db_disc_evt(ble_acqs_t* p_ble_acqs, ble_db_discovery_evt_t const* p_evt) {

    ble_acqs_evt_t evt;
    memset(&evt,0,sizeof(ble_acqs_evt_t));

    // Check if the ACQ Service was discovered.
    if (p_evt->evt_type == BLE_DB_DISCOVERY_COMPLETE &&
        p_evt->params.discovered_db.srv_uuid.uuid == ACQ_UUID_SERVICE &&
        p_evt->params.discovered_db.srv_uuid.type == p_ble_acqs->uuid_type){

        for (uint16_t i = 0; i < p_evt->params.discovered_db.char_count; i++) {

            const ble_gatt_db_char_t* p_char = &(p_evt->params.discovered_db.charateristics[i]);
			
            switch (p_char->characteristic.uuid.uuid){

                case ACQ_UUID_SINE_CHAR:
                    evt.params.handles.sine_handle = p_char->characteristic.handle_value;
                    evt.params.handles.sine_cccd_handle = p_char->cccd_handle;
                    break;

                case ACQ_UUID_COUNTER_CHAR:
                    evt.params.handles.counter_handle = p_char->characteristic.handle_value;
                    evt.params.handles.counter_cccd_handle = p_char->cccd_handle;
                    break;

                default:
                    break;
				
            }
        }

        //If the instance was assigned prior to db_discovery, assign the db_handles
		
        if (p_ble_acqs->conn_handle != BLE_CONN_HANDLE_INVALID){
            if ((p_ble_acqs->handles.sine_handle           == BLE_GATT_HANDLE_INVALID)&&
                (p_ble_acqs->handles.sine_cccd_handle      == BLE_GATT_HANDLE_INVALID)&&
                (p_ble_acqs->handles.counter_handle        == BLE_GATT_HANDLE_INVALID)&&
                (p_ble_acqs->handles.counter_cccd_handle   == BLE_GATT_HANDLE_INVALID)){
                
                p_ble_acqs->handles = evt.params.handles;
            }
        }
        
        if (p_ble_acqs->evt_handler != NULL) {
            evt.evt_type    = BLE_ACQS_EVT_DISCOVERY_COMPLETE;
            evt.conn_handle = p_evt->conn_handle;
            p_ble_acqs->evt_handler(p_ble_acqs, &evt);
        }
    }
}

uint32_t ble_acqs_init(ble_acqs_t* p_ble_acqs, ble_acqs_init_t* p_ble_acqs_init) {

    uint32_t      err_code;
    ble_uuid_t    acqs_uuid;
    ble_uuid128_t acqs_base_uuid = ACQ_UUID_BASE;

    VERIFY_PARAM_NOT_NULL(p_ble_acqs);
    VERIFY_PARAM_NOT_NULL(p_ble_acqs_init);
    VERIFY_PARAM_NOT_NULL(p_ble_acqs_init->p_gatt_queue);

    err_code = sd_ble_uuid_vs_add(&acqs_base_uuid, &p_ble_acqs->uuid_type);
    VERIFY_SUCCESS(err_code);

    acqs_uuid.type = p_ble_acqs->uuid_type;
    acqs_uuid.uuid = ACQ_UUID_SERVICE;

    p_ble_acqs->conn_handle                 = BLE_CONN_HANDLE_INVALID;
    p_ble_acqs->handles.sine_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    p_ble_acqs->handles.sine_handle         = BLE_GATT_HANDLE_INVALID;
    p_ble_acqs->handles.counter_cccd_handle = BLE_GATT_HANDLE_INVALID;
    p_ble_acqs->handles.counter_handle      = BLE_GATT_HANDLE_INVALID;
    
    p_ble_acqs->evt_handler     = p_ble_acqs_init->evt_handler;
    p_ble_acqs->p_gatt_queue    = p_ble_acqs_init->p_gatt_queue;
    p_ble_acqs->error_handler   = p_ble_acqs_init->error_handler;

    return ble_db_discovery_evt_register(&acqs_uuid);
}

void ble_acqs_on_ble_evt(ble_evt_t const* p_ble_evt, void* p_context){

    ble_acqs_t* p_ble_acqs = (ble_acqs_t*)p_context;

    if ((p_context == NULL) || (p_ble_evt == NULL)) {
        return;
    }

    if ((p_ble_acqs->conn_handle == BLE_CONN_HANDLE_INVALID) || 
        (p_ble_acqs->conn_handle != p_ble_evt->evt.gap_evt.conn_handle)) {
        return;
    }

    switch (p_ble_evt->header.evt_id){

        case BLE_GATTC_EVT_HVX:
            //on_hvx(p_ble_acqs, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            //on_disconnected(p_ble_acqs, p_ble_evt);
            break;

        default:
            break;
    }
}

uint32_t ble_acqs_handles_assign(ble_acqs_t                 *p_ble_acqs,
                                 uint16_t                   conn_handle,
                                 ble_acqs_handles_t const   *p_peer_handles) {

    VERIFY_PARAM_NOT_NULL(p_ble_acqs);
    p_ble_acqs->conn_handle = conn_handle;

    if (p_peer_handles != NULL) {
        p_ble_acqs->handles = *p_peer_handles;
    }
    return nrf_ble_gq_conn_handle_register(p_ble_acqs->p_gatt_queue, conn_handle);
}