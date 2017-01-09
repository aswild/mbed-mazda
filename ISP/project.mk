# Project Config Template

# Project name (name of the folder and the resulting binary)
PROJECT        = ISP

# Copy destination for "make upload"
UPLOAD_DIR     = /cygdrive/n

# Default target
TARGET         = LPC1768

# Toolchain location (path where arm-none-eabi-gcc can be found)
ifeq ($(shell test -d /cygdrive && echo -n yes),yes)
TOOLCHAIN     ?= /opt/gcc-arm-none-eabi/bin
else
TOOLCHAIN     ?=
endif

# C/C++ source files for this project
PROJECT_SRCS   = command_interface.cpp \
                 DirectoryList.cpp \
                 dir_handling.cpp \
                 isp.cpp \
                 main.cpp \
                 serial_utilities.cpp \
                 target_handling.cpp \
                 target_table.cpp \
                 uu_coding.cpp \
                 verification.cpp \
                 writing.cpp \
                 MODSERIAL/FLUSH.cpp \
                 MODSERIAL/GETC.cpp \
                 MODSERIAL/INIT.cpp \
                 MODSERIAL/ISR_RX.cpp \
                 MODSERIAL/ISR_TX.cpp \
                 MODSERIAL/MODSERIAL.cpp \
                 MODSERIAL/MODSERIAL_IRQ_INFO.cpp \
                 MODSERIAL/PUTC.cpp \
                 MODSERIAL/RESIZE.cpp

PROJECT_INCLUDES = -I$(PROJECT)/MODSERIAL
