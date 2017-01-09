# Project Config for mazda mbed controller

# Project name (name of the folder and the resulting binary)
PROJECT        = mazda

# Copy destination for "make upload"
UPLOAD_DIR     = /cygdrive/n

# Default target
TARGET        ?= LPC1114

# Toolchain location (path where arm-none-eabi-gcc can be found)
ifeq ($(shell test -d /cygdrive && echo -n yes),yes)
TOOLCHAIN     ?= /opt/gcc-arm-none-eabi/bin
else
TOOLCHAIN     ?=
endif

# C/C++ source files for this project
PROJECT_SRCS   = Input.cpp \
                 main.cpp \
                 MCP41XXX.cpp \
                 PioneerRemote.cpp \
                 RN52.cpp

ifeq ($(TARGET),LPC1768)
PROJECT_SRCS += MPR121.cpp
PROJECT_CXXFLAGS += -DSERIAL_DEBUG
endif
