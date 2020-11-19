#ifndef BLE_ACQS_H__
#define BLE_ACQS_H__

#include <stdint.h>
#include "ble.h"
#include "ble_srv_common.h"

#define BLE_ACQ_BASE_UUID {{0x77, 0x94, 0x9D, 0x36, 0xD4, 0xB9, 0x21, 0x21, 0x87, 0xD0, 0x02, 0x42, 0xAC, 0x13, 0x18, 0x40}} // 128-bit base UUID

#define BLE_ACQ_SERVICE 0x0000
#define BLE_SINE_CHARACTERISTC 0x0001
#define BLE_COUNTER_CHARACTERISTC 0x0002

typedef enum {
	SINE_NOTIFICATION_ENABLED,
	SINE_NOTIFICATION_DISABLED
} sine_notification_t;

typedef enum {
	COUNTER_NOTIFICATION_ENABLED,
	COUNTER_NOTIFICATION_DISABLED
} counter_notification_t;

typedef struct {
	uint16_t conn_handle;
	uint16_t service_handle;
	ble_gatts_char_handles_t sine_handles;
	ble_gatts_char_handles_t counter_handles;
	sine_notification_t sine_notification;
	counter_notification_t counter_notification;
} ble_acqs_t;


void ble_acqs_init(ble_acqs_t* p_acqs);
void ble_acqs_on_ble_evt(ble_evt_t const* p_ble_evt, ble_acqs_t* p_acqs);
void sine_characteristic_notify(ble_acqs_t* p_our_service, float* sine_value);
void counter_characteristic_notify(ble_acqs_t* p_acqs, uint16_t* counter_value);

void sine_table_init(void);
float sine_value_get(uint16_t degree);

#endif
