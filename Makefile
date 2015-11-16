# This file was automagically generated by mbed.org. For more information, 
# see http://mbed.org/handbook/Exporting-to-GCC-ARM-Embedded
#
# Customized by Allen Wild to be better

GCC_BIN = /opt/gcc-arm-none-eabi/bin
PROJECT = mbed-mazda
OBJDIR	= build
CPPSOURCES = $(wildcard *.cpp)
CPPOBJECTS = $(patsubst %.cpp, $(OBJDIR)/%.o, $(CPPSOURCES))

OBJECTS		= $(CPPOBJECTS)
SYS_OBJECTS = ./mbed/TARGET_LPC1114/TOOLCHAIN_GCC_ARM/board.o \
			  ./mbed/TARGET_LPC1114/TOOLCHAIN_GCC_ARM/cmsis_nvic.o \
			  ./mbed/TARGET_LPC1114/TOOLCHAIN_GCC_ARM/retarget.o \
			  ./mbed/TARGET_LPC1114/TOOLCHAIN_GCC_ARM/startup_LPC11xx.o \
			  ./mbed/TARGET_LPC1114/TOOLCHAIN_GCC_ARM/system_LPC11xx.o \

PROBJ	= $(OBJDIR)/$(PROJECT)
BINFILE = $(PROJECT).bin

INCLUDE_PATHS = -I. \
				-I./mbed \
				-I./mbed/TARGET_LPC1114 \
				-I./mbed/TARGET_LPC1114/TARGET_NXP \
				-I./mbed/TARGET_LPC1114/TARGET_NXP/TARGET_LPC11XX_11CXX \
				-I./mbed/TARGET_LPC1114/TARGET_NXP/TARGET_LPC11XX_11CXX/TARGET_LPC11XX \
				-I./mbed/TARGET_LPC1114/TOOLCHAIN_GCC_ARM 

LIBRARY_PATHS = -L./mbed/TARGET_LPC1114/TOOLCHAIN_GCC_ARM 
LIBRARIES     = -lmbed 
LINKER_SCRIPT = ./mbed/TARGET_LPC1114/TOOLCHAIN_GCC_ARM/LPC1114.ld

############################################################################### 
AS		= $(GCC_BIN)/arm-none-eabi-as
CC		= $(GCC_BIN)/arm-none-eabi-gcc
CPP		= $(GCC_BIN)/arm-none-eabi-g++
LD		= $(GCC_BIN)/arm-none-eabi-gcc
OBJCOPY = $(GCC_BIN)/arm-none-eabi-objcopy
OBJDUMP = $(GCC_BIN)/arm-none-eabi-objdump
SIZE	= $(GCC_BIN)/arm-none-eabi-size 


CPU = -mcpu=cortex-m0 -mthumb 
CC_FLAGS = $(CPU) -c -g -fno-common -fmessage-length=0 -Wall -Wextra -fno-exceptions -ffunction-sections -fdata-sections -fomit-frame-pointer -MMD -MP
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
#LD_FLAGS += -u _printf_float -u _scanf_float
LD_SYS_LIBS = -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys

#Colors
Y = "\e[0;33m"
G = "\e[1;32m"
N = "\e[0m"


ifeq ($(DEBUG), 1)
  CC_FLAGS += -DDEBUG -O0
else
  CC_FLAGS += -DNDEBUG -Os
endif

.PHONY: all clean lst size

all: $(PROJECT).bin $(PROBJ).hex size


clean:
	@#rm -f $(BINFILE) $(PROBJ).elf $(PROBJ).hex $(PROBJ).map $(PROBJ).lst $(OBJECTS) $(DEPS)
	rm -rf $(BINFILE) $(OBJDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(CPPOBJECTS) : $(OBJDIR)/%.o : %.cpp | $(OBJDIR)
	@echo -e $(Y)$@$(N)
	$(CPP) $(CC_FLAGS) $(CC_SYMBOLS) -std=gnu++98 -fno-rtti $(INCLUDE_PATHS) -o $@ $<


# Original .o implicit targets, which don't handle OBJDIR nicely
# Add similar rules as CPPOBJECTS if source files of different types are added
#.asm.o:
#	$(CC) $(CPU) -c -x assembler-with-cpp -o $@ $<
#.s.o:
#	$(CC) $(CPU) -c -x assembler-with-cpp -o $@ $<
#.S.o:
#	$(CC) $(CPU) -c -x assembler-with-cpp -o $@ $<
#.c.o:
#	$(CC)  $(CC_FLAGS) $(CC_SYMBOLS) -std=gnu99   $(INCLUDE_PATHS) -o $@ $<
#.cpp.o:
#	$(CPP) $(CC_FLAGS) $(CC_SYMBOLS) -std=gnu++98 -fno-rtti $(INCLUDE_PATHS) -o $@ $<

$(PROJECT).bin: $(PROBJ).elf
	@echo -e $(Y)$@$(N)
	$(OBJCOPY) -O binary $< $@


$(PROBJ).elf: $(OBJECTS) $(SYS_OBJECTS)
	@echo -e $(Y)$@$(N)
	$(LD) $(LD_FLAGS) -T$(LINKER_SCRIPT) $(LIBRARY_PATHS) -o $@ $^ $(LIBRARIES) $(LD_SYS_LIBS)

#	@echo ""
#	@echo "*****"
#	@echo "***** You must modify vector checksum value in *.bin and *.hex files."
#	@echo "*****"
#	@echo ""


$(PROBJ).hex: $(PROBJ).elf
	@$(OBJCOPY) -O ihex $< $@

$(PROBJ).lst: $(PROBJ).elf
	@$(OBJDUMP) -Sdh $< > $@

lst: $(PROBJ).lst

size: $(PROBJ).elf
	@echo -e $(G)"\nBuild complete!"$(N)
	$(SIZE) $(PROBJ).elf

DEPS = $(OBJECTS:.o=.d) $(SYS_OBJECTS:.o=.d)
-include $(DEPS)
