#define SRAM_ALLOC_INTERNAL

#include <string.h>

#include "bank_switch.h"
#include "config.h"
#include "gamevars.h"
#include "sram_alloc.h"

typedef struct __attribute__((packed)) {
	uint8_t flags;
	uint16_t size;
} sram_entry_t;

#define SRAM_HEADER_FLAG_WORLD 0x01
#define SRAM_FLAG_USED 0x01

static void sram_inc_ptr(sram_ptr_t *ptr) {
	if ((++(ptr->position)) == 0) {
		ptr->bank++;
	}
}

void sram_add_ptr(sram_ptr_t *ptr, uint16_t val) {
	if ((ptr->position + val) < ptr->position) {
		ptr->position += val;
		ptr->bank++;
	} else {
		ptr->position += val;
	}
}

void sram_sub_ptr(sram_ptr_t *ptr, uint16_t val) {
	if ((ptr->position - val) > ptr->position) {
		ptr->position -= val;
		ptr->bank--;
	} else {
		ptr->position -= val;
	}
}

uint8_t sram_read8(sram_ptr_t *ptr) {
	ZOO_SWITCH_RAM(ptr->bank);
	uint8_t value = *((volatile uint8_t __far*) MK_FP(0x1000, ptr->position));
	sram_inc_ptr(ptr);
	return value;
}

void sram_write8(sram_ptr_t *ptr, uint8_t value) {
	ZOO_SWITCH_RAM(ptr->bank);
	*((volatile uint8_t __far*) MK_FP(0x1000, ptr->position)) = value;
	sram_inc_ptr(ptr);
}

void sram_read(sram_ptr_t *ptr, uint8_t *data, uint16_t len) {
#if 1
	while (len > 0) {
		ZOO_SWITCH_RAM(ptr->bank);

		uint16_t len_to_read = len;
		if ((ptr->position + len) < ptr->position) {
			len_to_read = 65536 - ptr->position;
			len -= len_to_read;

			memcpy(data, MK_FP(0x1000, ptr->position), len_to_read);

			ptr->bank++;
			ptr->position = 0;
		} else {
			memcpy(data, MK_FP(0x1000, ptr->position), len_to_read);

			ptr->position += len_to_read;
		}

		data += len_to_read;
		len -= len_to_read;
	}
#else
	while ((len--) > 0) {
		*(data++) = sram_read8(ptr);
	}
#endif
}

void sram_write(sram_ptr_t *ptr, const uint8_t __far* data, uint16_t len) {
#if 1
	while (len > 0) {
		ZOO_SWITCH_RAM(ptr->bank);

		uint16_t len_to_read = len;
		if ((ptr->position + len) < ptr->position) {
			len_to_read = 65536 - ptr->position;
			len -= len_to_read;

			memcpy(MK_FP(0x1000, ptr->position), data, len_to_read);

			ptr->bank++;
			ptr->position = 0;
		} else {
			memcpy(MK_FP(0x1000, ptr->position), data, len_to_read);

			ptr->position += len_to_read;
		}

		data += len_to_read;
		len -= len_to_read;
	}
#else
	while ((len--) > 0) {
		sram_write8(ptr, *(data++));
	}
#endif
}

#ifdef DEBUG_SRAM_WRITES
uint8_t sram_read8_debug(sram_ptr_t *ptr) {
	EMU_printf("reading 1 from %02X:%04X",
		(uint16_t) ptr->bank, (uint16_t) ptr->position);
	return sram_read8(ptr);
}

void sram_write8_debug(sram_ptr_t *ptr, uint8_t value) {
	sram_ptr_t tptr, dptr;
	uint8_t tvalue;
	tptr = *ptr;
	dptr = *ptr;
	EMU_printf("writing 1 to %02X:%04X",
		(uint16_t) ptr->bank, (uint16_t) ptr->position);
	sram_write8(ptr, value);
	if ((tvalue = sram_read8(&tptr)) != value) {
		EMU_printf("data mismatch %02X:%04X - exp %d, act %d",
			(uint16_t) dptr.bank, (uint16_t) dptr.position,
			(uint16_t) value, (uint16_t) tvalue);
	}
}

void sram_read_debug(sram_ptr_t *ptr, uint8_t *data, uint16_t len) {
	EMU_printf("reading %d from %02X:%04X",
		len, (uint16_t) ptr->bank, (uint16_t) ptr->position);
	sram_read(ptr, data, len);
}

void sram_write_debug(sram_ptr_t *ptr, const uint8_t *data, uint16_t len) {
	sram_ptr_t tptr, dptr;
	uint8_t tvalue;
	tptr = *ptr;
	EMU_printf("writing %d to %02X:%04X",
		len, (uint16_t) ptr->bank, (uint16_t) ptr->position);
	sram_write(ptr, data, len);
	for (uint16_t i = 0; i < len; i++) {
		dptr = tptr;
		if ((tvalue = sram_read8(&tptr)) != data[i]) {
			EMU_printf("data mismatch %02X:%04X - exp %d, act %d",
				(uint16_t) dptr.bank, (uint16_t) dptr.position,
				(uint16_t) data[i], (uint16_t) tvalue);
		}
	}
}
#endif

bool sram_alloc(uint16_t len, sram_ptr_t *ptr) {
	uint16_t offset, nlen;
	sram_entry_t entry;

	offset = 0;
	ptr->bank = 0;
	ptr->position = sizeof(sram_header_t);

	uint8_t max_bank = zoo_get_ram_bank_count();
	while (ptr->bank < max_bank) {
		sram_read(ptr, (uint8_t*) &entry, sizeof(sram_entry_t));
		if ((entry.flags & SRAM_FLAG_USED) != 0) {
			offset = 0;
		} else {
			nlen = entry.size + offset;
			if (nlen >= len) {
				if ((nlen - len) <= sizeof(sram_entry_t)) {
					len = nlen;
				}
				// rewind to first entry location
				sram_sub_ptr(ptr, sizeof(sram_entry_t) + offset);
				// allocate entry of size len
				entry.flags = SRAM_FLAG_USED;
				entry.size = len;
				sram_write(ptr, (const uint8_t*) &entry, sizeof(sram_entry_t));
				if (nlen > len) {
					// add new free entry
					sram_add_ptr(ptr, len);
					entry.flags = 0;
					entry.size = nlen - len - sizeof(sram_entry_t);
					sram_write(ptr, (const uint8_t*) &entry, sizeof(sram_entry_t));
					sram_sub_ptr(ptr, len + sizeof(sram_entry_t));
				}
				return true;
			} else {
				offset += sizeof(sram_entry_t) + entry.size;
			}
		}
		sram_add_ptr(ptr, entry.size);
	}

	// TODO: block shifting? is it even doable?
	return false;
}

void sram_free(sram_ptr_t *ptr) {
	sram_entry_t entry;
	sram_sub_ptr(ptr, sizeof(sram_entry_t));
	sram_read(ptr, (uint8_t*) &entry, sizeof(sram_entry_t));
	sram_sub_ptr(ptr, sizeof(sram_entry_t));
	entry.flags &= ~SRAM_FLAG_USED;
	sram_write(ptr, (const uint8_t*) &entry, sizeof(sram_entry_t));
}

static const uint8_t __far sram_expected_magic[4] = {'T', 'n', 'Z', 0x01};

void sram_toggle_write(void) {
	ZOO_SWITCH_RAM(0);
	*((uint8_t __far*) MK_FP(0x1000, 0)) ^= 0x20;
}

void sram_init(bool force) {
	uint8_t magic[8];
	sram_ptr_t ptr;
	sram_entry_t entry;

	ptr.position = 0;
	ptr.bank = 0;

	ZOO_ENABLE_RAM;

	uint8_t prev_bank = _current_bank;
	ZOO_SWITCH_ROM(0x00);

#ifndef RESET_SAVE_ON_START
	sram_read(&ptr, magic, 8);
	if (memcmp(magic, sram_expected_magic, 4)
		|| memcmp(magic + 4, (uint8_t __far*) MK_FP(0x2000, 0), 4)
		|| force)
#endif
	{
		ptr.position = 0;
		sram_write(&ptr, sram_expected_magic, 4);
		sram_write(&ptr, (uint8_t __far*) MK_FP(0x2000, 0), 4);
		// write empty flags
		sram_write8(&ptr, 0);
		// write empty world id
		sram_write8(&ptr, 0xFF);

		// init linked list
		ptr.position = sizeof(sram_header_t);

		entry.flags = 0;
		entry.size = (16384 - sizeof(sram_entry_t) - sizeof(sram_header_t));
		sram_write(&ptr, (const uint8_t*) &entry, sizeof(sram_entry_t));
		sram_add_ptr(&ptr, entry.size);

		entry.size += sizeof(sram_header_t);

		uint8_t max_bank = zoo_get_ram_bank_count();
		if (max_bank == 1) {
			// tinyzoo-ws: only 32K!
			sram_write(&ptr, (const uint8_t*) &entry, sizeof(sram_entry_t));
			sram_add_ptr(&ptr, entry.size);
		} else {
			while (ptr.bank < max_bank) {
				sram_write(&ptr, (const uint8_t*) &entry, sizeof(sram_entry_t));
				sram_add_ptr(&ptr, entry.size);
			}
		}
	}

	ZOO_SWITCH_ROM(prev_bank);
	ZOO_DISABLE_RAM;
}
