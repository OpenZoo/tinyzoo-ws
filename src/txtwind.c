#include <string.h>
#include "bank_switch.h"
#include "config.h"
#include "game.h"
#include "gamevars.h"
#include "input.h"
#include "oop.h"
#include "renderer.h"
#include "sram_alloc.h"
#include "txtwind.h"

#include "../res/txtwind_license.inc"

uint16_t txtwind_lines;

void txtwind_init(void) {
	txtwind_lines = 0;
}

void txtwind_append(uint16_t line_ptr, uint8_t line_bank) {
	if (txtwind_lines > MAX_TEXT_WINDOW_LINES) {
		return;
	}

	sram_ptr_t ptr;
	ptr.bank = 0;
	ptr.position = SRAM_TEXT_WINDOW_POS + (txtwind_lines * 3);

	ZOO_ENABLE_RAM;
	sram_write8(&ptr, line_ptr);
	sram_write8(&ptr, line_ptr >> 8);
	sram_write8(&ptr, line_bank);
	ZOO_DISABLE_RAM;

	txtwind_lines++;
}

void txtwind_append_rom(const void __far* line_ptr) {
	uint32_t linear_ptr = (((uint32_t) line_ptr) >> 12) + ((uint16_t) ((uint32_t) line_ptr));
	uint8_t bank = (uint8_t) (linear_ptr >> 16);
	uint16_t offset = (uint16_t) linear_ptr;
	txtwind_append(offset, bank);
}

static void txtwind_open_static(const uint8_t __far* data) {
	txtwind_init();
	const uint16_t __far* data_line = (const uint16_t __far*) data;
	while (*data_line != 0) {
		txtwind_append_rom(data + (*(data_line++)));
	}
}

void txtwind_open_license(void) {
	txtwind_open_static(txtwind_license_data);
}

bool txtwind_exec_line(uint16_t idx) {
	txtwind_line_t line;
	txtwind_read_line(idx, &line);

	if (line.type == TXTWIND_LINE_TYPE_HYPERLINK) {
		if (oop_send_target(line.text[line.len], false, line.text[line.len + 1], false)) {
			return true;
		}
	}

	return false;
}

void txtwind_read_line(int16_t idx, txtwind_line_t *line) {
	if (idx < 0 || idx >= ((int16_t) txtwind_lines)) {
		line->type = TXTWIND_LINE_TYPE_REGULAR;
		line->len = 0;
		return;
	}

	sram_ptr_t ptr;
	far_ptr_t fptr;
	ptr.bank = 0;
	ptr.position = SRAM_TEXT_WINDOW_POS + (idx * 3);

	ZOO_ENABLE_RAM;
	sram_read(&ptr, &fptr, 3);
	ZOO_DISABLE_RAM;

	uint8_t prev_bank = _current_bank;
	ZOO_SWITCH_ROM(fptr.bank);

	const txtwind_line_t __far *tptr = MK_FP(0x2000, fptr.ptr);
	memcpy(line, tptr, TXTWIND_LINE_HEADER_LEN + tptr->len + 2 /* hyperlink */);
	ZOO_SWITCH_ROM(prev_bank);
}
