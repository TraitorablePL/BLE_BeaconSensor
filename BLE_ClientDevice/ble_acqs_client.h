#ifndef BLE_ACQS_CLIENT_H__
#define BLE_ACQS_CLIENT_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_gatt.h"
#include "ble_db_discovery.h"
#include "ble_srv_common.h"
#include "nrf_ble_gq.h"
#include "nrf_sdh_ble.h"

#define BLE_ACQS_BLE_OBSERVER_PRIO 2

#define BLE_ACQS_DEF(_name)                                                                        \
static ble_acqs_t _name;                                                                           \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                \
                     BLE_ACQS_BLE_OBSERVER_PRIO,                                                   \
                     ble_acqs_on_ble_evt, &_name)

#define ACQ_UUID_BASE {0x77, 0x94, 0x9D, 0x36, 0xD4, 0xB9, 0x21, 0x21, 0x87, 0xD0, 0x02, 0x42, 0xAC, 0x13, 0x18, 0x40}

#define ACQ_UUID_SERVICE 0xAAA0
#define ACQ_UUID_SINE_CHAR 0xAAA1
#define ACQ_UUID_COUNTER_CHAR 0xAAA2

typedef enum {
    BLE_ACQS_EVT_DISCOVERY_COMPLETE,  
    BLE_ACQS_EVT_SINE_NOTIFICATION,
	BLE_ACQS_EVT_COUNTER_NOTIFICATION,
    BLE_ACQS_EVT_DISCONNECTED
} ble_acqs_evt_type_t;

typedef struct {
    uint16_t sine_cccd_handle;
    uint16_t sine_handle;
	uint16_t counter_cccd_handle;
	uint16_t counter_handle;
} ble_acqs_handles_t;

/**@brief ACQS Event structure. */
typedef struct {
    ble_acqs_evt_type_t 	evt_type;    /**< Type of the event. */
    uint16_t             	conn_handle; /**< Connection handle on which the event occured.*/

	//Only one value can be received per event
    union {
        float		        sine;			// Sine value received from event BLE_ACQS_EVT_SINE_NOTIFICATION
		uint16_t	        counter;		// Counter value received from event BLE_ACQS_EVT_COUNTER_NOTIFICATION
    	ble_acqs_handles_t  handles;        // Handles related Aquisition Service from event BLE_ACQS_EVT_COUNTER_NOTIFICATION
    } params;

} ble_acqs_evt_t;

// Forward declaration of the ble_acqs_client_t type needed by event handler below
typedef struct ble_acqs_s ble_acqs_t;

/**@brief   Event handler type.
 *
 * @details This is the type of the event handler that is to be provided by the application
 *          of this module in order to receive events.
 */

typedef void (*ble_acqs_evt_handler_t) (ble_acqs_t * p_ble_acqs, ble_acqs_evt_t * p_evt);

struct ble_acqs_s {
    uint8_t                   	uuid_type;      /**< UUID type. */
	uint16_t                  	conn_handle;    /**< Connection handle as provided by the SoftDevice. */
    ble_acqs_handles_t          handles;        /**< Handles related to LBS on the peer. */
    ble_acqs_evt_handler_t   	evt_handler;    /**< Application event handler to be called when there is an event related to the LED Button service. */
    ble_srv_error_handler_t   	error_handler;  /**< Function to be called in case of an error. */
    nrf_ble_gq_t            	*p_gatt_queue;  /**< Pointer to the BLE GATT Queue instance. */
};

typedef struct {
    ble_acqs_evt_handler_t      evt_handler;    /**< Event handler to be called by the LED Button Client module when there is an event related to the LED Button Service. */
    ble_srv_error_handler_t     error_handler;  /**< Function to be called in case of an error. */
    nrf_ble_gq_t                *p_gatt_queue;  /**< Pointer to the BLE GATT Queue instance. */
} ble_acqs_init_t;

uint32_t ble_acqs_init(ble_acqs_t* p_ble_acqs, ble_acqs_init_t* p_ble_acqs_init);

void ble_acqs_on_db_disc_evt(ble_acqs_t * p_ble_acqs, const ble_db_discovery_evt_t * p_evt);

void ble_acqs_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);

uint32_t ble_acqs_sine_notif_enable(ble_acqs_t * p_ble_acqs);

uint32_t ble_acqs_counter_notif_enable(ble_acqs_t * p_ble_acqs);

uint32_t ble_acqs_handles_assign(ble_acqs_t                 *p_ble_acqs,
                                 uint16_t                   conn_handle,
                                 ble_acqs_handles_t const   *p_peer_handles);

#endif