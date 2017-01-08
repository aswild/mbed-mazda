# Configuration options for LPC1768

_SYS_OBJECTS = analogin_api.o \
               analogout_api.o \
               can_api.o \
               cmsis_nvic.o \
               ethernet_api.o \
               gpio_api.o \
               gpio_irq_api.o \
               i2c_api.o \
               mbed_board.o \
               pinmap.o \
               port_api.o \
               pwmout_api.o \
               retarget.o \
               rtc_api.o \
               serial_api.o \
               sleep.o \
               spi_api.o \
               startup_LPC17xx.o \
               system_LPC17xx.o \
               us_ticker.o

SYS_OBJECTS  = $(addprefix $(MBED)/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/,$(_SYS_OBJECTS))

SYS_INCLUDES = -I$(MBED)/TARGET_LPC1768 \
               -I$(MBED)/TARGET_LPC1768/TARGET_NXP \
               -I$(MBED)/TARGET_LPC1768/TARGET_NXP/TARGET_LPC176X \
               -I$(MBED)/TARGET_LPC1768/TARGET_NXP/TARGET_LPC176X/TARGET_MBED_LPC1768 \
               -I$(MBED)/TARGET_LPC1768/TARGET_NXP/TARGET_LPC176X/device \
               -I$(MBED)/TARGET_LPC1768/TOOLCHAIN_GCC_ARM

CPU         := -mcpu=cortex-m3 -mthumb
SYS_CFLAGS  := $(CPU) \
               -fmessage-length=0 \
               -fno-exceptions \
               -fno-builtin \
               -ffunction-sections \
               -fdata-sections \
               -funsigned-char \
               -fno-delete-null-pointer-checks \
               -fomit-frame-pointer \
			   -D__MBED__=1 \
               -D__CORTEX_M3 \
               -DARM_MATH_CM3 \
               -DTARGET_CORTEX_M \
               -DTARGET_M3 \
               -DTARGET_NXP \
               -DTARGET_LPC176X \
               -DTARGET_LPC1768 \
               -DTARGET_MBED_LPC1768 \
               -DTOOLCHAIN_GCC \
               -DTOOLCHAIN_GCC_ARM

SYS_LIBS    = -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys
SYS_LDFLAGS = $(CPU) \
              -Wl,--gc-sections \
              -Wl,--wrap,main \
              -Wl,--wrap,_malloc_r \
              -Wl,--wrap,_calloc_r \
              -Wl,--wrap,_realloc_r \
              -Wl,--wrap,_free_r \
              -Wl,-Map=$(PROBJ).map,--cref \
              --specs=nano.specs \
              -L$(MBED)/TARGET_LPC1768/TOOLCHAIN_GCC_ARM

LD_SCRIPT   = $(MBED)/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/LPC1768.ld

UPLOAD_DEST = $(UPLOAD_DIR)/$(BINFILE)
