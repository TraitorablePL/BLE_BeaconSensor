#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf_sdm.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_db_discovery.h"
#include "ble_srv_common.h"
#include "nrf.h"
#include "nrf_drv_usbd.h"
#include "nrf_drv_clock.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"
#include "nrf_pwr_mgmt.h"
#include "app_util.h"
#include "app_error.h"

#include "app_usbd_cdc_acm.h"
#include "app_usbd_serial_num.h"
#include "app_fifo.h"

#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "ble_acqs_client.h"
#include "app_util.h"
#include "app_timer.h"
#include "fds.h"
#include "nrf_fstorage.h"
#include "ble_conn_state.h"
#include "nrf_ble_gatt.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_ble_scan.h"

#define APP_BLE_CONN_CFG_TAG        1                                   /**< A tag identifying the SoftDevice BLE configuration. */

#define APP_BLE_OBSERVER_PRIO       3                                   /**< Application's BLE observer priority. You shouldn't need to modify this value. */

#define LED_MAIN_GREEN      (BSP_BOARD_LED_0)
#define LED_RGB_RED         (BSP_BOARD_LED_1)
#define LED_RGB_GREEN       (BSP_BOARD_LED_2)
#define LED_RGB_BLUE        (BSP_BOARD_LED_3)

#ifndef USBD_POWER_DETECTION
#define USBD_POWER_DETECTION true
#endif

#define CDC_ACM_COMM_INTERFACE  0
#define CDC_ACM_COMM_EPIN       NRF_DRV_USBD_EPIN2

#define CDC_ACM_DATA_INTERFACE  1
#define CDC_ACM_DATA_EPIN       NRF_DRV_USBD_EPIN1
#define CDC_ACM_DATA_EPOUT      NRF_DRV_USBD_EPOUT1



static void cdc_acm_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                    app_usbd_cdc_acm_user_event_t event);

BLE_ACQS_DEF(m_acqs); 
NRF_BLE_GATT_DEF(m_gatt);                                           /**< GATT module instance. */
BLE_DB_DISCOVERY_DEF(m_db_disc);                                    /**< DB discovery module instance. */
NRF_BLE_SCAN_DEF(m_scan);                                           /**< Scanning module instance. */
NRF_BLE_GQ_DEF(m_ble_gatt_queue,                                    /**< BLE GATT Queue instance. */
               NRF_SDH_BLE_CENTRAL_LINK_COUNT,
               NRF_BLE_GQ_QUEUE_SIZE);

APP_USBD_CDC_ACM_GLOBAL_DEF(m_app_cdc_acm,
                            cdc_acm_user_ev_handler,
                            CDC_ACM_COMM_INTERFACE,
                            CDC_ACM_DATA_INTERFACE,
                            CDC_ACM_COMM_EPIN,
                            CDC_ACM_DATA_EPIN,
                            CDC_ACM_DATA_EPOUT,
                            APP_USBD_CDC_COMM_PROTOCOL_AT_V250
);

#define FIFO_SIZE               512

static app_fifo_t   m_fifo;                             // FIFO instance for TX USBD message
static char         m_fifo_string[NRF_DRV_USBD_EPSIZE]; // FIFO string
static char         m_fifo_buffer[FIFO_SIZE];           // FIFO TX buffer
static char         m_tx_char;                          // Transmitted character
static bool         fifo_empty = true;                // FIFO empty flag

static uint16_t m_conn_handle;                                      /**< Current connection handle. */

static char const m_target_periph_name[] = "ACQ Beacon";            /**< Target advertising name */

/**< Scan parameters requested for scanning and connection. */
static ble_gap_scan_params_t const m_scan_param = {
    .active        = 0x01,
    .interval      = NRF_BLE_SCAN_SCAN_INTERVAL,
    .window        = NRF_BLE_SCAN_SCAN_WINDOW,
    .filter_policy = BLE_GAP_SCAN_FP_ACCEPT_ALL, //Accept all advertising packets except directed advertising packets
    .timeout       = 3000,
    .scan_phys     = BLE_GAP_PHY_1MBPS,
};

static void scan_start(void);

static void write_message(const char*, uint32_t);

/**@brief Function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num     Line number of the failing ASSERT call.
 * @param[in] p_file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name) {

    app_error_handler(0xDEADBEEF, line_num, p_file_name);
}

/**@brief Function for handling the Heart Rate Service Client and Battery Service Client errors.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void service_error_handler(uint32_t nrf_error) {

    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for handling database discovery events.
 *
 * @details This function is callback function to handle events from the database discovery module.
 *          Depending on the UUIDs that are discovered, this function should forward the events
 *          to their respective services.
 *
 * @param[in] p_event  Pointer to the database discovery event.
 */
static void db_disc_handler(ble_db_discovery_evt_t * p_evt) {

    ble_acqs_on_db_disc_evt(&m_acqs, p_evt);
}

/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const * p_evt) {

    pm_handler_on_pm_evt(p_evt);
    pm_handler_flash_clean(p_evt);

    switch (p_evt->evt_id) {
        default:
            break;
    }
}

/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context) {

    ret_code_t            err_code;
    uint32_t len;
    ble_gap_evt_t const * p_gap_evt = &p_ble_evt->evt.gap_evt;

    switch (p_ble_evt->header.evt_id) {

        case BLE_GAP_EVT_CONNECTED:
        {
            NRF_LOG_INFO("Connected to the device.");
#ifdef BOARD_PCA10059
            len = sprintf(m_fifo_string, "Connected to the device.\r\n");
            write_message(m_fifo_string, len);
#endif
            // Discover peer's services.
            err_code = ble_db_discovery_start(&m_db_disc, p_ble_evt->evt.gap_evt.conn_handle);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GAP_EVT_DISCONNECTED:
        {
            NRF_LOG_INFO("Disconnected, reason 0x%x.", p_gap_evt->params.disconnected.reason);

#ifdef BOARD_PCA10059
            len = sprintf(m_fifo_string, "Disconnected, reason 0x%x.\r\n", p_gap_evt->params.disconnected.reason);
            write_message(m_fifo_string, len);
#endif
            if (ble_conn_state_central_conn_count() < NRF_SDH_BLE_CENTRAL_LINK_COUNT) {
                scan_start();
            }
        } break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
#ifdef BOARD_PCA10059
            len = sprintf(m_fifo_string, "Param update request.\r\n");
            write_message(m_fifo_string, len);
#endif
            err_code = sd_ble_gap_conn_param_update(p_gap_evt->conn_handle,
                                                    &p_gap_evt->params.conn_param_update_request.conn_params);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_TIMEOUT:
            if (p_gap_evt->params.timeout.src == BLE_GAP_TIMEOUT_SRC_CONN) {
                NRF_LOG_DEBUG("Connection Request timed out.");
#ifdef BOARD_PCA10059
                len = sprintf(m_fifo_string, "Connection Request timed out.\r\n");
                write_message(m_fifo_string, len);
#endif
            }
            break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.");
#ifdef BOARD_PCA10059
            len = sprintf(m_fifo_string, "GATT Client Timeout.\r\n");
            write_message(m_fifo_string, len);
#endif
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
#ifdef BOARD_PCA10059
            len = sprintf(m_fifo_string, "GATT Server Timeout.\r\n");
            write_message(m_fifo_string, len);
#endif
            //NRF_LOG_DEBUG("GATT Server Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        default:
            break;
    }
}

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void) {

    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register handlers for BLE and SoC events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}

/**@brief Function for the Peer Manager initialization.
 */
static void peer_manager_init(void) {

    ble_gap_sec_params_t sec_param;
    ret_code_t err_code;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // New configuration (Only pairing)
    // Security parameters to be used for all security procedures.
    sec_param.bond = false;
    sec_param.mitm = false;
    sec_param.lesc = 0;
    sec_param.keypress = 0;
    sec_param.io_caps = BLE_GAP_IO_CAPS_NONE;
    sec_param.oob = false;
    sec_param.min_key_size = 7;
    sec_param.max_key_size = 16;
    sec_param.kdist_own.enc = 0;
    sec_param.kdist_own.id = 0;
    sec_param.kdist_peer.enc = 0;
    sec_param.kdist_peer.id = 0;

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}

static void acqs_evt_handler(ble_acqs_t * p_acqs, ble_acqs_evt_t * p_acqs_evt) {

    ret_code_t err_code;
    uint32_t len;

    switch (p_acqs_evt->evt_type) {

        case BLE_ACQS_EVT_DISCOVERY_COMPLETE:
            NRF_LOG_INFO("Acquisition service discovered.");
#ifdef BOARD_PCA10059
            len = sprintf(m_fifo_string, "Acquisition service discovered.\r\n");
            write_message(m_fifo_string, len);
#endif
            err_code = ble_acqs_handles_assign(p_acqs, p_acqs_evt->conn_handle, &p_acqs_evt->params.handles);
            APP_ERROR_CHECK(err_code);

            //ACQ service discovered. Enable notifications.
            err_code = ble_acqs_accx_notif_enable(p_acqs);
            APP_ERROR_CHECK(err_code);
            err_code = ble_acqs_accy_notif_enable(p_acqs);
            APP_ERROR_CHECK(err_code);
            err_code = ble_acqs_accz_notif_enable(p_acqs);
            APP_ERROR_CHECK(err_code);
            err_code = ble_acqs_temp_notif_enable(p_acqs);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_ACQS_EVT_TEMP_NOTIFICATION:
            NRF_LOG_INFO("Temperature = "NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(p_acqs_evt->params.temp));
#ifdef BOARD_PCA10059
            len = sprintf(m_fifo_string, "T:"NRF_LOG_FLOAT_MARKER"\r\n", NRF_LOG_FLOAT(p_acqs_evt->params.temp));
            write_message(m_fifo_string, len);
#endif
            break;

        case BLE_ACQS_EVT_ACCX_NOTIFICATION:
            NRF_LOG_INFO("Acceleration X = %d", p_acqs_evt->params.acc);
            
#ifdef BOARD_PCA10059
            len = sprintf(m_fifo_string, "X:%d\r\n", p_acqs_evt->params.acc);
            write_message(m_fifo_string, len);
#endif
            break;

        case BLE_ACQS_EVT_ACCY_NOTIFICATION:
            NRF_LOG_INFO("Acceleration Y = %d", p_acqs_evt->params.acc);
#ifdef BOARD_PCA10059
            len = sprintf(m_fifo_string, "Y:%d\r\n", p_acqs_evt->params.acc);
            write_message(m_fifo_string, len);
#endif
            break;

        case BLE_ACQS_EVT_ACCZ_NOTIFICATION:
            NRF_LOG_INFO("Acceleration Z = %d", p_acqs_evt->params.acc);
#ifdef BOARD_PCA10059
            len = sprintf(m_fifo_string, "Z:%d\r\n", p_acqs_evt->params.acc);
            write_message(m_fifo_string, len);
#endif
            break;

        case BLE_ACQS_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected");
#ifdef BOARD_PCA10059
            len = sprintf(m_fifo_string, "Disconnected\r\n");
            write_message(m_fifo_string, len);
#endif
            scan_start();
            break;

        default:
            break;
    }
}

static void acqs_init(void) {

    ble_acqs_init_t acqs_init_obj;

    acqs_init_obj.evt_handler   = acqs_evt_handler;
    acqs_init_obj.error_handler = service_error_handler;
    acqs_init_obj.p_gatt_queue  = &m_ble_gatt_queue;

    ret_code_t err_code = ble_acqs_init(&m_acqs, &acqs_init_obj);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief Database discovery collector initialization.
 */
static void db_discovery_init(void) {

    ble_db_discovery_init_t db_init;

    memset(&db_init, 0, sizeof(db_init));

    db_init.evt_handler  = db_disc_handler;
    db_init.p_gatt_queue = &m_ble_gatt_queue;

    ret_code_t err_code = ble_db_discovery_init(&db_init);

    APP_ERROR_CHECK(err_code);
}

/**@brief Function to start scanning.
 */
static void scan_start(void) {

    ret_code_t err_code;

    NRF_LOG_INFO("Starting scan.");

    err_code = nrf_ble_scan_start(&m_scan);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the nrf log module.
 */
static void log_init(void) {

    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/**@brief Function for initializing the power management module. */
static void power_management_init(void) {

    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}

/**@brief GATT module event handler.
 */
static void gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt) {

    switch (p_evt->evt_id) {
        case NRF_BLE_GATT_EVT_ATT_MTU_UPDATED:
            NRF_LOG_INFO("GATT ATT MTU on connection 0x%x changed to %d.", p_evt->conn_handle, p_evt->params.att_mtu_effective);
            break;

        case NRF_BLE_GATT_EVT_DATA_LENGTH_UPDATED:
            NRF_LOG_INFO("Data length for connection 0x%x updated to %d.", p_evt->conn_handle, p_evt->params.data_length);
            break;

        default:
            break;
    }
}

static void scan_evt_handler(scan_evt_t const * p_scan_evt) {

    uint32_t err_code;
    switch(p_scan_evt->scan_evt_id) {

        case NRF_BLE_SCAN_EVT_CONNECTING_ERROR:
            err_code = p_scan_evt->params.connecting_err.err_code;
            APP_ERROR_CHECK(err_code);
            break;

        case NRF_BLE_SCAN_EVT_SCAN_TIMEOUT:
            scan_start();
            break;

        default:
            break;
    }
}

/**@brief Function for initializing the timer.
 */
static void timer_init(void) {

    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the GATT module.
 */
static void gatt_init(void) {

    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initialization scanning and setting filters.
 */
static void scan_init(void) {

    ret_code_t          err_code;
    nrf_ble_scan_init_t init_scan;

    memset(&init_scan, 0, sizeof(init_scan));

    init_scan.p_scan_param     = &m_scan_param;
    init_scan.connect_if_match = true;
    init_scan.conn_cfg_tag     = APP_BLE_CONN_CFG_TAG;

    err_code = nrf_ble_scan_init(&m_scan, &init_scan, scan_evt_handler);
    APP_ERROR_CHECK(err_code);


    err_code = nrf_ble_scan_filter_set( &m_scan,
                                        SCAN_NAME_FILTER,
                                        m_target_periph_name);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_scan_filters_enable(&m_scan,
                                           NRF_BLE_SCAN_NAME_FILTER,
                                           false);
    APP_ERROR_CHECK(err_code);

}

/**@brief Function for handling the idle state (main loop).
 *
 * @details Handle any pending log operation(s), then sleep until the next event occurs.
 */
static void idle_state_handle(void) {
    
    NRF_LOG_FLUSH();
    nrf_pwr_mgmt_run();
}

/**
 * @brief User event handler @ref app_usbd_cdc_acm_user_ev_handler_t (headphones)
 * */
static void cdc_acm_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                    app_usbd_cdc_acm_user_event_t event) {
    uint32_t err_code;
    char rx_buffer;

    switch (event) {
        case APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN:
            /*Setup first transfer*/
            app_usbd_cdc_acm_read(&m_app_cdc_acm, &rx_buffer, 1);
            break;

        case APP_USBD_CDC_ACM_USER_EVT_TX_DONE:
            if(!fifo_empty){
                err_code = app_fifo_get(&m_fifo, &m_tx_char);
                if(err_code == NRF_ERROR_NOT_FOUND) {
                    fifo_empty = true;
                }
                else {
                    app_usbd_cdc_acm_write(&m_app_cdc_acm, &m_tx_char, 1);
                    fifo_empty = false;
                }
            }
            break;

        case APP_USBD_CDC_ACM_USER_EVT_RX_DONE:
            do {
                /* Fetch data until internal buffer is empty */
                err_code = app_usbd_cdc_acm_read(&m_app_cdc_acm, &rx_buffer, 1);
            } 
            while (err_code == NRF_SUCCESS);
            break;

        default:
            break;
    }
}

static void usbd_user_ev_handler(app_usbd_event_type_t event) {
    switch (event) {
        case APP_USBD_EVT_STOPPED:
            app_usbd_disable();
            break;

        case APP_USBD_EVT_POWER_DETECTED:
            if (!nrf_drv_usbd_is_enabled())
                app_usbd_enable();
            break;

        case APP_USBD_EVT_POWER_REMOVED:
            app_usbd_stop();
            break;

        case APP_USBD_EVT_POWER_READY:
            app_usbd_start();
            break;

        default:
            break;
    }
}

static void write_message(const char* message, uint32_t len){
    app_fifo_write(&m_fifo, message, &len);
    if(fifo_empty){
        app_fifo_get(&m_fifo, &m_tx_char);
        app_usbd_cdc_acm_write(&m_app_cdc_acm, &m_tx_char, 1);
        fifo_empty = false;
    }
}

int main(void) {
    
    static const app_usbd_config_t usbd_config = {
        .ev_state_proc = usbd_user_ev_handler
    };

    nrf_drv_clock_init();
    log_init();
    timer_init();

    app_usbd_serial_num_generate();
    app_usbd_init(&usbd_config);
    app_usbd_class_inst_t const* class_cdc_acm = app_usbd_cdc_acm_class_inst_get(&m_app_cdc_acm);
    app_usbd_class_append(class_cdc_acm);

    app_fifo_init(&m_fifo, m_fifo_buffer, FIFO_SIZE);
    
    power_management_init();
    ble_stack_init();
    gatt_init();
    peer_manager_init();
    db_discovery_init();
    acqs_init();
    scan_init();

    if (USBD_POWER_DETECTION) {
        app_usbd_power_events_enable();
    }
    else {
        app_usbd_enable();
        app_usbd_start();
    }

    scan_start();

    while(true){
        while(app_usbd_event_queue_process()){}
        idle_state_handle();
    }
}