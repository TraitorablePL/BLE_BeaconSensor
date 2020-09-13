PROJECT_NAME     := usbd_ble_uart_pca10056_s140
TARGETS          := nrf52840_xxaa
OUTPUT_DIRECTORY := _build

SDK_ROOT := ../../../../../..
PROJ_DIR := ../../..

$(OUTPUT_DIRECTORY)/nrf52840_xxaa.out: \
  LINKER_SCRIPT  := nrf51840_xxaa.ld

# Source files common to all targets
SRC_FILES += \
  modules/nrfx/mdk/gcc_startup_nrf52840.S \
  components/libraries/log/src/nrf_log_backend_rtt.c \
  components/libraries/log/src/nrf_log_backend_serial.c \
  components/libraries/log/src/nrf_log_backend_uart.c \
  components/libraries/log/src/nrf_log_default_backends.c \
  components/libraries/log/src/nrf_log_frontend.c \
  components/libraries/log/src/nrf_log_str_formatter.c \
  components/libraries/button/app_button.c \
  components/libraries/util/app_error.c \
  components/libraries/util/app_error_handler_gcc.c \
  components/libraries/util/app_error_weak.c \
  components/libraries/fifo/app_fifo.c \
  components/libraries/scheduler/app_scheduler.c \
  components/libraries/timer/app_timer2.c \
  components/libraries/uart/app_uart_fifo.c \
  components/libraries/usbd/app_usbd.c \
  components/libraries/usbd/class/cdc/acm/app_usbd_cdc_acm.c \
  components/libraries/usbd/app_usbd_core.c \
  components/libraries/usbd/app_usbd_serial_num.c \
  components/libraries/usbd/app_usbd_string_desc.c \
  components/libraries/util/app_util_platform.c \
  components/libraries/timer/drv_rtc.c \
  components/libraries/hardfault/nrf52/handler/hardfault_handler_gcc.c \
  components/libraries/hardfault/hardfault_implementation.c \
  components/libraries/util/nrf_assert.c \
  components/libraries/atomic_fifo/nrf_atfifo.c \
  components/libraries/atomic_flags/nrf_atflags.c \
  components/libraries/atomic/nrf_atomic.c \
  components/libraries/balloc/nrf_balloc.c \
  components/libraries/cli/nrf_cli.c \
  components/libraries/cli/uart/nrf_cli_uart.c \
  external/fprintf/nrf_fprintf.c \
  external/fprintf/nrf_fprintf_format.c \
  components/libraries/memobj/nrf_memobj.c \
  components/libraries/pwr_mgmt/nrf_pwr_mgmt.c \
  components/libraries/ringbuf/nrf_ringbuf.c \
  components/libraries/experimental_section_vars/nrf_section_iter.c \
  components/libraries/sortlist/nrf_sortlist.c \
  components/libraries/strerror/nrf_strerror.c \
  components/libraries/uart/retarget.c \
  modules/nrfx/mdk/system_nrf52840.c \
  components/boards/boards.c \
  integration/nrfx/legacy/nrf_drv_clock.c \
  integration/nrfx/legacy/nrf_drv_power.c \
  integration/nrfx/legacy/nrf_drv_uart.c \
  modules/nrfx/soc/nrfx_atomic.c \
  modules/nrfx/drivers/src/nrfx_clock.c \
  modules/nrfx/drivers/src/nrfx_gpiote.c \
  modules/nrfx/drivers/src/nrfx_power.c \
  modules/nrfx/drivers/src/prs/nrfx_prs.c \
  modules/nrfx/drivers/src/nrfx_systick.c \
  modules/nrfx/drivers/src/nrfx_uart.c \
  modules/nrfx/drivers/src/nrfx_uarte.c \
  modules/nrfx/drivers/src/nrfx_usbd.c \
  components/libraries/bsp/bsp.c \
  components/libraries/bsp/bsp_cli.c \
  components/libraries/bsp/bsp_btn_ble.c \
  components/ble/nrf_ble_gq/nrf_ble_gq.c \
  components/ble/nrf_ble_scan/nrf_ble_scan.c \
  components/ble/ble_db_discovery/ble_db_discovery.c \
  main.c \
  external/segger_rtt/SEGGER_RTT.c \
  external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c \
  external/segger_rtt/SEGGER_RTT_printf.c \
  components/ble/common/ble_advdata.c \
  components/ble/ble_advertising/ble_advertising.c \
  components/ble/common/ble_conn_params.c \
  components/ble/common/ble_conn_state.c \
  components/ble/ble_link_ctx_manager/ble_link_ctx_manager.c \
  components/ble/common/ble_srv_common.c \
  components/ble/nrf_ble_gatt/nrf_ble_gatt.c \
  external/utf_converter/utf.c \
  components/ble/ble_services/ble_nus/ble_nus.c \
  components/softdevice/common/nrf_sdh.c \
  components/softdevice/common/nrf_sdh_ble.c \
  components/softdevice/common/nrf_sdh_soc.c \

# Include folders common to all targets
INC_FOLDERS += \
  components/nfc/ndef/generic/message \
  components/nfc/t2t_lib \
  components/nfc/t4t_parser/hl_detection_procedure \
  components/ble/ble_services/ble_ancs_c \
  components/nfc/platform \
  components/ble/ble_services/ble_ias_c \
  components/libraries/pwm \
  components/libraries/usbd/class/cdc/acm \
  components/libraries/usbd/class/hid/generic \
  components/libraries/usbd/class/msc \
  components/libraries/usbd/class/hid \
  modules/nrfx/hal \
  components/nfc/ndef/conn_hand_parser/le_oob_rec_parser \
  components/libraries/log \
  components/ble/ble_services/ble_gls \
  components/libraries/fstorage \
  components/nfc/ndef/text \
  components/libraries/mutex \
  components/libraries/gfx \
  components/libraries/bootloader/ble_dfu \
  components/nfc/ndef/connection_handover/common \
  components/libraries/fifo \
  components/boards \
  components/nfc/ndef/generic/record \
  components/nfc/t4t_parser/cc_file \
  components/ble/ble_advertising \
  components/ble/nrf_ble_gq \
  components/ble/nrf_ble_scan \
  components/ble/ble_db_discovery \
  external/utf_converter \
  components/ble/ble_services/ble_bas_c \
  modules/nrfx/drivers/include \
  components/libraries/experimental_task_manager \
  components/ble/ble_services/ble_hrs_c \
  components/softdevice/s140/headers/nrf52 \
  components/nfc/ndef/connection_handover/le_oob_rec \
  components/libraries/queue \
  components/libraries/pwr_mgmt \
  components/ble/ble_dtm \
  components/toolchain/cmsis/include \
  components/ble/ble_services/ble_rscs_c \
  components/ble/common \
  components/ble/ble_services/ble_lls \
  components/libraries/hardfault/nrf52 \
  components/libraries/bsp \
  components/nfc/ndef/connection_handover/ac_rec \
  components/ble/ble_services/ble_bas \
  components/libraries/mpu \
  components/libraries/experimental_section_vars \
  components/ble/ble_services/ble_ans_c \
  components/libraries/slip \
  components/libraries/delay \
  components/libraries/csense_drv \
  components/libraries/memobj \
  components/ble/ble_services/ble_nus_c \
  components/softdevice/common \
  components/ble/ble_services/ble_ias \
  components/libraries/usbd/class/hid/mouse \
  components/libraries/low_power_pwm \
  components/nfc/ndef/conn_hand_parser/ble_oob_advdata_parser \
  components/ble/ble_services/ble_dfu \
  external/fprintf \
  components/libraries/svc \
  components/libraries/atomic \
  components \
  components/libraries/scheduler \
  components/libraries/cli \
  components/ble/ble_services/ble_lbs \
  components/ble/ble_services/ble_hts \
  components/ble/ble_services/ble_cts_c \
  components/libraries/crc16 \
  components/nfc/t4t_parser/apdu \
  components/libraries/util \
  config \
  components/libraries/usbd/class/cdc \
  components/libraries/csense \
  components/libraries/balloc \
  components/libraries/ecc \
  components/libraries/hardfault \
  components/libraries/cli/uart \
  components/ble/ble_services/ble_cscs \
  components/libraries/uart \
  components/libraries/hci \
  components/libraries/usbd/class/hid/kbd \
  components/libraries/timer \
  components/softdevice/s140/headers \
  integration/nrfx \
  components/nfc/t4t_parser/tlv \
  components/libraries/sortlist \
  components/libraries/spi_mngr \
  components/libraries/led_softblink \
  components/nfc/ndef/conn_hand_parser \
  components/libraries/sdcard \
  components/nfc/ndef/parser/record \
  modules/nrfx/mdk \
  components/ble/ble_link_ctx_manager \
  components/ble/ble_services/ble_nus \
  components/libraries/twi_mngr \
  components/ble/ble_services/ble_hids \
  components/libraries/strerror \
  components/libraries/crc32 \
  components/nfc/ndef/connection_handover/ble_oob_advdata \
  components/nfc/t2t_parser \
  components/nfc/ndef/connection_handover/ble_pair_msg \
  components/libraries/usbd/class/audio \
  components/nfc/t4t_lib \
  components/ble/peer_manager \
  components/libraries/mem_manager \
  components/libraries/ringbuf \
  components/ble/ble_services/ble_tps \
  components/nfc/ndef/parser/message \
  components/ble/ble_services/ble_dis \
  components/nfc/ndef/uri \
  components/ble/nrf_ble_gatt \
  components/ble/nrf_ble_qwr \
  components/libraries/gpiote \
  components/libraries/button \
  modules/nrfx \
  components/libraries/twi_sensor \
  integration/nrfx/legacy \
  components/libraries/usbd \
  components/nfc/ndef/connection_handover/ep_oob_rec \
  external/segger_rtt \
  components/libraries/atomic_fifo \
  components/ble/ble_services/ble_lbs_c \
  components/nfc/ndef/connection_handover/ble_pair_lib \
  components/libraries/crypto \
  components/ble/ble_racp \
  components/libraries/fds \
  components/nfc/ndef/launchapp \
  components/libraries/atomic_flags \
  components/ble/ble_services/ble_hrs \
  components/ble/ble_services/ble_rscs \
  components/nfc/ndef/connection_handover/hs_rec \
  components/nfc/ndef/conn_hand_parser/ac_rec_parser \
  components/libraries/stack_guard \
  components/libraries/log/src \

# Libraries common to all targets
LIB_FILES += \

# Optimization flags
OPT = -O3 -g3
# Uncomment the line below to enable link time optimization
#OPT += -flto

# C flags common to all targets
CFLAGS += $(OPT)
CFLAGS += -DAPP_TIMER_V2
CFLAGS += -DAPP_TIMER_V2_RTC1_ENABLED
CFLAGS += -DBOARD_PCA10059
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DFLOAT_ABI_HARD
CFLAGS += -DNRF52840_XXAA
CFLAGS += -DNRF_SD_BLE_API_VERSION=7
CFLAGS += -DS140
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
#CFLAGS += -Wall -Werror
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums

# C++ flags common to all targets
CXXFLAGS += $(OPT)
# Assembler flags common to all targets
ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
ASMFLAGS += -DAPP_TIMER_V2
ASMFLAGS += -DAPP_TIMER_V2_RTC1_ENABLED
ASMFLAGS += -DBOARD_PCA10059
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DFLOAT_ABI_HARD
ASMFLAGS += -DNRF52840_XXAA
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=7
ASMFLAGS += -DS140
ASMFLAGS += -DSOFTDEVICE_PRESENT

# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += -mthumb -mabi=aapcs -Lmodules/nrfx/mdk -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs

nrf52840_xxaa: CFLAGS += -D__HEAP_SIZE=8192
nrf52840_xxaa: CFLAGS += -D__STACK_SIZE=8192
nrf52840_xxaa: ASMFLAGS += -D__HEAP_SIZE=8192
nrf52840_xxaa: ASMFLAGS += -D__STACK_SIZE=8192

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm


.PHONY: default help

# Default target - first one defined
default: nrf52840_xxaa

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo		nrf52840_xxaa
	@echo		flash_softdevice
	@echo		sdk_config - starting external tool for editing sdk_config.h
	@echo		flash      - flashing binary

TEMPLATE_PATH := components/toolchain/gcc


include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))

.PHONY: flash flash_softdevice erase

# Flash the program
flash: default
	@echo Flashing: $(OUTPUT_DIRECTORY)/nrf52840_xxaa.hex
	nrfjprog -f nrf52 --program $(OUTPUT_DIRECTORY)/nrf52840_xxaa.hex --sectorerase
	nrfjprog -f nrf52 --reset

# Flash softdevice
flash_softdevice:
	@echo Flashing: s140_nrf52_7.0.1_softdevice.hex
	nrfjprog -f nrf52 --program components/softdevice/s140/hex/s140_nrf52_7.0.1_softdevice.hex --sectorerase
	nrfjprog -f nrf52 --reset

erase:
	nrfjprog -f nrf52 --eraseall

SDK_CONFIG_FILE := ../config/sdk_config.h
CMSIS_CONFIG_TOOL := external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)
