#include <stdint.h>
#include <string.h>

#include "nrf_gpio.h"
#include "ble_acqs.h"
#include "ble_srv_common.h"
#include "app_error.h"

void ble_acqs_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context){

	//ble_acqs_t * p_our_service = (ble_acqs_t*)p_context;  
	// OUR_JOB: Step 3.D Implement switch case handling BLE events related to our service. 
}

/**@brief Function for adding our new characterstic to "Our service" that we initiated in the previous tutorial. 
 *
 * @param[in]   p_our_service        Our Service structure.
 *
 */

static uint32_t sine_char_add(ble_acqs_t * p_acqs){

	// OUR_JOB: Step 2.A, Add a custom characteristic UUID
	uint32_t err_code;
	ble_uuid_t char_uuid;
	ble_uuid128_t base_uuid = BLE_ACQ_BASE_UUID;
	char_uuid.uuid = BLE_SINE_CHARACTERISTC;
	err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
	APP_ERROR_CHECK(err_code);  

	// OUR_JOB: Step 2.F Add read/write properties to our characteristic
	ble_gatts_char_md_t char_md;
	memset(&char_md, 0, sizeof(char_md));
	char_md.char_props.read = 1;

	// OUR_JOB: Step 3.A, Configuring Client Characteristic Configuration Descriptor metadata and add to char_md structure
	ble_gatts_attr_md_t cccd_md;
	memset(&cccd_md, 0, sizeof(cccd_md));

	// OUR_JOB: Step 2.B, Configure the attribute metadata
	ble_gatts_attr_md_t attr_md;
	memset(&attr_md, 0, sizeof(attr_md));
	attr_md.vloc = BLE_GATTS_VLOC_STACK; //store attributes in SoftDevice controlled part of memory

	// OUR_JOB: Step 2.G, Set read/write security levels to our characteristic
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
	
	// OUR_JOB: Step 2.C, Configure the characteristic value attribute
	ble_gatts_attr_t attr_char_value;
	memset(&attr_char_value, 0, sizeof(attr_char_value));    
	attr_char_value.p_uuid = &char_uuid;
	attr_char_value.p_attr_md = &attr_md;

	// OUR_JOB: Step 2.H, Set characteristic length in number of bytes


	// OUR_JOB: Step 2.E, Add our new characteristic to the service
	
	err_code = sd_ble_gatts_characteristic_add(p_acqs->service_handle,
																						 &char_md,
																						 &attr_char_value,
																						 &p_acqs->char_handles);
	APP_ERROR_CHECK(err_code);

	return NRF_SUCCESS;
}

static uint32_t counter_char_add(ble_acqs_t* p_acqs){

	// OUR_JOB: Step 2.A, Add a custom characteristic UUID
	uint32_t err_code;
	ble_uuid_t char_uuid;
	ble_uuid128_t base_uuid = BLE_ACQ_BASE_UUID;
	char_uuid.uuid = BLE_COUNTER_CHARACTERISTC;
	err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
	APP_ERROR_CHECK(err_code);  

	// OUR_JOB: Step 2.F Add read/write properties to our characteristic
	ble_gatts_char_md_t char_md;
	memset(&char_md, 0, sizeof(char_md));
	char_md.char_props.read = 1;

	// OUR_JOB: Step 3.A, Configuring Client Characteristic Configuration Descriptor metadata and add to char_md structure
	ble_gatts_attr_md_t cccd_md;
	memset(&cccd_md, 0, sizeof(cccd_md));

	// OUR_JOB: Step 2.B, Configure the attribute metadata
	ble_gatts_attr_md_t attr_md;
	memset(&attr_md, 0, sizeof(attr_md));
	attr_md.vloc = BLE_GATTS_VLOC_STACK; //store attributes in SoftDevice controlled part of memory

	// OUR_JOB: Step 2.G, Set read/write security levels to our characteristic
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);

	// OUR_JOB: Step 2.C, Configure the characteristic value attribute
	ble_gatts_attr_t attr_char_value;
	memset(&attr_char_value, 0, sizeof(attr_char_value));    
	
	attr_char_value.p_uuid = &char_uuid;
	attr_char_value.p_attr_md = &attr_md;

	// OUR_JOB: Step 2.H, Set characteristic length in number of bytes
	
	uint8_t value[4]            = {0x00,0x00};
	
	attr_char_value.max_len     = 2;
	attr_char_value.init_len    = 2;
	attr_char_value.p_value     = value;

	// OUR_JOB: Step 2.E, Add our new characteristic to the service
	err_code = sd_ble_gatts_characteristic_add(p_acqs->service_handle,
																						 &char_md,
																						 &attr_char_value,
																						 &p_acqs->char_handles);
	APP_ERROR_CHECK(err_code);

	return NRF_SUCCESS;
}

void ble_acqs_init(ble_acqs_t* p_acqs){
	
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

	sine_char_add(p_acqs);
	counter_char_add(p_acqs);
}
