#include <stdint.h>
#include <string.h>

#include "ble_acqs.h"
#include "ble_srv_common.h"
#include "app_error.h"


void ble_acqs_init(ble_acqs_t* p_acqs)
{
    uint32_t	err_code; // Variable to hold return codes from library and softdevice functions
		
		ble_uuid_t service_uuid; // What is the purpose of additional 8bit uuid
		ble_uuid128_t base_uuid = BLE_ACQ_BASE_UUID;
		service_uuid.uuid = BLE_ACQ_SERVICE;
		err_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
		APP_ERROR_CHECK(err_code);
		
		//Add a service declaration to the Attribute Table
		err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
																				&service_uuid,
																				&p_acqs->service_handle);
		APP_ERROR_CHECK(err_code);
}