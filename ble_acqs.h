#ifndef BLE_ACQS_H__
#define BLE_ACQS_H__

#include <stdint.h>
#include "ble.h"
#include "ble_srv_common.h"

#define BLE_ACQ_BASE_UUID {0x77, 0x94, 0x9D, 0x36, 0xD4, 0xB9, 0x21, 0x21, 0x87, 0xD0, 0x02, 0x42, 0xAC, 0x13, 0x00, 0x03} // 128-bit base UUID
#define BLE_ACQ_SERVICE 0xDEAD // Just a random, but recognizable value

typedef struct {
    uint16_t    service_handle;     /**< Handle of ACQ Service (as provided by the BLE stack). */
} ble_acqs_t;

void ble_acqs_init(ble_acqs_t* p_acqs);

#endif
