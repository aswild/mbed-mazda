# mbed LPC1768 and LPC1114 multi-platform makefile

# Project Config
PROJECT = mbed-mazda
UPLOAD_DIR = /cygdrive/n

TARGET        ?= LPC1114
CROSS_COMPILE ?= arm-none-eabi-
MBED          ?= ./mbed
OBJDIR        ?= obj_$(TARGET)

# In cygwin, we can't use relative paths because the compilers don't recognize
# them. Relative paths work though
ifeq ($(shell echo "$(MBED)" | grep "^\."),)
ifeq ($(shell test -d /cygdrive ; echo $$?),0)
NEW_MBED := $(shell realpath --relative-to=$(PWD) $(MBED))
MBED     := $(NEW_MBED)
endif
endif

include TARGET_$(TARGET).mk

###############################################################################
AS        := $(CROSS_COMPILE)as
CC        := $(CROSS_COMPILE)gcc
CXX       := $(CROSS_COMPILE)g++
LD        := $(CROSS_COMPILE)gcc
OBJCOPY   := $(CROSS_COMPILE)objcopy
OBJDUMP   := $(CROSS_COMPILE)objdump
SIZE      := $(CROSS_COMPILE)size

CXX_SRCS   = Input.cpp \
             main.cpp \
             MCP41XXX.cpp \
             PioneerRemote.cpp \
             RN52.cpp

ifeq ($(TARGET),LPC1768)
CXX_SRCS += MPR121.cpp
endif

CXX_OBJS   = $(patsubst %.cpp, $(OBJDIR)/%.o, $(CXX_SRCS))
OBJECTS    = $(CXX_OBJS)
PROBJ      = $(OBJDIR)/$(PROJECT)

BINFILE    = $(PROJECT)_$(TARGET).bin

COMMON_CFLAGS  = -g -Wall -Wextra
USER_CFLAGS   := $(CFLAGS)

MBED_INCLUDES := -I. \
                 -I$(MBED) \
                 -I$(MBED)/drivers \
                 -I$(MBED)/hal \
                 -I$(MBED)/platform

CFLAGS         = $(COMMON_CFLAGS) $(SYS_CFLAGS) $(USER_CFLAGS) $(MBED_INCLUDES) $(SYS_INCLUDES)
USER_CXXFLAGS := $(CXXFLAGS)
CXXFLAGS       = $(COMMON_CFLAGS) -fno-rtti -std=gnu++98 \
                 $(SYS_CFLAGS) $(USER_CXXFLAGS) -I. -I$(MBED) $(MBED_INCLUDES) $(SYS_INCLUDES)

USER_LDFLAGS  := $(LDFLAGS)
LDFLAGS        = $(SYS_LDFLAGS) $(USER_LDFLAGS)
LIBS           = -lmbed $(SYS_LIBS)

#Colors
Y = \e[0;33m
G = \e[1;32m
N = \e[0m

ifeq ($(V),1)
quiet =
else
quiet = @
endif

############################# BEGIN TARGETS ###############################
.PHONY: all clean lst hex size upload debug

all: $(BINFILE) lst hex size

debug:
	$(MAKE) CFLAGS=-DSERIAL_DEBUG

clean:
	rm -rf $(BINFILE) $(OBJDIR)

upload: all
	cp $(BINFILE) $(UPLOAD_DEST)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(CXX_OBJS) : $(OBJDIR)/%.o : %.cpp | $(OBJDIR)
	@printf " $(Y)CXX  $@$(N)\n"
	$(quiet)$(CXX) $(CXXFLAGS) -MMD -MP -c -o $@ $<

$(PROBJ).elf: $(OBJECTS) $(SYS_OBJECTS)
	@printf " $(Y)LD   $@$(N)\n"
	$(quiet)$(LD) $(LDFLAGS) -T$(LD_SCRIPT) -o $@ $^ -Wl,--start-group $(LIBS) -Wl,--end-group

$(BINFILE): $(PROBJ).elf
	@printf " $(Y)BIN  $@$(N)\n"
	$(quiet)$(OBJCOPY) -O binary $< $@

hex: $(PROBJ).hex
$(PROBJ).hex: $(PROBJ).elf
	@printf " $(Y)HEX  $@$(N)\n"
	$(quiet)$(OBJCOPY) -O ihex $< $@

lst: $(PROBJ).lst
$(PROBJ).lst: $(PROBJ).elf
	@printf " $(Y)LST  $@$(N)\n"
	$(quiet)$(OBJDUMP) -Sdh $< > $@

size: $(PROBJ).elf
	@printf "\n$(G)Build complete!$(N)\n$(BINFILE):\n"
	$(quiet)$(SIZE) $(PROBJ).elf

DEPS = $(OBJECTS:.o=.d) $(SYS_OBJECTS:.o=.d)
-include $(DEPS)
