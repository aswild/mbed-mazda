# Configuration for LPC1114

_SYS_OBJECTS = analogin_api.o \
               cmsis_nvic.o \
               gpio_api.o \
               gpio_irq_api.o \
               i2c_api.o \
               mbed_board.o \
               pinmap.o \
               port_api.o \
               pwmout_api.o \
               retarget.o \
               serial_api.o \
               sleep.o \
               spi_api.o \
               startup_LPC11xx.o \
               system_LPC11xx.o \
               us_ticker.o

SYS_OBJECTS  = $(addprefix $(MBED)/TARGET_LPC1114/TOOLCHAIN_GCC_ARM/,$(_SYS_OBJECTS))

SYS_INCLUDES = -I$(MBED)/TARGET_LPC1114 \
               -I$(MBED)/TARGET_LPC1114/TARGET_NXP \
               -I$(MBED)/TARGET_LPC1114/TARGET_NXP/TARGET_LPC11XX_11CXX \
               -I$(MBED)/TARGET_LPC1114/TARGET_NXP/TARGET_LPC11XX_11CXX/TARGET_LPC11XX \
               -I$(MBED)/TARGET_LPC1114/TARGET_NXP/TARGET_LPC11XX_11CXX/device \
               -I$(MBED)/TARGET_LPC1114/TOOLCHAIN_GCC_ARM

CPU         := -mcpu=cortex-m0 -mthumb
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
               -D__CORTEX_M0 \
               -DARM_MATH_CM0 \
               -DTARGET_CORTEX_M \
               -DTARGET_M0 \
               -DTARGET_LIKE_CORTEX_M0 \
               -DTARGET_NXP \
               -DTARGET_LPC11XX_11CXX \
               -DTARGET_LPC11XX \
               -DTARGET_LPC1114 \
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
              -L$(MBED)/TARGET_LPC1114/TOOLCHAIN_GCC_ARM

LD_SCRIPT   = $(MBED)/TARGET_LPC1114/TOOLCHAIN_GCC_ARM/LPC1114.ld

UPLOAD_DEST = $(UPLOAD_DIR)/1114bin
