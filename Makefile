# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Adrian "asie" Siekierka, 2023

WONDERFUL_TOOLCHAIN ?= /opt/wonderful
TARGET = wswan/medium
include $(WONDERFUL_TOOLCHAIN)/target/$(TARGET)/makedefs.mk

# Metadata
# --------

NAME		:= tinyzoo_ws_engine

# Source code paths
# -----------------

INCLUDEDIRS	:= src src/platform_ws
SOURCEDIRS	:= res src src/elements src/platform_ws
CBINDIRS	:=

# Defines passed to all files
# ---------------------------

DEFINES		:=

# Libraries
# ---------

LIBS		:= -lwsx -lws
LIBDIRS		:= $(WF_TARGET_DIR)

# Build artifacts
# ---------------

BUILDDIR	:= build
ELF		:= build/$(NAME).elf
ELF_STAGE1	:= build/$(NAME)_stage1.elf
ROM		:= $(NAME).bin

# Verbose flag
# ------------

ifeq ($(V),1)
_V		:=
else
_V		:= @
endif

# Source files
# ------------

ifneq ($(CBINDIRS),)
    SOURCES_CBIN	:= $(shell find -L $(CBINDIRS) -name "*.bin")
    INCLUDEDIRS		+= $(addprefix $(BUILDDIR)/,$(CBINDIRS))
endif
SOURCES_S	:= $(shell find -L $(SOURCEDIRS) -maxdepth 1 -name "*.s")
SOURCES_C	:= $(shell find -L $(SOURCEDIRS) -maxdepth 1 -name "*.c")

# Compiler and linker flags
# -------------------------

WARNFLAGS	:= -Wall

INCLUDEFLAGS	:= $(foreach path,$(INCLUDEDIRS),-I$(path)) \
		   $(foreach path,$(LIBDIRS),-I$(path)/include)

LIBDIRSFLAGS	:= $(foreach path,$(LIBDIRS),-L$(path)/lib)

ASFLAGS		+= -x assembler-with-cpp $(DEFINES) $(WF_ARCH_CFLAGS) \
		   $(INCLUDEFLAGS) -fno-function-sections -fdata-sections -fno-common

CFLAGS		+= -std=gnu11 $(WARNFLAGS) $(DEFINES) $(WF_ARCH_CFLAGS) \
		   $(INCLUDEFLAGS) -fno-function-sections -fdata-sections -fno-common -O2 -g

LDFLAGS		:= -T$(WF_LDSCRIPT) $(LIBDIRSFLAGS) \
		   $(WF_ARCH_LDFLAGS) $(LIBS)

BUILDROMFLAGS	:= --trim -v

# Intermediate build files
# ------------------------

OBJS_ASSETS	:= $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_CBIN)))

HEADERS_ASSETS	:= $(patsubst %.bin,%_bin.h,$(addprefix $(BUILDDIR)/,$(SOURCES_CBIN)))

OBJS_SOURCES	:= $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_S))) \
		   $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_C)))

OBJS		:= $(OBJS_ASSETS) $(OBJS_SOURCES)

DEPS		:= $(OBJS:.o=.d)

# Targets
# -------

.PHONY: all clean

all: $(ROM)

$(ROM) $(ELF): $(ELF_STAGE1)
	@echo "  ROM     $@"
	$(_V)$(BUILDROM) -o $(ROM) --output-elf $(ELF) $(BUILDROMFLAGS) $<

$(ELF_STAGE1): $(OBJS)
	@echo "  LD      $@"
	$(_V)$(CC) -r -o $(ELF_STAGE1) $(OBJS) $(WF_CRT0) $(LDFLAGS)

clean:
	@echo "  CLEAN"
	$(_V)$(RM) $(ROM) $(BUILDDIR)

# Rules
# -----

$(BUILDDIR)/%.s.o : %.s
	@echo "  AS      $<"
	@$(MKDIR) -p $(@D)
	$(_V)$(CC) $(ASFLAGS) -MMD -MP -c -o $@ $<

$(BUILDDIR)/%.c.o : %.c
	@echo "  CC      $<"
	@$(MKDIR) -p $(@D)
	$(_V)$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<

$(BUILDDIR)/%.bin.o $(BUILDDIR)/%_bin.h : %.bin
	@echo "  BIN2C   $<"
	@$(MKDIR) -p $(@D)
	$(_V)$(WF)/bin/wf-bin2c -a 2 --address-space __far $(@D) $<
	$(_V)$(CC) $(CFLAGS) -MMD -MP -c -o $(BUILDDIR)/$*.bin.o $(BUILDDIR)/$*_bin.c

# Include dependency files if they exist
# --------------------------------------

-include $(DEPS)
