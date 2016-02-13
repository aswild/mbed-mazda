# mbed LPC1768 and LPC1114 multi-platform makefile

GCC_BIN = /opt/gcc-arm-none-eabi/bin
PROJECT = mbed-mazda
OBJDIR  = build
UPLOAD_DIR = /cygdrive/n

CPPSOURCES = $(wildcard *.cpp)
CPPOBJECTS = $(patsubst %.cpp, $(OBJDIR)/%.o, $(CPPSOURCES))
OBJECTS    = $(CPPOBJECTS)
PROBJ      = $(OBJDIR)/$(PROJECT)
LIBRARIES     = -lmbed

ifndef PLATFORM
PLATFORM = LPC1114
endif
BINFILE    = $(PROJECT)_$(PLATFORM).bin

# Platform-specific compiler options {{{
##################### LPC 1114 Options ########################
ifeq ($(PLATFORM),LPC1114)
SYS_OBJECTS   = ./mbed/TARGET_LPC1114/TOOLCHAIN_GCC_ARM/board.o \
                ./mbed/TARGET_LPC1114/TOOLCHAIN_GCC_ARM/cmsis_nvic.o \
                ./mbed/TARGET_LPC1114/TOOLCHAIN_GCC_ARM/retarget.o \
                ./mbed/TARGET_LPC1114/TOOLCHAIN_GCC_ARM/startup_LPC11xx.o \
                ./mbed/TARGET_LPC1114/TOOLCHAIN_GCC_ARM/system_LPC11xx.o
INCLUDE_PATHS = -I. \
                -I./mbed \
                -I./mbed/TARGET_LPC1114 \
                -I./mbed/TARGET_LPC1114/TARGET_NXP \
                -I./mbed/TARGET_LPC1114/TARGET_NXP/TARGET_LPC11XX_11CXX \
                -I./mbed/TARGET_LPC1114/TARGET_NXP/TARGET_LPC11XX_11CXX/TARGET_LPC11XX \
                -I./mbed/TARGET_LPC1114/TOOLCHAIN_GCC_ARM

LIBRARY_PATHS = -L./mbed/TARGET_LPC1114/TOOLCHAIN_GCC_ARM
LINKER_SCRIPT = ./mbed/TARGET_LPC1114/TOOLCHAIN_GCC_ARM/LPC1114.ld

CPU = -mcpu=cortex-m0 -mthumb
CC_FLAGS = $(CPU) $(CFLAGS) -c -g -fno-common -fmessage-length=0 -Wall -Wextra \
           -fno-exceptions -ffunction-sections -fdata-sections -fomit-frame-pointer -MMD -MP
CC_SYMBOLS = -D__CORTEX_M0 \
             -DTARGET_LPC1114 \
             -DTOOLCHAIN_GCC_ARM \
             -DTOOLCHAIN_GCC \
             -DTARGET_CORTEX_M \
             -DARM_MATH_CM0 \
             -DTARGET_NXP \
             -DTARGET_M0 \
             -DTARGET_LPC11XX \
             -DMBED_BUILD_TIMESTAMP=1447636823.25 \
             -D__MBED__=1 \
             -DTARGET_LPC11XX_11CXX

LD_FLAGS = $(CPU) -Wl,--gc-sections --specs=nano.specs -Wl,--wrap,main -Wl,-Map=$(PROBJ).map,--cref
LD_SYS_LIBS = -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys

UPLOAD_DEST = $(UPLOAD_DIR)/1114bin

else
##################### LPC 1768 Options ########################
ifeq ($(PLATFORM),LPC1768)
SYS_OBJECTS   = ./mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/board.o \
                ./mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/cmsis_nvic.o \
                ./mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/retarget.o \
                ./mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/startup_LPC17xx.o \
                ./mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/system_LPC17xx.o
INCLUDE_PATHS = -I. \
                -I./mbed \
                -I./mbed/TARGET_LPC1768 \
                -I.mbed/TARGET_LPC1768/TARGET_NXP \
                -I./mbed/TARGET_LPC1768/TARGET_NXP/TARGET_LPC176X \
                -I./mbed/TARGET_LPC1768/TARGET_NXP/TARGET_LPC176X/TARGET_MBED_LPC1768 \
                -I./mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM
LIBRARY_PATHS = -L./mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM
LINKER_SCRIPT = ./mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/LPC1768.ld

CPU = -mcpu=cortex-m3 -mthumb
CC_FLAGS = $(CPU) -c -g -fno-common -fmessage-length=0 -Wall -Wextra -fno-exceptions -ffunction-sections -fdata-sections -fomit-frame-pointer -MMD -MP
CC_FLAGS += -Wno-unused-parameter -Wno-write-strings
CC_SYMBOLS = -DTOOLCHAIN_GCC_ARM \
             -DTOOLCHAIN_GCC \
             -DMBED_BUILD_TIMESTAMP=1448083447.58 \
             -DARM_MATH_CM3 -DTARGET_CORTEX_M \
             -DTARGET_LPC176X \
             -DTARGET_NXP \
             -DTARGET_MBED_LPC1768 \
             -DTARGET_LPC1768 \
             -D__CORTEX_M3 \
             -DTARGET_M3 \
             -D__MBED__=1
CC_SYMBOLS += -DSERIAL_DEBUG

LD_FLAGS = $(CPU) -Wl,--gc-sections --specs=nano.specs -u _printf_float -u _scanf_float -Wl,--wrap,main -Wl,-Map=$(PROBJ).map,--cref
LD_SYS_LIBS = -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys
UPLOAD_DEST = $(UPLOAD_DIR)/$(BINFILE)

else
$(error Invalid PLATFORM)
endif # LPC1768
endif # LPC1114 }}}

###############################################################################
AS        = $(GCC_BIN)/arm-none-eabi-as
CC        = $(GCC_BIN)/arm-none-eabi-gcc
CPP       = $(GCC_BIN)/arm-none-eabi-g++
LD        = $(GCC_BIN)/arm-none-eabi-gcc
OBJCOPY   = $(GCC_BIN)/arm-none-eabi-objcopy
OBJDUMP   = $(GCC_BIN)/arm-none-eabi-objdump
SIZE      = $(GCC_BIN)/arm-none-eabi-size

#Colors
Y = "\e[0;33m"
G = "\e[1;32m"
N = "\e[0m"

ifeq ($(DEBUG), 1)
  CC_FLAGS += -DDEBUG -O0
else
  CC_FLAGS += -DNDEBUG -Os
endif

############################# BEGIN TARGETS ###############################
.PHONY: all clean lst hex size upload

all: $(BINFILE) lst size hex

clean:
	rm -rf $(BINFILE) $(OBJDIR)

upload: all
	cp $(BINFILE) $(UPLOAD_DEST)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(CPPOBJECTS) : $(OBJDIR)/%.o : %.cpp | $(OBJDIR)
	@echo -e $(Y)$@$(N)
	$(CPP) $(CC_FLAGS) $(CC_SYMBOLS) -std=gnu++98 -fno-rtti $(INCLUDE_PATHS) -o $@ $<

$(BINFILE): $(PROBJ).elf
	@echo -e $(Y)$@$(N)
	$(OBJCOPY) -O binary $< $@

$(PROBJ).elf: $(OBJECTS) $(SYS_OBJECTS)
	@echo -e $(Y)$@$(N)
	$(LD) $(LD_FLAGS) -T$(LINKER_SCRIPT) $(LIBRARY_PATHS) -o $@ $^ $(LIBRARIES) $(LD_SYS_LIBS)

hex: $(PROBJ).hex
$(PROBJ).hex: $(PROBJ).elf
	@$(OBJCOPY) -O ihex $< $@

lst: $(PROBJ).lst
$(PROBJ).lst: $(PROBJ).elf
	@$(OBJDUMP) -Sdh $< > $@

size: $(PROBJ).elf
	@echo -e $(G)"\nBuild complete!"$(N)
	$(SIZE) $(PROBJ).elf

DEPS = $(OBJECTS:.o=.d) $(SYS_OBJECTS:.o=.d)
-include $(DEPS)
