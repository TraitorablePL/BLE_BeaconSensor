#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_dis.h"
#include "ble_acqs.h"
#include "ble_conn_params.h"
#include "boards.h"
#include "softdevice_handler.h"
#include "app_timer.h"
#include "bsp.h"
#include "nrf_delay.h"
#include "bsp_btn_ble.h"
#include "peer_manager.h"
#include "fds.h"
#include "fstorage.h"
#include "nrf_ble_gatt.h"
#include "ble_conn_state.h"

#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define IS_SRVC_CHANGED_CHARACT_PRESENT		1                                           /**< Include or not the service_changed characteristic. if not enabled, the server's database cannot be changed for the lifetime of the device*/

#define DEVICE_NAME                      	"ACQ Beacon"                                /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME                	"NordicSemiconductor"                       /**< Manufacturer. Will be passed to Device Information Service. */
#define APP_ADV_INTERVAL                 	160                                         /**< The advertising interval (in units of 0.625 ms. This value corresponds to 100 ms). */

#define APP_TIMER_PRESCALER              	0                                           /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE          	4                                           /**< Size of timer operation queues. */

#define MIN_CONN_INTERVAL                	MSEC_TO_UNITS(180, UNIT_1_25_MS)            /**< Minimum acceptable connection interval (0.4 seconds). */
#define MAX_CONN_INTERVAL                	MSEC_TO_UNITS(220, UNIT_1_25_MS)            /**< Maximum acceptable connection interval (0.65 second). */

#define SLAVE_LATENCY                    	0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                 	MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY   	APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)  /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY    	APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT     	3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_BOND                   	1                                           /**< Perform bonding. */
#define SEC_PARAM_MITM                   	0                                           /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                   	0                                           /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS               	0                                           /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES        	BLE_GAP_IO_CAPS_NONE                        /**< No I/O capabilities. */
#define SEC_PARAM_OOB                    	0                                           /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE           	7                                           /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE           	16                                          /**< Maximum encryption key size. */

#define DEAD_BEEF                        	0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define APP_FEATURE_NOT_SUPPORTED			BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2        /**< Reply when unsupported features are requested. */

#define SINE_TIMER_INTERVAL 				APP_TIMER_TICKS(500, APP_TIMER_PRESCALER) 	/**< 200 ms interval */
#define COUNTER_TIMER_INTERVAL 				APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER) 	/**< 1000 ms interval */

static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID; 								/**< Connection Handle */

static ble_acqs_t m_acqs;																/**< Aquisition Service Structure */
static uint16_t counter;
static uint16_t degree;

static nrf_ble_gatt_t m_gatt;                             								/**< Structure for gatt module */

APP_TIMER_DEF(m_sine_timer_id);
APP_TIMER_DEF(m_counter_timer_id);

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name){
	
	app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

void advertising_start(void){
	
	ret_code_t err_code;
	err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
	APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling File Data Storage events.
 *
 * @param[in] p_evt  Peer Manager event.
 * @param[in] cmd
 */
static void fds_evt_handler(fds_evt_t const * const p_evt){

	if (p_evt->id == FDS_EVT_GC){
		NRF_LOG_DEBUG("GC completed\n");
	}
}

/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const * p_evt){
	
	ret_code_t err_code;

	switch (p_evt->evt_id)
	{
		case PM_EVT_BONDED_PEER_CONNECTED:
			NRF_LOG_INFO("Connected to a previously bonded device.\r\n");
			break;

		case PM_EVT_CONN_SEC_SUCCEEDED:
			NRF_LOG_INFO(	"Connection secured. Role: %d. conn_handle: %d, Procedure: %d\r\n",
							ble_conn_state_role(p_evt->conn_handle),
							p_evt->conn_handle,
							p_evt->params.conn_sec_succeeded.procedure);
			break;

		case PM_EVT_CONN_SEC_FAILED:
			/* Often, when securing fails, it shouldn't be restarted, for security reasons.
			* Other times, it can be restarted directly.
			* Sometimes it can be restarted, but only after changing some Security Parameters.
			* Sometimes, it cannot be restarted until the link is disconnected and reconnected.
			* Sometimes it is impossible, to secure the link, or the peer device does not support it.
			* How to handle this error is highly application dependent. */
			break;

		case PM_EVT_CONN_SEC_CONFIG_REQ:
			{
				// Reject pairing request from an already bonded peer.
				pm_conn_sec_config_t conn_sec_config = {.allow_repairing = false};
				pm_conn_sec_config_reply(p_evt->conn_handle, &conn_sec_config);
			} 
			break;

		case PM_EVT_STORAGE_FULL:
			// Run garbage collection on the flash.
			err_code = fds_gc();
			if (err_code == FDS_ERR_BUSY || err_code == FDS_ERR_NO_SPACE_IN_QUEUES){
				// Retry.
			}
			else{
				APP_ERROR_CHECK(err_code);
			}
			break;

		case PM_EVT_PEERS_DELETE_SUCCEEDED:
			advertising_start();
			break;

		case PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED:
			// The local database has likely changed, send service changed indications.
			pm_local_database_has_changed();
			break;

		case PM_EVT_PEER_DATA_UPDATE_FAILED:
			// Assert.
			APP_ERROR_CHECK(p_evt->params.peer_data_update_failed.error);
			break;

		case PM_EVT_PEER_DELETE_FAILED:
			// Assert.
			APP_ERROR_CHECK(p_evt->params.peer_delete_failed.error);
			break;

		case PM_EVT_PEERS_DELETE_FAILED:
			// Assert.
			APP_ERROR_CHECK(p_evt->params.peers_delete_failed_evt.error);
			break;

		case PM_EVT_ERROR_UNEXPECTED:
			// Assert.
			APP_ERROR_CHECK(p_evt->params.error_unexpected.error);
			break;

		case PM_EVT_CONN_SEC_START:
		case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
		case PM_EVT_PEER_DELETE_SUCCEEDED:
		case PM_EVT_LOCAL_DB_CACHE_APPLIED:
		case PM_EVT_SERVICE_CHANGED_IND_SENT:
		case PM_EVT_SERVICE_CHANGED_IND_CONFIRMED:
		default:
				break;
	}
}

static void sine_timeout_handler(void* p_context){
	
	float sine = sine_value_get(degree);
	//float test = 0.125;
	sine_characteristic_notify(&m_acqs, &sine);
	degree++;
}

static void counter_timeout_handler(void* p_context){
	
	counter_characteristic_notify(&m_acqs, &counter);
	counter++;
}

/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timers_init(void){
	
	uint32_t err_code;
	
	// Initialize timer module.
	APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);

	// Create timers.
	
	err_code = app_timer_create(&m_sine_timer_id,
								APP_TIMER_MODE_REPEATED,
								sine_timeout_handler);
	APP_ERROR_CHECK(err_code);
	
	err_code = app_timer_create(&m_counter_timer_id,
								APP_TIMER_MODE_REPEATED,
								counter_timeout_handler);
	APP_ERROR_CHECK(err_code);
	
}

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void){
	
	uint32_t err_code;
	ble_gap_conn_params_t gap_conn_params;
	ble_gap_conn_sec_mode_t sec_mode;	//security level

	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

	err_code = sd_ble_gap_device_name_set(	&sec_mode,
											(const uint8_t *)DEVICE_NAME,
											strlen(DEVICE_NAME));
										
	APP_ERROR_CHECK(err_code);

	err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_UNKNOWN);
	APP_ERROR_CHECK(err_code);

	memset(&gap_conn_params, 0, sizeof(gap_conn_params));

	gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
	gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
	gap_conn_params.slave_latency = SLAVE_LATENCY;
	gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

	err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
	APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void){
	
	uint32_t err_code;
	ble_dis_init_t dis_init;
	
	NRF_LOG_INFO("SERVICES INIT\r\n");
	
	// Initialize Aquisition Service
	ble_acqs_init(&m_acqs);

	// Initialize Device Information Service.
	memset(&dis_init, 0, sizeof(dis_init));

	ble_srv_ascii_to_utf8(&dis_init.manufact_name_str, (char *)MANUFACTURER_NAME);

	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dis_init.dis_attr_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&dis_init.dis_attr_md.write_perm);

	err_code = ble_dis_init(&dis_init);
	APP_ERROR_CHECK(err_code);
}

static void application_timers_start(void){
	
	uint32_t err_code;

	err_code = app_timer_start(m_sine_timer_id, SINE_TIMER_INTERVAL, NULL);
	APP_ERROR_CHECK(err_code);
	
	err_code = app_timer_start(m_counter_timer_id, COUNTER_TIMER_INTERVAL, NULL);
	APP_ERROR_CHECK(err_code);
	
}

/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt){
	
	uint32_t err_code;

	if(p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED){
		err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
		APP_ERROR_CHECK(err_code);
	}
}

/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error){
	
	APP_ERROR_HANDLER(nrf_error);
}

static void conn_params_init(void){
	
	uint32_t err_code;
	ble_conn_params_init_t cp_init;

	memset(&cp_init, 0, sizeof(cp_init));

	cp_init.p_conn_params = NULL;
	cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
	cp_init.next_conn_params_update_delay = NEXT_CONN_PARAMS_UPDATE_DELAY;
	cp_init.max_conn_params_update_count = MAX_CONN_PARAMS_UPDATE_COUNT;
	cp_init.disconnect_on_fail = false;
	cp_init.evt_handler = on_conn_params_evt;
	cp_init.error_handler = conn_params_error_handler;

	err_code = ble_conn_params_init(&cp_init);
	APP_ERROR_CHECK(err_code);
}

/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */

/*
static void sleep_mode_enter(void){
	
	uint32_t err_code = bsp_indication_set(BSP_INDICATE_IDLE);

	APP_ERROR_CHECK(err_code);

	// Prepare wakeup buttons.
	err_code = bsp_btn_ble_sleep_mode_prepare();
	APP_ERROR_CHECK(err_code);

	// Go to system-off mode (this function will not return; wakeup will cause a reset).
	err_code = sd_power_system_off();
	APP_ERROR_CHECK(err_code);
}
*/

/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt){
	
	uint32_t err_code;

	switch (ble_adv_evt){
		case BLE_ADV_EVT_FAST:
			NRF_LOG_INFO("Fast Advertising\r\n");
			err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
			APP_ERROR_CHECK(err_code);
			break;

		case BLE_ADV_EVT_IDLE:
			//sleep_mode_enter();
			break;

		default:
			break;
	}
}

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void on_ble_evt(ble_evt_t * p_ble_evt){
	
	uint32_t err_code;

	switch (p_ble_evt->header.evt_id){
		case BLE_GAP_EVT_CONNECTED:
			NRF_LOG_INFO("Connected\r\n");
			err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
			APP_ERROR_CHECK(err_code);
			m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
			break;

		case BLE_GAP_EVT_DISCONNECTED:
			NRF_LOG_INFO(	"Disconnected, reason %d\r\n",
							p_ble_evt->evt.gap_evt.params.disconnected.reason);
			m_conn_handle = BLE_CONN_HANDLE_INVALID;
			break;

		case BLE_GATTC_EVT_TIMEOUT:
			NRF_LOG_DEBUG("GATT Client Timeout.\r\n");
			err_code = sd_ble_gap_disconnect(	p_ble_evt->evt.gattc_evt.conn_handle,
												BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
			APP_ERROR_CHECK(err_code);
			break;

		case BLE_GATTS_EVT_TIMEOUT:
			NRF_LOG_DEBUG("GATT Server Timeout.\r\n");
			err_code = sd_ble_gap_disconnect(	p_ble_evt->evt.gatts_evt.conn_handle,
												BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
			APP_ERROR_CHECK(err_code);
			break;

		case BLE_EVT_USER_MEM_REQUEST:
			err_code = sd_ble_user_mem_reply(m_conn_handle, NULL);
			APP_ERROR_CHECK(err_code);
			break;

		case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
			{
				ble_gatts_evt_rw_authorize_request_t req;
				ble_gatts_rw_authorize_reply_params_t auth_reply;

				req = p_ble_evt->evt.gatts_evt.params.authorize_request;

				if(req.type != BLE_GATTS_AUTHORIZE_TYPE_INVALID){
					
					if(	(req.request.write.op == BLE_GATTS_OP_PREP_WRITE_REQ) ||
						(req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW) ||
						(req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL)){
								
						if(req.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE){
							auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
						}
						else{
							auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
						}
						auth_reply.params.write.gatt_status = APP_FEATURE_NOT_SUPPORTED;
						err_code = sd_ble_gatts_rw_authorize_reply(	p_ble_evt->evt.gatts_evt.conn_handle,
																	&auth_reply);
						APP_ERROR_CHECK(err_code);
					}
				}
			} 
			break;

		default:
			break;
	}
}

/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the BLE Stack event interrupt handler after a BLE stack
 *          event has been received.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt){
	
	ble_conn_state_on_ble_evt(p_ble_evt);
	pm_on_ble_evt(p_ble_evt);
	ble_acqs_on_ble_evt(p_ble_evt, &m_acqs);
	ble_conn_params_on_ble_evt(p_ble_evt);
	bsp_btn_ble_on_ble_evt(p_ble_evt);
	on_ble_evt(p_ble_evt);
	ble_advertising_on_ble_evt(p_ble_evt);
	nrf_ble_gatt_on_ble_evt(&m_gatt, p_ble_evt);
}

/**@brief Function for dispatching a system event to interested modules.
 *
 * @details This function is called from the System event interrupt handler after a system
 *          event has been received.
 *
 * @param[in] sys_evt  System stack event.
 */
static void sys_evt_dispatch(uint32_t sys_evt){
	
	// Dispatch the system event to the fstorage module, where it will be
	// dispatched to the Flash Data Storage (FDS) module.
	fs_sys_event_handler(sys_evt);

	// Dispatch to the Advertising module last, since it will check if there are any
	// pending flash operations in fstorage. Let fstorage process system events first,
	// so that it can report correctly to the Advertising module.
	ble_advertising_on_sys_evt(sys_evt);
}

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void){
	
	uint32_t err_code;
	nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

	// Initialize the SoftDevice handler module.
	SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

	ble_enable_params_t ble_enable_params;
	err_code = softdevice_enable_get_default_config(NRF_BLE_CENTRAL_LINK_COUNT,
													NRF_BLE_PERIPHERAL_LINK_COUNT,
													&ble_enable_params);
	APP_ERROR_CHECK(err_code);

	// Check the ram settings against the used number of links
	CHECK_RAM_START_ADDR(NRF_BLE_CENTRAL_LINK_COUNT, NRF_BLE_PERIPHERAL_LINK_COUNT);

	// Enable BLE stack.
#if (NRF_SD_BLE_API_VERSION == 3)
	ble_enable_params.gatt_enable_params.att_mtu = NRF_BLE_GATT_MAX_MTU_SIZE;
#endif
	err_code = softdevice_enable(&ble_enable_params);
	APP_ERROR_CHECK(err_code);

	// Register with the SoftDevice handler module for BLE events.
	err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
	APP_ERROR_CHECK(err_code);

	// Register with the SoftDevice handler module for BLE events.
	err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
	APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated by button press.
 */
void bsp_event_handler(bsp_event_t event){

	uint32_t err_code;

	switch (event){
		case BSP_EVENT_SLEEP:
			//sleep_mode_enter();
			break;

		case BSP_EVENT_DISCONNECT:
			err_code = sd_ble_gap_disconnect(	m_conn_handle,
												BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
			if (err_code != NRF_ERROR_INVALID_STATE){
				APP_ERROR_CHECK(err_code);
			}
			break;

		case BSP_EVENT_WHITELIST_OFF:
			if (m_conn_handle == BLE_CONN_HANDLE_INVALID){
				err_code = ble_advertising_restart_without_whitelist();
				
				if (err_code != NRF_ERROR_INVALID_STATE){
					APP_ERROR_CHECK(err_code);
				}
			}
			break;

		default:
			break;
	}
}

/**@brief Function for the Peer Manager initialization.
 *
 * @param[in] erase_bonds  Indicates whether bonding information should be cleared from
 *                         persistent storage during initialization of the Peer Manager.
 */
static void peer_manager_init(){

	ble_gap_sec_params_t sec_param;
	ret_code_t err_code;

	err_code = pm_init();
	APP_ERROR_CHECK(err_code);

	memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

	// Security parameters to be used for all security procedures.
	sec_param.bond = SEC_PARAM_BOND;
	sec_param.mitm = SEC_PARAM_MITM;
	sec_param.io_caps = SEC_PARAM_IO_CAPABILITIES;
	sec_param.oob = SEC_PARAM_OOB;
	sec_param.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
	sec_param.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
	sec_param.kdist_own.enc = 1;
	sec_param.kdist_own.id = 1;
	sec_param.kdist_peer.enc = 1;
	sec_param.kdist_peer.id = 1;

	err_code = pm_sec_params_set(&sec_param);
	APP_ERROR_CHECK(err_code);

	err_code = pm_register(pm_evt_handler);
	APP_ERROR_CHECK(err_code);

	err_code = fds_register(fds_evt_handler);
	APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void){
	
	uint32_t err_code;
	ble_advdata_t advdata;
	ble_adv_modes_config_t options;

	// Build advertising data struct to pass into @ref ble_advertising_init.
	memset(&advdata, 0, sizeof(advdata));

	advdata.name_type = BLE_ADVDATA_FULL_NAME;
	advdata.include_appearance = false;
	advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

	memset(&options, 0, sizeof(options));
	options.ble_adv_fast_enabled = true;
	options.ble_adv_fast_interval = APP_ADV_INTERVAL;
	options.ble_adv_fast_timeout = 0;

	err_code = ble_advertising_init(&advdata, NULL, &options, on_adv_evt, NULL);
	APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the application up.
 */
static void buttons_leds_init(void){
	
	bsp_event_t startup_event;

	uint32_t err_code = bsp_init(	BSP_INIT_LED | BSP_INIT_BUTTONS,
									APP_TIMER_TICKS(100, APP_TIMER_PRESCALER),
									bsp_event_handler);

	APP_ERROR_CHECK(err_code);

	err_code = bsp_btn_ble_init(NULL, &startup_event);
	APP_ERROR_CHECK(err_code);
}

/**@brief Function for the Power manager.
 */
static void power_manage(void){
	
	uint32_t err_code = sd_app_evt_wait();
	APP_ERROR_CHECK(err_code);
}

/**@brief Function for application main entry.
 */
int main(void){
	
	uint32_t err_code;

	// Initialize.
	err_code = NRF_LOG_INIT(NULL);
	APP_ERROR_CHECK(err_code);

	timers_init();
	buttons_leds_init();
	ble_stack_init();
	peer_manager_init();
	gap_params_init();
	advertising_init();
	sine_table_init();
	services_init();
	conn_params_init();

	// Start execution.
	NRF_LOG_INFO("Application initialized!\r\n");
	application_timers_start();
	advertising_start();

	// Enter main loop.
	while(1){
		if(NRF_LOG_PROCESS() == false){
			power_manage();
		}
	}
}
