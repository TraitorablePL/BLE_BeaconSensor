PROJECT_NAME     := usbd_cdc_acm_pca10056
TARGETS          := nrf52840_xxaa
OUTPUT_DIRECTORY := _build

$(OUTPUT_DIRECTORY)/nrf52840_xxaa.out: \
  LINKER_SCRIPT  := usbd_cdc_acm_gcc_nrf52.ld

# Source files common to all targets
SRC_FILES += \
  modules/nrfx/mdk/gcc_startup_nrf52840.S \
  components/libraries/log/src/nrf_log_backend_rtt.c \
  components/libraries/log/src/nrf_log_backend_serial.c \
  components/libraries/log/src/nrf_log_backend_uart.c \
  components/libraries/log/src/nrf_log_default_backends.c \
  components/libraries/log/src/nrf_log_frontend.c \
  components/libraries/log/src/nrf_log_str_formatter.c \
  components/boards/boards.c \
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
  external/fnmatch/fnmatch.c \
  components/libraries/hardfault/nrf52/handler/hardfault_handler_gcc.c \
  components/libraries/hardfault/hardfault_implementation.c \
  components/libraries/util/nrf_assert.c \
  components/libraries/atomic_fifo/nrf_atfifo.c \
  components/libraries/atomic/nrf_atomic.c \
  components/libraries/balloc/nrf_balloc.c \
  components/libraries/cli/nrf_cli.c \
  components/libraries/cli/uart/nrf_cli_uart.c \
  external/fprintf/nrf_fprintf.c \
  external/fprintf/nrf_fprintf_format.c \
  components/libraries/memobj/nrf_memobj.c \
  components/libraries/pwr_mgmt/nrf_pwr_mgmt.c \
  components/libraries/queue/nrf_queue.c \
  components/libraries/ringbuf/nrf_ringbuf.c \
  components/libraries/experimental_section_vars/nrf_section_iter.c \
  components/libraries/sortlist/nrf_sortlist.c \
  components/libraries/strerror/nrf_strerror.c \
  integration/nrfx/legacy/nrf_drv_clock.c \
  integration/nrfx/legacy/nrf_drv_power.c \
  integration/nrfx/legacy/nrf_drv_uart.c \
  components/drivers_nrf/nrf_soc_nosd/nrf_nvic.c \
  components/drivers_nrf/nrf_soc_nosd/nrf_soc.c \
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
  main.c \
  external/segger_rtt/SEGGER_RTT.c \
  external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c \
  external/segger_rtt/SEGGER_RTT_printf.c \
  modules/nrfx/mdk/system_nrf52840.c \
  external/utf_converter/utf.c \

# Include folders common to all targets
INC_FOLDERS += \
  components \
  components/libraries/cli \
  modules/nrfx/mdk \
  components/libraries/scheduler \
  components/libraries/queue \
  components/libraries/pwr_mgmt \
  components/libraries/fifo \
  components/libraries/sortlist \
  components/libraries/strerror \
  components/toolchain/cmsis/include \
  components/libraries/timer \
  components/libraries/util \
  components/libraries/bsp \
  config \
  components/libraries/usbd/class/cdc \
  components/libraries/balloc \
  components/libraries/ringbuf \
  components/libraries/hardfault/nrf52 \
  components/libraries/cli/uart \
  components/libraries/hardfault \
  components/libraries/uart \
  components/libraries/log \
  components/libraries/button \
  modules/nrfx \
  components/libraries/experimental_section_vars \
  integration/nrfx/legacy \
  components/libraries/usbd \
  components/libraries/usbd/class/cdc/acm \
  components/libraries/mutex \
  components/libraries/delay \
  external/segger_rtt \
  components/libraries/atomic_fifo \
  components/drivers_nrf/nrf_soc_nosd \
  components/libraries/atomic \
  components/boards \
  components/libraries/memobj \
  external/fnmatch \
  integration/nrfx \
  external/utf_converter \
  modules/nrfx/drivers/include \
  modules/nrfx/hal \
  external/fprintf \
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
	@echo		sdk_config - starting external tool for editing sdk_config.h
	@echo		flash      - flashing binary

TEMPLATE_PATH := components/toolchain/gcc


include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))

.PHONY: flash erase

# Flash the program
flash: default
	@echo Flashing: $(OUTPUT_DIRECTORY)/nrf52840_xxaa.hex
	nrfjprog -f nrf52 --program $(OUTPUT_DIRECTORY)/nrf52840_xxaa.hex --sectorerase
	nrfjprog -f nrf52 --reset

erase:
	nrfjprog -f nrf52 --eraseall

SDK_CONFIG_FILE := ../config/sdk_config.h
CMSIS_CONFIG_TOOL := external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)
