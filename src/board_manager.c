#include <string.h>
#include "gamevars.h"
#include "game.h"
#include "p_banking.h"
#include "board_manager.h"
#include "config.h"

static uint8_t board_pointers_bank;
static far_ptr_t *board_pointers_ptr;

void load_world_rom(uint8_t offset) {
	uint8_t prev_bank = _current_bank;
	ZOO_SWITCH_ROM(ROM_DATA_START_BANK);

	far_ptr_t __far* world_ptr_ptr = (far_ptr_t __far*) MK_FP(0x2000, 4 + offset * 3);
	uint8_t *data = world_ptr_ptr->ptr;
	board_pointers_bank = world_ptr_ptr->bank;
	ZOO_SWITCH_ROM(board_pointers_bank);

	// load world info
	memcpy(&zoo_world_info, MK_FP(0x2000, data), sizeof(zoo_world_info_t));
	data += sizeof(zoo_world_info_t);
	board_pointers_ptr = (far_ptr_t*) data;

	ZOO_SWITCH_ROM(prev_bank);
}

void load_board_rom(uint8_t offset) {
	uint8_t prev_bank = _current_bank;
	ZOO_SWITCH_ROM(board_pointers_bank);

	far_ptr_t __far* ptr = &((far_ptr_t __far*) MK_FP(0x2000, board_pointers_ptr))[offset];
	load_board_data_rom(ptr->bank, MK_FP(0x2000, ptr->ptr));

	ZOO_SWITCH_ROM(prev_bank);
}

void load_board_data_rom(uint8_t bank, const uint8_t __far* data) {
	uint8_t prev_bank = _current_bank;
	ZOO_SWITCH_ROM(bank);

	// RLE decode
	uint8_t ix = 1;
	uint8_t iy = 1;
	uint8_t rle_count = 0;
	zoo_tile_t rle_tile;
	while (true) {
		if (rle_count == 0) {
				rle_tile.element = *(data++);
				rle_tile.color = *(data++);
				if (rle_tile.element & 0x80) {
					rle_count = 1;
					rle_tile.element &= 0x7F;
				} else {
					rle_count = *(data++);
				}
		}
		ZOO_TILE_COPY(ZOO_TILE(ix, iy), rle_tile);
		ix++;
		if (ix > BOARD_WIDTH) {
				ix = 1;
				iy++;
				if (iy > BOARD_HEIGHT) break;
		}
		rle_count--;
	}

	memcpy(&zoo_board_info, data, sizeof(zoo_board_info_t));
	data += sizeof(zoo_board_info_t);

	zoo_stat_count = *(data++);

	uint16_t zoo_stat_size = sizeof(zoo_stat_t) * (zoo_stat_count + 1);
	memcpy(zoo_stats + 1, data, zoo_stat_size);
	data += zoo_stat_size;

	zoo_stat_data_size = *((uint16_t __far*)data);
	data += 2;
	memcpy(zoo_stat_data, data, zoo_stat_data_size);

	ZOO_SWITCH_ROM(prev_bank);
}
