#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ws.h>
#include <wsx/planar_unpack.h>

#include "p_banking.h"
#include "game.h"
#include "gamevars.h"
#include "p_renderer.h"
#include "renderer.h"
#include "renderer_sidebar.h"
#include "../../res/font_default.h"
#include "../../res/font_small.h"

#define COL_LVL_0 0
#define COL_LVL_1 5
#define COL_LVL_2 10
#define COL_LVL_3 15

const uint8_t __far ws_subpal_idx[16] = {
	4, 5, 6, 7, 12, 13, 14, 15,
	4, 5, 6, 7, 12, 13, 14, 15
};

uint8_t renderer_mode;
uint8_t *sidebar_tile_data;

const uint16_t __far ws_subpal_tile[16] = {
	SCR_ENTRY_PALETTE(4) | 0x0000,
	SCR_ENTRY_PALETTE(5) | 0x0000,
	SCR_ENTRY_PALETTE(6) | 0x0000,
	SCR_ENTRY_PALETTE(7) | 0x0000,
	SCR_ENTRY_PALETTE(12) | 0x0000,
	SCR_ENTRY_PALETTE(13) | 0x0000,
	SCR_ENTRY_PALETTE(14) | 0x0000,
	SCR_ENTRY_PALETTE(15) | 0x0000,
	SCR_ENTRY_PALETTE(4) | 0x2000,
	SCR_ENTRY_PALETTE(5) | 0x2000,
	SCR_ENTRY_PALETTE(6) | 0x2000,
	SCR_ENTRY_PALETTE(7) | 0x2000,
	SCR_ENTRY_PALETTE(12) | 0x2000,
	SCR_ENTRY_PALETTE(13) | 0x2000,
	SCR_ENTRY_PALETTE(14) | 0x2000,
	SCR_ENTRY_PALETTE(15) | 0x2000
};

const uint8_t __far ws_subpal_idx_mono[16] = {
	7, 6, 3, 3,
	5, 5, 4, 2,
	4, 4, 1, 1,
	3, 3, 0, 0
};

const uint16_t __far ws_subpal_tile_mono[16] = {
	SCR_ENTRY_PALETTE(15), // 0
	SCR_ENTRY_PALETTE(14),
	SCR_ENTRY_PALETTE(7),
	SCR_ENTRY_PALETTE(7),
	SCR_ENTRY_PALETTE(13), // 4
	SCR_ENTRY_PALETTE(13),
	SCR_ENTRY_PALETTE(12),
	SCR_ENTRY_PALETTE(6),
	SCR_ENTRY_PALETTE(12), // 8
	SCR_ENTRY_PALETTE(12),
	SCR_ENTRY_PALETTE(5),
	SCR_ENTRY_PALETTE(5),
	SCR_ENTRY_PALETTE(7), // 12
	SCR_ENTRY_PALETTE(7),
	SCR_ENTRY_PALETTE(4),
	SCR_ENTRY_PALETTE(4)
};

const uint16_t __far ws_palette[16] = {
	RGB(COL_LVL_0, COL_LVL_0, COL_LVL_0),
	RGB(COL_LVL_0, COL_LVL_0, COL_LVL_2),
	RGB(COL_LVL_0, COL_LVL_2, COL_LVL_0),
	RGB(COL_LVL_0, COL_LVL_2, COL_LVL_2),
	RGB(COL_LVL_2, COL_LVL_0, COL_LVL_0),
	RGB(COL_LVL_2, COL_LVL_0, COL_LVL_2),
	RGB(COL_LVL_2, COL_LVL_1, COL_LVL_0),
	RGB(COL_LVL_2, COL_LVL_2, COL_LVL_2),
	RGB(COL_LVL_1, COL_LVL_1, COL_LVL_1),
	RGB(COL_LVL_1, COL_LVL_1, COL_LVL_3),
	RGB(COL_LVL_1, COL_LVL_3, COL_LVL_1),
	RGB(COL_LVL_1, COL_LVL_3, COL_LVL_3),
	RGB(COL_LVL_3, COL_LVL_1, COL_LVL_1),
	RGB(COL_LVL_3, COL_LVL_1, COL_LVL_3),
	RGB(COL_LVL_3, COL_LVL_3, COL_LVL_1),
	RGB(COL_LVL_3, COL_LVL_3, COL_LVL_3)
};

uint8_t draw_offset_x = 0;
uint8_t draw_offset_y = 7;
uint8_t renderer_scrolling;

void text_set_opaque_bg(uint8_t color) {
	for (uint8_t i = 0; i < 8; i++) {
		MEM_COLOR_PALETTE(ws_subpal_idx[i])[0] = ws_palette[color];
	}
}

void text_set_sidebar_height(uint8_t height) {
#ifdef HACK_HIDE_STATUSBAR
	uint8_t first = 28;
#else
	uint8_t first = renderer_mode == RENDER_MODE_TITLE ? 28 : 0;
#endif
	uint8_t count = height * 28;
	if (count >= first) count -= first;
	outportb(IO_SPR_FIRST, first);
	outportb(IO_SPR_COUNT, count);
}

uint16_t *sidebar_sprite_table;
uint16_t *screen1_table;

static const uint8_t __far ws_sidebar_palettes[28] = {
        PAL_SIDEBAR0, PAL_SIDEBAR0, PAL_SIDEBAR0, PAL_SIDEBAR0, // 0
	PAL_SIDEBAR1, PAL_SIDEBAR0, PAL_SIDEBAR0, PAL_SIDEBAR0, // 8
        PAL_SIDEBAR0, PAL_SIDEBAR0, PAL_SIDEBAR0, PAL_SIDEBAR0, // 16
        PAL_SIDEBAR2, PAL_SIDEBAR0, PAL_SIDEBAR0, PAL_SIDEBAR0, // 24
        PAL_SIDEBAR0, PAL_SIDEBAR0, PAL_SIDEBAR0, PAL_SIDEBAR0, // 32
	PAL_SIDEBAR0, PAL_SIDEBAR0, PAL_SIDEBAR0, PAL_SIDEBAR0, // 40
	PAL_SIDEBAR1, PAL_SIDEBAR1, PAL_SIDEBAR2, PAL_SIDEBAR0  // 48
};

static void text_rebuild_color_palette(const uint16_t __far* palette) {
	for (uint8_t i = 0; i < 8; i++) {
		MEM_COLOR_PALETTE(ws_subpal_idx[i])[1] = palette[i];
		MEM_COLOR_PALETTE(ws_subpal_idx[i])[2] = palette[i + 8];
	}
	// border colors
	MEM_COLOR_PALETTE(PAL_NONE)[0] = palette[0];
	MEM_COLOR_PALETTE(PAL_NONE)[1] = palette[0];
	MEM_COLOR_PALETTE(PAL_NONE)[2] = palette[1];
	MEM_COLOR_PALETTE(PAL_NONE)[3] = palette[0];

	MEM_COLOR_PALETTE(PAL_MENU)[0] = palette[0];
	MEM_COLOR_PALETTE(PAL_MENU)[1] = palette[15];
	MEM_COLOR_PALETTE(PAL_MENU)[2] = palette[12];
	MEM_COLOR_PALETTE(PAL_SIDEBAR0)[0] = palette[1];
	MEM_COLOR_PALETTE(PAL_SIDEBAR0)[1] = palette[15];
	MEM_COLOR_PALETTE(PAL_SIDEBAR0)[2] = palette[14];
	MEM_COLOR_PALETTE(PAL_SIDEBAR0)[3] = palette[13];
	MEM_COLOR_PALETTE(PAL_SIDEBAR1)[0] = palette[1];
	MEM_COLOR_PALETTE(PAL_SIDEBAR1)[1] = palette[9];
	MEM_COLOR_PALETTE(PAL_SIDEBAR1)[2] = palette[10];
	MEM_COLOR_PALETTE(PAL_SIDEBAR1)[3] = palette[11];
	MEM_COLOR_PALETTE(PAL_SIDEBAR2)[0] = palette[1];
	MEM_COLOR_PALETTE(PAL_SIDEBAR2)[1] = palette[12];
	MEM_COLOR_PALETTE(PAL_SIDEBAR2)[2] = palette[13];
	MEM_COLOR_PALETTE(PAL_SIDEBAR2)[3] = palette[6];
}

static void text_rebuild_sprite_table(void) {
	uint16_t sidebar_tile_offset = 256;
	text_set_sidebar_height(0);
	uint16_t *table = sidebar_sprite_table;
	uint16_t i = 0;
#ifdef HACK_HIDE_STATUSBAR
	i = 28;
#else
	for (; i < 28; i++) {
		*(table++) = SCR_ENTRY_PALETTE((uint16_t) ws_sidebar_palettes[i]) | (sidebar_tile_offset + i) | (1 << 13);
		*(table++) = (i << 11) | 136;
	}
#endif

	for (; i < 112; i++) {
		*(table++) = SCR_ENTRY_PALETTE(PAL_SIDEBAR0) | (1 << 13);
		table++;
	}
}

void text_init(uint8_t mode) {
	if (mode > RENDER_MODE_TITLE) {
		draw_offset_x = 0;
		draw_offset_y = 7;
	}

	outportw(IO_DISPLAY_CTRL, 7 << 8);
	outportb(IO_SPR_FIRST, 0);

	wsx_planar_unpack((uint8_t*) 0x2000, 256 * 8, _font_default_bin, WSX_PLANAR_UNPACK_1BPP_TO_2BPP_ZERO(1));

	if (USE_COLOR_RENDERER) {
		ws_mode_set(WS_MODE_COLOR);
		text_rebuild_color_palette(ws_palette);

		// fill bg / fg
		ws_screen_fill_tiles((uint16_t*) 0x6000, 219 | ws_subpal_tile[0], 0, 0, 32, 32);
		ws_screen_fill_tiles((uint16_t*) 0x6800, 0 | ws_subpal_tile[0], 0, 0, 32, 32);

		wsx_planar_unpack((uint8_t*) 0x4000, 256 * 8, _font_default_bin, WSX_PLANAR_UNPACK_1BPP_TO_2BPP_ZERO(2));

		sidebar_sprite_table = (uint16_t*) 0x3200;
		sidebar_tile_data = (uint8_t*) 0x3000;

		// configure screen 1 (bg) / 2 (fg)
		outportb(IO_SCR_BASE, SCR1_BASE(0x6000) | SCR2_BASE(0x6800));
		outportb(IO_SPR_BASE, SPR_BASE(0x3200));

		screen1_table = (uint16_t*) 0x6000;

		text_rebuild_sprite_table();
	} else {
		ws_display_set_shade_lut(SHADE_LUT(0, 2, 4, 6, 8, 10, 12, 15));

		// fill bg / fg
		ws_screen_fill_tiles((uint16_t*) 0x6000, 219 | ws_subpal_tile[0], 0, 0, 32, 32);
		ws_screen_fill_tiles((uint16_t*) 0x6800, 0 | ws_subpal_tile[0], 0, 0, 32, 32);

		// set palettes
		for (uint8_t i = 0; i < 8; i++) {
			outportw(0x20 + (ws_subpal_idx[i] << 1), (i << 4) | 0x07);
		}
		outportw(0x20 + (PAL_SIDEBAR0 << 1), 0x3006);
		outportw(0x20 + (PAL_SIDEBAR1 << 1), 0x1446);
		outportw(0x20 + (PAL_SIDEBAR2 << 1), 0x4336);
		outportw(0x20 + (PAL_MESSAGE << 1), 0x07);

		sidebar_sprite_table = (uint16_t*) 0x3800;
		sidebar_tile_data = (uint8_t*) 0x3000;

		// configure screen 1 (bg) / 2 (fg)
		outportb(IO_SCR_BASE, SCR1_BASE(0x3000) | SCR2_BASE(0x3800));
		outportb(IO_SPR_BASE, SPR_BASE(0x3800));

		screen1_table = (uint16_t*) 0x3000;
	}

	text_set_opaque_bg(1);
	sidebar_set_message_color(0x0F);

	text_reinit(mode);
}

void text_reinit(uint8_t mode) {
	renderer_mode = mode;

	if (!USE_COLOR_RENDERER && mode <= RENDER_MODE_TITLE) {
		text_rebuild_sprite_table();
	}

	if (mode == RENDER_MODE_PLAYFIELD) {
		text_set_sidebar_height(1);
	} else {
		text_set_sidebar_height(0);
	}

	if (mode <= RENDER_MODE_TITLE) {
		uint16_t *table = sidebar_sprite_table + 56;
#ifdef HACK_HIDE_STATUSBAR
		uint8_t y_offset = 136;
#else
		uint8_t y_offset = mode == RENDER_MODE_TITLE ? 136 : 128;
#endif
       		for (uint8_t i = 0; i < 84; i++) {
			table++;
                	*(table++) = ((i % 28) << 11) | (y_offset - ((i / 28) << 3));
        	}

		// enable display (in some modes)
		if (!USE_COLOR_RENDERER) {
			wait_vbl_done();
		}
		outportw(IO_DISPLAY_CTRL, DISPLAY_SCR1_ENABLE | DISPLAY_SCR2_ENABLE | DISPLAY_SPR_ENABLE);
	} else if (mode == RENDER_MODE_NONE) {
		outportw(IO_DISPLAY_CTRL, 7 << 8);
	}
}

void text_draw(uint8_t x, uint8_t y, uint8_t chr, uint8_t col) {
	x = (x + draw_offset_x) & 0x1F;
	y = (y + draw_offset_y) & 0x1F;
	uint16_t offset = (y << 6) | (x << 1);
	if (USE_COLOR_RENDERER) {
		*((volatile uint16_t*) (0x6000 + offset)) = 219 | ws_subpal_tile[(col & 0x70) >> 4];
		*((volatile uint16_t*) (0x6800 + offset)) = chr | ws_subpal_tile[col & 0x0F];
	} else {
		if (offset < (112 * 2)) return;
		*((volatile uint16_t*) (0x3000 + offset)) = 219 | ws_subpal_tile_mono[(col & 0x70) >> 4];
		*((volatile uint16_t*) (0x3800 + offset)) = chr | ws_subpal_tile_mono[col & 0x0F];
	}
}

extern int8_t viewport_y;
void text_scroll(int8_t dx, int8_t dy) {
	draw_offset_x += dx;
	if (USE_COLOR_RENDERER || (renderer_mode > RENDER_MODE_TITLE && renderer_mode != RENDER_MODE_NONE)) {
		draw_offset_y += dy;
	} else {
		draw_offset_y = 6 + viewport_y;
	}
}

void text_update(void) {
	outportb(IO_SCR1_SCRL_X, draw_offset_x << 3);
	outportb(IO_SCR2_SCRL_X, draw_offset_x << 3);
	outportb(IO_SCR1_SCRL_Y, draw_offset_y << 3);
	outportb(IO_SCR2_SCRL_Y, draw_offset_y << 3);
}

void sidebar_set_message_color(uint8_t color) {
	if (USE_COLOR_RENDERER) {
		MEM_COLOR_PALETTE(PAL_MESSAGE)[0] = 0x0000;
		MEM_COLOR_PALETTE(PAL_MESSAGE)[1] = ws_palette[color & 0x0F];
	} else {
		outportw(0x20 + (PAL_MESSAGE << 1), (ws_subpal_idx_mono[color & 0x0F] << 4) | 7);
	}
}

static uint8_t sidebar_show_line(const uint8_t __far* line, uint8_t sb_offset) {
        if (line != NULL) {
                uint8_t slen = *(line++);
                if (slen > 0) {
			volatile uint16_t *sptr = &sidebar_sprite_table[sb_offset << 1];
			for (uint8_t i = 0; i < 28; i++, sptr += 2) {
				*sptr = SCR_ENTRY_PALETTE(PAL_MESSAGE) | (1 << 13);
			}
			uint8_t soffset = 14 - (slen >> 1);
			sptr = &sidebar_sprite_table[(sb_offset + soffset) << 1];
			for (uint8_t i = 0; i < slen; i++, sptr += 2) {
				*sptr = line[i] | SCR_ENTRY_PALETTE(PAL_MESSAGE) | (1 << 13);
			}
                        return sb_offset + 28;
                }
        }
	return sb_offset;
}

void sidebar_show_message(const char __far* line1, uint8_t bank1, const char __far* line2, uint8_t bank2, const char __far* line3, uint8_t bank3) {
	text_set_sidebar_height(1);
        uint8_t sb_offset = 28;

        uint8_t prev_bank = _current_bank;
        ZOO_SWITCH_ROM(bank3);
        sb_offset = sidebar_show_line(line3, sb_offset);
        ZOO_SWITCH_ROM(bank2);
        sb_offset = sidebar_show_line(line2, sb_offset);
        ZOO_SWITCH_ROM(bank1);
        sb_offset = sidebar_show_line(line1, sb_offset);
        ZOO_SWITCH_ROM(prev_bank);

	text_set_sidebar_height(sb_offset / 28);
}

void sidebar_hide_message(void) {
	text_set_sidebar_height(1);
}
