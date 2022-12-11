ifndef WONDERFUL_TOOLCHAIN
$(error Please define WONDERFUL_TOOLCHAIN to point to the location of the Wonderful toolchain.)
endif
include $(WONDERFUL_TOOLCHAIN)/i8086/wswan.mk

BINFILE := data.bin

TARGET := tinyzoo.wsc
TARGET_ENGINE := tinyzoo_ws_engine.bin
OBJDIR := obj
SRCDIRS := res src src/elements src/platform_ws
MKDIRS := $(OBJDIR)
LIBS := -lws -lc -lgcc

CSOURCES := $(foreach dir,$(SRCDIRS),$(notdir $(wildcard $(dir)/*.c)))
ASMSOURCES := $(foreach dir,$(SRCDIRS),$(notdir $(wildcard $(dir)/*.S)))
OBJECTS := $(CSOURCES:%.c=$(OBJDIR)/%.o) $(ASMSOURCES:%.S=$(OBJDIR)/%.o)

DEPS := $(OBJECTS:.o=.d)
CFLAGS += -MMD -MP

CFLAGS += -Ires -Isrc -Isrc/elements -Isrc/platform_ws

vpath %.c $(SRCDIRS)
vpath %.S $(SRCDIRS)

.PHONY: all clean install

all: $(TARGET) $(TARGET_ENGINE)

$(TARGET_ENGINE): $(OBJECTS)
	$(SWANLINK) -v -o $@ --trim --heap-length 0x1FF0 --ram-type SRAM_32KB --color --output-elf $@.elf --linker-args $(LDFLAGS) $(WF_CRT0) $(OBJECTS) $(LIBS)

$(TARGET): $(OBJECTS) $(BINFILE)
	$(SWANLINK) -v -o $@ -a $(BINFILE) --heap-length 0x1FF0 --ram-type SRAM_32KB --color --output-elf $@.elf --linker-args $(LDFLAGS) $(WF_CRT0) $(OBJECTS) $(LIBS)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -D__WONDERFUL_WSWAN__ -O2 -fno-jump-tables -fno-function-sections -c -o $@ $<

$(OBJDIR)/%.o: %.S | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR):
	$(info $(shell mkdir -p $(MKDIRS)))

clean:
	rm -r $(OBJDIR)/*
	rm $(TARGET) $(TARGET).elf

-include $(DEPS)
