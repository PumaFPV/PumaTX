#
# F3 Make file include
#

ifeq ($(OPBL),yes)
LD_SCRIPT = $(LINKER_DIR)/stm32_flash_f303_$(FLASH_SIZE)k_opbl.ld
endif

TARGET_FLASH   := 256
MCU_COMMON_SRC  = startup_stm32f30x_md_gcc.S

STDPERIPH_DIR   = $(ROOT)/lib/main/STM32F3/Drivers/STM32F30x_StdPeriph_Driver
STDPERIPH_SRC   = $(notdir $(wildcard $(STDPERIPH_DIR)/src/*.c))
EXCLUDES        = stm32f30x_crc.c \
                  stm32f30x_can.c
STARTUP_SRC     = startup_stm32f30x_md_gcc.S

STDPERIPH_SRC   := $(filter-out ${EXCLUDES}, $(STDPERIPH_SRC))
DEVICE_STDPERIPH_SRC = $(STDPERIPH_SRC)

VPATH           := $(VPATH):$(CMSIS_DIR)/Core/Include
CMSIS_SRC       = $(notdir $(wildcard $(ROOT)/lib/main/STM32F3/Drivers/CMSIS/Device/ST/STM32F30x/*.c))

INCLUDE_DIRS    := $(INCLUDE_DIRS) \
                   $(STDPERIPH_DIR)/inc \
                   $(CMSIS_DIR)/Core/Include \
                   $(ROOT)/lib/main/STM32F3/Drivers/CMSIS/Device/ST/STM32F30x

ifneq ($(filter VCP, $(FEATURES)),)
USBFS_DIR       = $(ROOT)/lib/main/STM32_USB-FS-Device_Driver
USBPERIPH_SRC   = $(notdir $(wildcard $(USBFS_DIR)/src/*.c))

INCLUDE_DIRS    := $(INCLUDE_DIRS) \
                   $(USBFS_DIR)/inc \
                   $(ROOT)/src/main/vcp

VPATH           := $(VPATH):$(USBFS_DIR)/src

DEVICE_STDPERIPH_SRC := $(DEVICE_STDPERIPH_SRC)\
                        $(USBPERIPH_SRC)
endif

ifneq ($(filter SDCARD, $(FEATURES)),)
INCLUDE_DIRS    := $(INCLUDE_DIRS) \
                   $(FATFS_DIR) \

VPATH           := $(VPATH):$(FATFS_DIR)
endif

LD_SCRIPT       = $(LINKER_DIR)/stm32_flash_f303_$(FLASH_SIZE)k.ld

ARCH_FLAGS      = -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant -Wdouble-promotion
DEVICE_FLAGS    = -DSTM32F303xC -DSTM32F303

VCP_SRC = \
            vcp/hw_config.c \
            vcp/stm32_it.c \
            vcp/usb_desc.c \
            vcp/usb_endp.c \
            vcp/usb_istr.c \
            vcp/usb_prop.c \
            vcp/usb_pwr.c \
            drivers/serial_usb_vcp.c \
            drivers/usb_io.c


MCU_COMMON_SRC = \
            startup_stm32f30x_md_gcc.S \
            target/system_stm32f30x.c \
            drivers/accgyro/accgyro.c \
            drivers/adc_stm32f30x.c \
            drivers/bus_i2c_stm32f30x.c \
            drivers/dma_stm32f3xx.c \
            drivers/serial_uart_stm32f30x.c \
            drivers/system_stm32f30x.c \
            drivers/timer_impl_stdperiph.c \
            drivers/timer_stm32f30x.c

DSP_LIB := $(ROOT)/lib/main/CMSIS/DSP
DEVICE_FLAGS += -DARM_MATH_MATRIX_CHECK -DARM_MATH_ROUNDING -D__FPU_PRESENT=1 -DUNALIGNED_SUPPORT_DISABLE -DARM_MATH_CM4
