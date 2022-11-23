#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ws.h>

#include "bank_switch.h"
#include "gamevars.h"
#include "renderer.h"
#include "renderer_sidebar.h"
#include "../../res/font_default.h"

#define COL_LVL_0 0
#define COL_LVL_1 5
#define COL_LVL_2 10
#define COL_LVL_3 15

#define USE_COLOR_RENDERER system_color_present()
// #define USE_COLOR_RENDERER 0

const uint8_t __far ws_subpal_idx[16] = {
	4, 5, 6, 7, 12, 13, 14, 15,
	4, 5, 6, 7, 12, 13, 14, 15
};

// palettes:
// 4, 5, 6, 7, 12, 13, 14, 15 - text mode emulation
#define PAL_SIDEBAR0 8
#define PAL_SIDEBAR1 9
#define PAL_SIDEBAR2 10
#define PAL_MESSAGE 11

const uint16_t __far ws_subpal_tile[16] = {
	SCR_ENTRY_PALETTE(4),
	SCR_ENTRY_PALETTE(5),
	SCR_ENTRY_PALETTE(6),
	SCR_ENTRY_PALETTE(7),
	SCR_ENTRY_PALETTE(12),
	SCR_ENTRY_PALETTE(13),
	SCR_ENTRY_PALETTE(14),
	SCR_ENTRY_PALETTE(15),
	SCR_ENTRY_PALETTE(4) | 0x100,
	SCR_ENTRY_PALETTE(5) | 0x100,
	SCR_ENTRY_PALETTE(6) | 0x100,
	SCR_ENTRY_PALETTE(7) | 0x100,
	SCR_ENTRY_PALETTE(12) | 0x100,
	SCR_ENTRY_PALETTE(13) | 0x100,
	SCR_ENTRY_PALETTE(14) | 0x100,
	SCR_ENTRY_PALETTE(15) | 0x100
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

uint8_t draw_offset_x;
uint8_t draw_offset_y;
uint8_t renderer_scrolling;

void font_8x8_install(volatile uint8_t *vptr, bool col2) {
	const uint8_t __far *fptr = _font_default_bin;
	if (col2) {
		for (uint16_t i = 0; i < 2048; i++, fptr++) {
			uint8_t vf = *fptr;
			*(vptr++) = 0x00;
			*(vptr++) = vf;
		}
	} else {
		for (uint16_t i = 0; i < 2048; i++, fptr++) {
			uint8_t vf = *fptr;
			*(vptr++) = vf;
			*(vptr++) = 0x00;
		}
	}
}

void text_set_opaque_bg(uint8_t color) {
	for (uint8_t i = 0; i < 8; i++) {
		MEM_COLOR_PALETTE(ws_subpal_idx[i])[0] = ws_palette[color];
	}
}

void text_set_sidebar_height(uint8_t height) {
#ifdef HACK_HIDE_STATUSBAR
	if (height > 0) height--;
#endif
	outportb(IO_SPR_COUNT, height * 28);
}

uint16_t *sidebar_sprite_table;

void text_init(uint8_t mode) {
	if (mode > RENDER_MODE_TITLE) {
		draw_offset_x = 0;
		draw_offset_y = 0;
	}

	outportw(IO_DISPLAY_CTRL, 0);
	outportb(IO_SPR_FIRST, 0);

	if (USE_COLOR_RENDERER) {
		system_set_mode(MODE_COLOR);

		// set palettes
		for (uint8_t i = 0; i < 8; i++) {
			MEM_COLOR_PALETTE(ws_subpal_idx[i])[1] = ws_palette[i];
			MEM_COLOR_PALETTE(ws_subpal_idx[i])[2] = ws_palette[i + 8];
		}
		MEM_COLOR_PALETTE(PAL_SIDEBAR0)[0] = ws_palette[1];

		// fill bg / fg
		video_screen_fill(0x6000, 219 | ws_subpal_tile[0], 0, 0, 32, 32);
		video_screen_fill(0x6800, 0 | ws_subpal_tile[0], 0, 0, 32, 32);

		font_8x8_install(0x2000, false);
		font_8x8_install(0x3000, true);;

		sidebar_sprite_table = 0x7000;

		// configure screen 1 (bg) / 2 (fg)
		outportb(IO_SCR_BASE, SCR1_BASE(0x6000) | SCR2_BASE(0x6800));
		outportb(IO_SPR_BASE, SPR_BASE(0x7000));
	} else {
		video_shade_lut_set(GRAY_LUT(0, 3, 5, 7, 9, 11, 13, 15));

		// fill bg / fg
		video_screen_fill(0x6000, 219 | ws_subpal_tile[0], 0, 0, 32, 32);
		video_screen_fill(0x6800, 0 | ws_subpal_tile[0], 0, 0, 32, 32);

		// set palettes
		for (uint8_t i = 0; i < 8; i++) {
			outportw(0x20 + (ws_subpal_idx[i] << 1), i << 4);
		}

		font_8x8_install(0x2000, false);

		// TODO: sidebar sprite table

		// configure screen 1 (bg) / 2 (fg)
		outportb(IO_SCR_BASE, SCR1_BASE(0x3000) | SCR2_BASE(0x3800));
	}

	text_set_opaque_bg(1);
	sidebar_set_message_color(0x0F);

	// populate sprite table
	uint16_t *table = sidebar_sprite_table;
	for (uint16_t i = 0; i < 112; i++) {
		*(table++) = SCR_ENTRY_PALETTE(PAL_SIDEBAR0) | (1 << 13);
		*(table++) = ((i % 28) << 11) | (136 - ((i / 28) << 3));
	}
	text_set_sidebar_height(0);

	// enable display
	outportw(IO_DISPLAY_CTRL, DISPLAY_SCR1_ENABLE | DISPLAY_SCR2_ENABLE | DISPLAY_SPR_ENABLE);

	text_reinit(mode);
}

void text_reinit(uint8_t mode) {
	if (mode == RENDER_MODE_PLAYFIELD) {
		text_set_sidebar_height(1);
	} else {
		text_set_sidebar_height(0);
	}
}

void text_sync_hblank_safe(void) {
	// TODO
}

void text_undraw(uint8_t x, uint8_t y) {

}

void text_draw(uint8_t x, uint8_t y, uint8_t chr, uint8_t col) {
	x = (x + draw_offset_x) & 0x1F;
	y = (y + draw_offset_y) & 0x1F;
	uint16_t offset = (y << 6) | (x << 1);
	if (USE_COLOR_RENDERER) {
		*((volatile uint16_t*) (0x6000 + offset)) = 219 | ws_subpal_tile[(col & 0x70) >> 4];
		*((volatile uint16_t*) (0x6800 + offset)) = chr | ws_subpal_tile[col & 0x0F];
	} else {
		*((volatile uint16_t*) (0x3000 + offset)) = 219 | ws_subpal_tile_mono[(col & 0x70) >> 4];
		*((volatile uint16_t*) (0x3800 + offset)) = chr | ws_subpal_tile_mono[col & 0x0F];
	}
}

void text_free_line(uint8_t y) {

}

void text_scroll(int8_t dx, int8_t dy) {
	draw_offset_x += dx;
	draw_offset_y += dy;
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
		outportw(0x20 + (PAL_MESSAGE << 1), (ws_subpal_idx_mono[color & 0x0F] << 4));
	}
}

static uint8_t sidebar_show_line(const char __far* line, uint8_t sb_offset) {
        if (line != NULL) {
                uint8_t slen = *(line++);
                if (slen > 0) {
			// TODO: mono support
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
#ifdef HACK_HIDE_STATUSBAR
        uint8_t sb_offset = 0;
#else
        uint8_t sb_offset = 28;
#endif

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
