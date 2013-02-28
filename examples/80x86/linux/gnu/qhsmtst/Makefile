##############################################################################
# Product: QHsmTst example, Linux, GNU compiler
# Last Updated for Version: 4.2.00
# Date of the Last Update:  Jul 11, 2011
#
#                    Q u a n t u m     L e a P s
#                    ---------------------------
#                    innovating embedded systems
#
# Copyright (C) 2002-2011 Quantum Leaps, LLC. All rights reserved.
#
# This software may be distributed and modified under the terms of the GNU
# General Public License version 2 (GPL) as published by the Free Software
# Foundation and appearing in the file GPL.TXT included in the packaging of
# this file. Please note that GPL Section 2[b] requires that all works based
# on this software must also be made publicly available under the terms of
# the GPL ("Copyleft").
#
# Alternatively, this software may be distributed and modified under the
# terms of Quantum Leaps commercial licenses, which expressly supersede
# the GPL and are specifically designed for licensees interested in
# retaining the proprietary status of their code.
#
# Contact information:
# Quantum Leaps Web site:  http://www.quantum-leaps.com
# e-mail:                  info@quantum-leaps.com
##############################################################################
# examples of invoking this Makefile:
# building configurations: Debug (default), Release, and Spy
# make
# make CONF=rel
# make CONF=spy
#
# cleaning configurations: Debug (default), Release, and Spy
# make clean
# make CONF=rel clean
# make CONF=spy clean

#-----------------------------------------------------------------------------
# NOTE: the Makefile expects that the QPCPP environment variable is defined
# and points to the QP/C++ installation directory
#
ifndef QPCPP
$(error The QPCPP environment variable must be defined)
endif

#-----------------------------------------------------------------------------
# general utilities
#
RM = rm

#-----------------------------------------------------------------------------
# NOTE: this Makefile assumes that the MinGW\bin directory is added
# to the PATH variable.
#
CC    := gcc
CPP   := g++
#LINK  := gcc                                                 # for C programs
LINK  := g++                                                # for C++ programs

#-----------------------------------------------------------------------------
# directories
#
QP_PORT_DIR := $(QPCPP)/ports/80x86/linux/gnu
APP_DIR     := .

VPATH = $(APP_DIR)

# Output file basename
OUTPUT    := qhsmtst

# include directories
INCLUDES  = -I$(QPCPP)/include	\
	-I$(QP_PORT_DIR) \
	-I.

# defines
DEFINES =

#-----------------------------------------------------------------------------
# files
#

# assembler source files
ASM_SRCS :=

# C source files
C_SRCS := $(wildcard *.c)

# C++ source files
CPP_SRCS := $(wildcard *.cpp)

LD_SCRIPT :=

#-----------------------------------------------------------------------------
# build options for various configurations
#

ifeq (rel, $(CONF))       # Release configuration ............................

BIN_DIR := rel
LIBS    := -lqf -lqep

ASFLAGS =

CFLAGS = -c -Wall \
	-O2 $(INCLUDES) $(DEFINES) -DNDEBUG

CPPFLAGS = -c -Wall \
	-O2 $(INCLUDES) $(DEFINES) -DNDEBUG

LINKFLAGS = -L$(QP_PORT_DIR)/$(BIN_DIR)

else ifeq (spy, $(CONF))  # Spy configuration ................................

BIN_DIR := spy
LIBS    := -lqf -lqep -lqs -lwsock32

ASFLAGS =

CFLAGS = -c -Wall \
	-g $(INCLUDES) $(DEFINES) -DQ_SPY

CPPFLAGS = -c -Wall \
	-g $(INCLUDES) $(DEFINES) -DQ_SPY

LINKFLAGS = -L$(QP_PORT_DIR)/$(BIN_DIR)

else                     # default Debug configuration .......................

BIN_DIR := dbg
LIBS    := -lqf -lqep

ASFLAGS =

CFLAGS = -c -Wall \
	-g $(INCLUDES) $(DEFINES)

CPPFLAGS = -c -Wall \
	-g $(INCLUDES) $(DEFINES)

LINKFLAGS = -L$(QP_PORT_DIR)/$(BIN_DIR)


endif


ASM_OBJS     := $(patsubst %.s,%.o,$(ASM_SRCS))
C_OBJS       := $(patsubst %.c,%.o,$(C_SRCS))
CPP_OBJS     := $(patsubst %.cpp,%.o,$(CPP_SRCS))

TARGET_BIN   := $(BIN_DIR)/$(OUTPUT).bin
TARGET_EXE   := $(BIN_DIR)/$(OUTPUT)
ASM_OBJS_EXT := $(addprefix $(BIN_DIR)/, $(ASM_OBJS))
C_OBJS_EXT   := $(addprefix $(BIN_DIR)/, $(C_OBJS))
C_DEPS_EXT   := $(patsubst %.o, %.d, $(C_OBJS_EXT))
CPP_OBJS_EXT := $(addprefix $(BIN_DIR)/, $(CPP_OBJS))
CPP_DEPS_EXT := $(patsubst %.o, %.d, $(CPP_OBJS_EXT))


#-----------------------------------------------------------------------------
# rules
#

all: $(BIN_DIR) $(TARGET_EXE)
#all: $(BIN_DIR) $(TARGET_BIN)

$(BIN_DIR):
	@echo
	mkdir -p $@

$(TARGET_BIN): $(TARGET_EXE)
	$(BIN) -O binary $< $@

$(TARGET_EXE) : $(ASM_OBJS_EXT) $(C_OBJS_EXT) $(CPP_OBJS_EXT)
	$(LINK) $(LINKFLAGS) -o $@ $^ $(LIBS)

$(BIN_DIR)/%.d : %.c
	$(CC) -MM -MT $(@:.d=.o) $(CFLAGS) $< > $@

$(BIN_DIR)/%.d : %.cpp
	$(CPP) -MM -MT $(@:.d=.o) $(CPPFLAGS) $< > $@

$(BIN_DIR)/%.o : %.s
	$(AS) $(ASFLAGS) $< -o $@

$(BIN_DIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/%.o : %.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

-include $(C_DEPS_EXT) $(CPP_DEPS_EXT)

.PHONY : clean
clean:
	-$(RM) $(BIN_DIR)/*.o \
	$(BIN_DIR)/*.d \
	$(BIN_DIR)/$(OUTPUT) \
	$(BIN_DIR)/*.map
