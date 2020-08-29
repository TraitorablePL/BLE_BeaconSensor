#include <stdint.h>
#include <string.h>
#include <math.h>

#include "nrf_gpio.h"
#include "ble_acqs.h"
#include "ble_srv_common.h"
#include "app_error.h"
#include "nrf_log.h"

#define PI 3.14159265

static float sine_table[360];

void sine_table_init(){

	for (int i = 0; i < 360; i++) {
		float rads = i * PI / 180;
		sine_table[i] = sin(rads);
	}
}

float sine_value_get(uint16_t degree){

	return sine_table[degree%360];
}

static void on_write(ble_evt_t* p_ble_evt, ble_acqs_t* p_acqs){

	ble_gatts_evt_write_t* p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

	if((p_evt_write->handle == p_acqs->sine_handles.cccd_handle) && (p_evt_write->len == 2)){
		
		if(p_acqs->sine_notification == SINE_NOTIFICATION_DISABLED){
			p_acqs->sine_notification = SINE_NOTIFICATION_ENABLED;
		}
		else{
			p_acqs->sine_notification = SINE_NOTIFICATION_DISABLED;
		}
	}
	else if((p_evt_write->handle == p_acqs->counter_handles.cccd_handle) && (p_evt_write->len == 2)){
		if(p_acqs->counter_notification == COUNTER_NOTIFICATION_DISABLED){
			p_acqs->counter_notification = COUNTER_NOTIFICATION_ENABLED;
		}
		else{
			p_acqs->counter_notification = COUNTER_NOTIFICATION_DISABLED;
		}
	}
}

void ble_acqs_on_ble_evt(ble_evt_t const* p_ble_evt, ble_acqs_t* p_acqs){
	
	switch (p_ble_evt->header.evt_id){
		case BLE_GAP_EVT_CONNECTED:
			p_acqs->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
			NRF_LOG_INFO("ACQ Connected\r\n");
			break;
			
		case BLE_GAP_EVT_DISCONNECTED:
			p_acqs->counter_notification = COUNTER_NOTIFICATION_DISABLED;
			p_acqs->sine_notification = SINE_NOTIFICATION_DISABLED;
			p_acqs->conn_handle = BLE_CONN_HANDLE_INVALID;
			NRF_LOG_INFO("ACQ Disconnected\r\n");
			break;
			
		case BLE_GATTS_EVT_WRITE:
			on_write((ble_evt_t*)p_ble_evt, p_acqs);
			NRF_LOG_INFO("ACQ Write\r\n");
			break;
			
		default:
			break;
	}
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
	char_md.char_props.notify   = 1;
	
	// OUR_JOB: Step 3.A, Configuring Client Characteristic Configuration Descriptor metadata and add to char_md structure
	ble_gatts_attr_md_t cccd_md;
	memset(&cccd_md, 0, sizeof(cccd_md));
	
	//Important read & write permissions
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
	cccd_md.vloc = BLE_GATTS_VLOC_STACK;
	char_md.p_cccd_md = &cccd_md;
	
	//Characteristic presentation format
	ble_gatts_char_pf_t char_pf;
	memset(&char_pf, 0, sizeof(char_pf));
	char_pf.format = BLE_GATT_CPF_FORMAT_FLOAT32;
	char_md.p_char_pf = &char_pf;
	
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
	uint8_t value[4] = {0x00,0x00,0x00,0x00};
	attr_char_value.max_len = 4;
	attr_char_value.init_len = 4;
	attr_char_value.p_value = value;

	// OUR_JOB: Step 2.E, Add our new characteristic to the service
	
	err_code = sd_ble_gatts_characteristic_add(	p_acqs->service_handle,
												&char_md,
												&attr_char_value,
												&p_acqs->sine_handles);
	APP_ERROR_CHECK(err_code);

	return NRF_SUCCESS;
}

void sine_characteristic_notify(ble_acqs_t* p_acqs, float* sine_value){
	
	if ((p_acqs->conn_handle != BLE_CONN_HANDLE_INVALID) &&
		(p_acqs->sine_notification == SINE_NOTIFICATION_ENABLED)){
		
		uint16_t len = 4;
		ble_gatts_hvx_params_t hvx_params;
		memset(&hvx_params, 0, sizeof(hvx_params));

		hvx_params.handle = p_acqs->sine_handles.value_handle;
		hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
		hvx_params.offset = 0;
		hvx_params.p_len = &len;
		hvx_params.p_data = (uint8_t*)sine_value;  

		sd_ble_gatts_hvx(p_acqs->conn_handle, &hvx_params);
	}
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
	char_md.char_props.notify = 1;
	
	// OUR_JOB: Step 3.A, Configuring Client Characteristic Configuration Descriptor metadata and add to char_md structure
	ble_gatts_attr_md_t cccd_md;
	memset(&cccd_md, 0, sizeof(cccd_md));
	
	//Important read & write permissions
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
	cccd_md.vloc = BLE_GATTS_VLOC_STACK;
	char_md.p_cccd_md = &cccd_md;
	
	//Characteristic presentation format
	ble_gatts_char_pf_t char_pf;
	memset(&char_pf, 0, sizeof(char_pf));
	char_pf.format = BLE_GATT_CPF_FORMAT_UINT16;
	char_md.p_char_pf = &char_pf;
	
	// OUR_JOB: Step 2.B, Configure the attribute metadata
	ble_gatts_attr_md_t attr_md;
	memset(&attr_md, 0, sizeof(attr_md));
	attr_md.vloc = BLE_GATTS_VLOC_STACK;

	// OUR_JOB: Step 2.G, Set read/write security levels to our characteristic
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);

	// OUR_JOB: Step 2.C, Configure the characteristic value attribute
	ble_gatts_attr_t attr_char_value;
	memset(&attr_char_value, 0, sizeof(attr_char_value));    
	attr_char_value.p_uuid = &char_uuid;
	attr_char_value.p_attr_md = &attr_md;

	// OUR_JOB: Step 2.H, Set characteristic length in number of bytes
	
	uint8_t value[2] = {0x00,0x00};
	attr_char_value.max_len = 2;
	attr_char_value.init_len = 2;
	attr_char_value.p_value = value;

	// OUR_JOB: Step 2.E, Add our new characteristic to the service
	err_code = sd_ble_gatts_characteristic_add(	p_acqs->service_handle,
												&char_md,
												&attr_char_value,
												&p_acqs->counter_handles);
	APP_ERROR_CHECK(err_code);

	return NRF_SUCCESS;
}

void counter_characteristic_notify(ble_acqs_t* p_acqs, uint16_t* counter_value){
	
	if ((p_acqs->conn_handle != BLE_CONN_HANDLE_INVALID) &&
		(p_acqs->counter_notification == COUNTER_NOTIFICATION_ENABLED)){
		
		uint16_t len = 2;
		ble_gatts_hvx_params_t hvx_params;
		memset(&hvx_params, 0, sizeof(hvx_params));
				
		hvx_params.handle = p_acqs->counter_handles.value_handle;
		hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
		hvx_params.offset = 0;
		hvx_params.p_len = &len;
		hvx_params.p_data = (uint8_t*)counter_value;

		sd_ble_gatts_hvx(p_acqs->conn_handle, &hvx_params);
	}
}

void ble_acqs_init(ble_acqs_t* p_acqs){
	
	uint32_t err_code;
	ble_uuid_t service_uuid;
	ble_uuid128_t base_uuid = BLE_ACQ_BASE_UUID;
	service_uuid.uuid = BLE_ACQ_SERVICE;
	
	err_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
	APP_ERROR_CHECK(err_code);
	
	err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
										&service_uuid,
										&p_acqs->service_handle);
	APP_ERROR_CHECK(err_code);

	sine_char_add(p_acqs);
	counter_char_add(p_acqs);
	
	p_acqs->sine_notification = SINE_NOTIFICATION_DISABLED;
	p_acqs->counter_notification = COUNTER_NOTIFICATION_DISABLED;
	p_acqs->conn_handle = BLE_CONN_HANDLE_INVALID;
}
