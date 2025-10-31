#include <ws.h>
#include "p_banking.h"

#define IRAM_IMPLEMENTATION
#include "iram.h"

static const uint8_t __far bank_counts[] = {0, 0, 1, 2, 4, 8};
static const uint8_t __far bank_masks[] = {
	0xFE, // 0x00 = 2 banks
	0xFC, // 0x01 = 4 banks
	0xF8, // 0x02 = 8 banks
	0xF0, // 0x03 = 16 banks
	0xE0, // 0x04 = 32 banks
	0xE0, // 0x05 = 48 banks (unsupported)
	0xC0, // 0x06 = 64 banks
	0xC0, // 0x07 = 96 banks (unsupported)
	0x80, // 0x08 = 128 banks
	0x00  // 0x09 = 256 banks
};
uint8_t _current_bank;
uint8_t bank_mask;

void zoo_switch_init(void) {
	uint8_t linear_offset = (inportb(WS_CART_BANK_ROML_PORT) << 4) | 0xF;
	uint8_t rom_mask = bank_masks[* ((uint8_t __far*) MK_FP(0xFFFF, 0x000A))];
	bank_mask = rom_mask & linear_offset;
}

void zoo_switch_rom(uint8_t bank) {
	bank |= bank_mask;
	_current_bank = bank;
	outportb(WS_CART_BANK_ROM0_PORT, bank);
}

uint8_t zoo_get_ram_bank_count(void) {
	return bank_counts[* ((uint8_t __far*) MK_FP(0xFFFF, 0x000B))];
}
