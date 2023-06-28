#include <string.h>

#include "p_banking.h"
#include "elements.h"
#include "elements_utils.h"
#include "game.h"
#include "gamevars.h"
#include "config.h"
#include "math.h"
#include "oop.h"
#include "renderer_sidebar.h"
#include "sram_alloc.h"
#include "txtwind.h"

#include "element_defs_cycles.inc"

static inline uint8_t get_color_for_tile_match(uint8_t element, uint8_t color) {
	uint8_t def_color = zoo_element_defs_color[element];
	if (def_color < COLOR_SPECIAL_MIN) {
		return def_color & 0x07;
	} else if (def_color == COLOR_WHITE_ON_CHOICE) {
		return ((color >> 4) & 0x0F) | 8;
	} else {
		return color & 0x0F;
	}
}

bool find_tile_on_board(uint8_t *x, uint8_t *y, uint8_t element, uint8_t color) {
	zoo_tile_t *tile_ptr;
	zoo_tile_t tile;

	uint8_t temp7 = *x;
	uint8_t temp8 = *y;
#ifdef USE_ZOO_TILES_Y
	tile_ptr = zoo_tiles_y[temp8];
#else
	tile_ptr = &zoo_tiles[(BOARD_WIDTH + 2) * temp8];
#endif

	if (color == 0) {
		while (true) {
			if ((++temp7) > BOARD_WIDTH) {
				temp7 = 1;
				if ((++temp8) > BOARD_HEIGHT) {
					return false;
				}
#ifdef USE_ZOO_TILES_Y
				tile_ptr = zoo_tiles_y[temp8];
#else
				tile_ptr = &zoo_tiles[(BOARD_WIDTH + 2) * temp8];
#endif
			}

			*((uint16_t*) &tile) = ((uint16_t*) tile_ptr)[temp7];
			if (tile.element == element) {
				*x = temp7;
				*y = temp8;
				return true;
			}
		}
	} else {
		while (true) {
			if ((++temp7) > BOARD_WIDTH) {
				temp7 = 1;
				if ((++temp8) > BOARD_HEIGHT) {
					return false;
				}
#ifdef USE_ZOO_TILES_Y
				tile_ptr = zoo_tiles_y[temp8];
#else
				tile_ptr = &zoo_tiles[(BOARD_WIDTH + 2) * temp8];
#endif
			}

			*((uint16_t*) &tile) = ((uint16_t*) tile_ptr)[temp7];
			if (tile.element == element) {
				if (get_color_for_tile_match(tile.element, tile.color) == color) {
					*x = temp7;
					*y = temp8;
					return true;
				}
			}
		}
	}
}

void oop_place_tile(uint8_t x, uint8_t y, uint8_t element, uint8_t color) {
	if (!ZOO_TILE_WRITEBOUNDS(x, y)) return;

	zoo_tile_t *src_tile = &ZOO_TILE(x, y);

	if (src_tile->element != E_PLAYER) {
		uint8_t new_color = color;
		uint8_t def_color = zoo_element_defs_color[element];
		if (def_color < COLOR_SPECIAL_MIN) {
			new_color = def_color;
		} else {
			if (new_color == 0) {
				new_color = src_tile->color;
			}

			if (new_color == 0) {
				new_color = 0x0F;
			}

			if (def_color == COLOR_WHITE_ON_CHOICE) {
				new_color = ((new_color - 8) << 4) | 0x0F;
			}
		}

		if (src_tile->element == element) {
			src_tile->color = new_color;
		} else {
			board_damage_tile(x, y);
			if (zoo_element_defs_flags[element] & ELEMENT_TYPICALLY_STATTED) {
				add_stat(x, y, element, new_color, zoo_element_defs_cycles[element], &stat_template_default);
			} else {
				src_tile->element = element;
				src_tile->color = new_color;
			}
		}

		board_draw_tile(x, y);
	}
}

uint16_t oop_dataofs_clone(uint16_t loc) {
	uint8_t len = zoo_stat_data[loc + 3] & 0x7F;
#ifdef DEBUG_PRINTFS
	EMU_printf("cloning dataofs @ %u, len %u, size %u", loc, len, zoo_stat_data_size);
#endif
	_nmemmove(zoo_stat_data + zoo_stat_data_size, zoo_stat_data + loc, len);
	uint16_t new_pos = zoo_stat_data_size;
	zoo_stat_data_size += len;
	return new_pos;
}

static void oop_dataofs_free(uint16_t loc) {
	uint8_t len = zoo_stat_data[loc + 3] & 0x7F;
#ifdef DEBUG_PRINTFS
	EMU_printf("freeing dataofs @ %u, len %u, size %u", loc, len, zoo_stat_data_size);
#endif
	_nmemmove(zoo_stat_data + loc, zoo_stat_data + loc + len, zoo_stat_data_size - loc - len);
	zoo_stat_data_size -= len;

	zoo_stat_t *stat = &ZOO_STAT(0);
	uint8_t stat_id = 0;
	for (; stat_id <= zoo_stat_count; stat_id++, stat++) {
		if (stat->data_ofs != 0xFFFF) {
			if (stat->data_ofs > loc) {
#ifdef DEBUG_PRINTFS
				EMU_printf("shifting stat %d from %u", stat_id, stat->data_ofs);
#endif
				stat->data_ofs -= len;
			}
		}
	}
}

void oop_dataofs_free_if_unused(uint16_t loc, uint8_t except_id) {
	zoo_stat_t *stat = &ZOO_STAT(0);
	uint8_t stat_id = 0;
	for (; stat_id <= zoo_stat_count; stat_id++, stat++) {
		if (stat_id != except_id && stat->data_ofs == loc) {
			return;
		}
	}
	oop_dataofs_free(loc);
}

extern uint16_t oop_window_zzt_lines;

#define TWL_OFS (TXTWIND_LINE_HEADER_LEN - 1)

bool oop_handle_txtwind(void) {
	if (oop_window_zzt_lines > 1) {
		return txtwind_run(RENDER_MODE_TXTWIND) != 255;
	} else if (oop_window_zzt_lines == 1) {
		uint8_t sram_ptr_data[9];
		sram_ptr_t sram_ptr;

		ZOO_ENABLE_RAM;
		sram_ptr.bank = 0;
		sram_ptr.position = SRAM_TEXT_WINDOW_POS;
		sram_read(&sram_ptr, sram_ptr_data, sizeof(sram_ptr_data));
		ZOO_DISABLE_RAM;

		// TODO: This doesn't handle 1-liners prefixed with $ or !.
		// Let's hope they don't actually occur much?

		init_display_message(200, true);
		switch (txtwind_lines) {
			case 1:
				sidebar_show_message(NULL, 3, NULL, 3,
					MK_FP(0x2000, (*((uint16_t**) (sram_ptr_data)))) + TWL_OFS, sram_ptr_data[2]);
				break;
			case 2:
				sidebar_show_message(NULL, 3,
					MK_FP(0x2000, (*((uint16_t**) (sram_ptr_data)))) + TWL_OFS, sram_ptr_data[2],
					MK_FP(0x2000, (*((uint16_t**) (sram_ptr_data + 3)))) + TWL_OFS, sram_ptr_data[5]);
				break;
			default:
				sidebar_show_message(
					MK_FP(0x2000, (*((uint16_t**) (sram_ptr_data)))) + TWL_OFS, sram_ptr_data[2],
					MK_FP(0x2000, (*((uint16_t**) (sram_ptr_data + 3)))) + TWL_OFS, sram_ptr_data[5],
					MK_FP(0x2000, (*((uint16_t**) (sram_ptr_data + 6)))) + TWL_OFS, sram_ptr_data[8]);
				break;
		}

		return false;
	} else {
		return false;
	}
}
