#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "p_banking.h"
#include "gamevars.h"
#include "math.h"
#include "renderer_sidebar.h"
#include "../../res/font_default.h"
#include "../../res/font_small.h"

extern uint8_t *sidebar_tile_data;

#define TILE_DATA_AT_X(x) (sidebar_tile_data + ((((uint16_t) (x)) << 3) & 0x1F0))

static void sidebar_draw_char(uint8_t x, uint8_t chr, uint8_t col) {
        const uint8_t __far *font_data = _font_small_bin + (chr << 3);
        uint8_t mask = (x & 1) ? 0x0F : 0xF0;
        uint8_t i;

        uint8_t *tile_data = TILE_DATA_AT_X(x);

        if (col == 3) {
                for (i = 0; i < 8; i++) {
                        uint8_t f = font_data[i] & mask;
                        *(tile_data++) |= f;
                        *(tile_data++) |= f;
                }
        } else {
                if (col == 2) tile_data++;
                for (i = 0; i < 8; i++) {
                        uint8_t f = font_data[i] & mask;
                        *(tile_data++) |= f;
                        tile_data++;
                }
        }
}

void sidebar_draw_empty(uint8_t x, uint8_t width) {
	if (!((x | width) & 1)) {
	        uint8_t *tile_data = sidebar_tile_data + (x << 3);
		_nmemset(tile_data, 0, ((uint16_t) width) << 3);
	} else {
		for (uint8_t i = 0; i < width; i++) {
			sidebar_draw_char(x+width,0,0);
		}
	}
}

void sidebar_draw_panel(uint8_t x, uint8_t chr, uint8_t col, int16_t value, bool wide) {
	uint8_t text[5];
	uint8_t data_len = wide ? 64 : 48;

	_nmemset(TILE_DATA_AT_X(x), 0, data_len);
	sidebar_draw_char(x, chr, col);
	uint8_t offset = x + 2;

	if (wide) {
DrawFullNumber:
		// six tiles
		if (value < 0) {
			sidebar_draw_char(offset++, '-', 1);
			value = -value;
		}
		if (value == 0) {
			sidebar_draw_char(offset, '0', 1);
		} else {
			uint8_t text_pos = 5;
			while (value > 0) {
				text[--text_pos] = 48 + (value % 10);
				value /= 10;
			}
			while (text_pos < 5) {
				sidebar_draw_char(offset++, text[text_pos++], 1);
			}
		}
	} else {
#if 0
		if (value <= -100) {
			// TODO?
			sidebar_draw_char(offset++, '?', 1);
		} else if (value >= 10000) {
			sidebar_draw_char(offset++, (value / 10000) + 48, 1);
			sidebar_draw_char(offset++, zoo_mods16_8((value / 1000), 10) + 48, 1);
			sidebar_draw_char(offset++, 'K', 1);
		} else if (value >= 1000) {
			sidebar_draw_char(offset++, (value / 1000) + 48, 1);
			sidebar_draw_char(offset++, 'K', 1);
			if (value > 0) {
				sidebar_draw_char(offset++, '+', 1);
			}
		} else goto DrawFullNumber;
#endif
	}
}

void sidebar_draw_keys(uint8_t x, uint8_t value) {
	_nmemset(TILE_DATA_AT_X(x), 0, 64);

	if (value & 0x02) sidebar_draw_char(x + 1, 0x0C, 1);
	if (value & 0x04) sidebar_draw_char(x + 2, 0x0C, 2);
	if (value & 0x08) sidebar_draw_char(x + 3, 0x0C, 3);
	if (value & 0x10) sidebar_draw_char(x + 4, 0x0C, 1);
	if (value & 0x20) sidebar_draw_char(x + 5, 0x0C, 2);
	if (value & 0x40) sidebar_draw_char(x + 6, 0x0C, 2);
	if (value & 0x80) sidebar_draw_char(x + 7, 0x0C, 1);
}
