#ifndef BLE_ACQS_H__
#define BLE_ACQS_H__

#include <stdint.h>
#include "ble.h"
#include "ble_srv_common.h"

#define BLE_ACQ_BASE_UUID {{0x77, 0x94, 0x9D, 0x36, 0xD4, 0xB9, 0x21, 0x21, 0x87, 0xD0, 0x02, 0x42, 0xAC, 0x13, 0x18, 0x40}} // 128-bit base UUID

#define BLE_ACQ_SERVICE 0x0000
#define BLE_TEMP_CHARACTERISTC 0x0001
#define BLE_ACCX_CHARACTERISTC 0x0002
#define BLE_ACCY_CHARACTERISTC 0x0003
#define BLE_ACCZ_CHARACTERISTC 0x0004

typedef enum {
	TEMP_NOTIFICATION_ENABLED,
	TEMP_NOTIFICATION_DISABLED
} temp_notification_t;

typedef enum {
	ACCX_NOTIFICATION_ENABLED,
	ACCX_NOTIFICATION_DISABLED
} accx_notification_t;

typedef enum {
	ACCY_NOTIFICATION_ENABLED,
	ACCY_NOTIFICATION_DISABLED
} accy_notification_t;

typedef enum {
	ACCZ_NOTIFICATION_ENABLED,
	ACCZ_NOTIFICATION_DISABLED
} accz_notification_t;

typedef struct {
	uint16_t conn_handle;
	uint16_t service_handle;
	ble_gatts_char_handles_t temp_handles;
	ble_gatts_char_handles_t accx_handles;
	ble_gatts_char_handles_t accy_handles;
	ble_gatts_char_handles_t accz_handles;
	temp_notification_t temp_notification;
	accx_notification_t accx_notification;
	accy_notification_t accy_notification;
	accz_notification_t accz_notification;
} ble_acqs_t;

void ble_acqs_init(ble_acqs_t* p_acqs);
void ble_acqs_on_ble_evt(ble_evt_t const* p_ble_evt, ble_acqs_t* p_acqs);
void temp_characteristic_notify(ble_acqs_t* p_acqs, float* temp);
void accx_characteristic_notify(ble_acqs_t* p_acqs, int16_t* value);
void accy_characteristic_notify(ble_acqs_t* p_acqs, int16_t* value);
void accz_characteristic_notify(ble_acqs_t* p_acqs, int16_t* value);

#endif
