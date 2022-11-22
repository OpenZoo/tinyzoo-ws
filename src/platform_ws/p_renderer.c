#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ws.h>

#include "bank_switch.h"
#include "gamevars.h"
#include "renderer.h"
#include "../../res/font_default.h"

#define COL_LVL_0 0
#define COL_LVL_1 5
#define COL_LVL_2 10
#define COL_LVL_3 15

// #define USE_COLOR_RENDERER system_color_present()
#define USE_COLOR_RENDERER 0

const uint8_t __far ws_subpal_idx[16] = {
	0, 1, 2, 3, 8, 9, 10, 11,
	0, 1, 2, 3, 8, 9, 10, 11
};

const uint16_t __far ws_subpal_tile[16] = {
	SCR_ENTRY_PALETTE(0),
	SCR_ENTRY_PALETTE(1),
	SCR_ENTRY_PALETTE(2),
	SCR_ENTRY_PALETTE(3),
	SCR_ENTRY_PALETTE(8),
	SCR_ENTRY_PALETTE(9),
	SCR_ENTRY_PALETTE(10),
	SCR_ENTRY_PALETTE(11),
	SCR_ENTRY_PALETTE(0) | 0x100,
	SCR_ENTRY_PALETTE(1) | 0x100,
	SCR_ENTRY_PALETTE(2) | 0x100,
	SCR_ENTRY_PALETTE(3) | 0x100,
	SCR_ENTRY_PALETTE(8) | 0x100,
	SCR_ENTRY_PALETTE(9) | 0x100,
	SCR_ENTRY_PALETTE(10) | 0x100,
	SCR_ENTRY_PALETTE(11) | 0x100
};

const uint16_t __far ws_subpal_tile_mono[16] = {
	SCR_ENTRY_PALETTE(11), // 0
	SCR_ENTRY_PALETTE(10),
	SCR_ENTRY_PALETTE(3),
	SCR_ENTRY_PALETTE(3),
	SCR_ENTRY_PALETTE(9), // 4
	SCR_ENTRY_PALETTE(9),
	SCR_ENTRY_PALETTE(8),
	SCR_ENTRY_PALETTE(2),
	SCR_ENTRY_PALETTE(8), // 8
	SCR_ENTRY_PALETTE(8),
	SCR_ENTRY_PALETTE(1),
	SCR_ENTRY_PALETTE(1),
	SCR_ENTRY_PALETTE(3), // 12
	SCR_ENTRY_PALETTE(3),
	SCR_ENTRY_PALETTE(0),
	SCR_ENTRY_PALETTE(0)
};

#define WS_SUBPAL_TRANS 0x800

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

void font_8x8_install(volatile uint8_t *vptr, uint8_t color_bg, uint8_t color_fg) {
	uint8_t mask_bg_1 = ((color_bg & 1) ? 0xFF : 0x00);
	uint8_t mask_bg_2 = ((color_bg & 2) ? 0xFF : 0x00);
	uint8_t mask_fg_1 = ((color_fg & 1) ? 0xFF : 0x00);
	uint8_t mask_fg_2 = ((color_fg & 2) ? 0xFF : 0x00);

	const uint8_t __far *fptr = _font_default_bin;
	for (uint16_t i = 0; i < 2048; i++, fptr++) {
		uint8_t vf = *fptr;
		uint8_t v1 = (mask_bg_1 & (~vf)) | (mask_fg_1 & (vf));
		uint8_t v2 = (mask_bg_2 & (~vf)) | (mask_fg_2 & (vf));
		*(vptr++) = v1;
		*(vptr++) = v2;
	}
}

void text_set_opaque_bg(uint8_t color) {
	for (uint8_t i = 0; i < 8; i++) {
		MEM_COLOR_PALETTE(ws_subpal_idx[i])[0] = ws_palette[color];
	}
}

void text_init(uint8_t mode) {
	if (mode > RENDER_MODE_TITLE) {
		draw_offset_x = 0;
		draw_offset_y = 0;
	}

	outportw(IO_DISPLAY_CTRL, 0);

	if (USE_COLOR_RENDERER) {
		system_set_mode(MODE_COLOR);

		// set palettes
		for (uint8_t i = 0; i < 8; i++) {
			MEM_COLOR_PALETTE(ws_subpal_idx[i])[0] = 0x0000;
			MEM_COLOR_PALETTE(ws_subpal_idx[i])[1] = ws_palette[i];
			MEM_COLOR_PALETTE(ws_subpal_idx[i])[2] = ws_palette[i + 8];

			MEM_COLOR_PALETTE(ws_subpal_idx[i] | 4)[0] = 0x0000;
			MEM_COLOR_PALETTE(ws_subpal_idx[i] | 4)[1] = ws_palette[i];
			MEM_COLOR_PALETTE(ws_subpal_idx[i] | 4)[2] = ws_palette[i + 8];
		}

		// fill bg / fg
		video_screen_fill(0x6000, 219 | ws_subpal_tile[0], 0, 0, 32, 32);
		video_screen_fill(0x6800, 0 | WS_SUBPAL_TRANS | ws_subpal_tile[0], 0, 0, 32, 32);

		font_8x8_install(0x2000, 0, 1);
		font_8x8_install(0x3000, 0, 2);

		// configure screen 1 (bg) / 2 (fg)
		outportb(IO_SCR_BASE, SCR1_BASE(0x6000) | SCR2_BASE(0x6800));
		outportw(IO_DISPLAY_CTRL, DISPLAY_SCR1_ENABLE | DISPLAY_SCR2_ENABLE);
	} else {
		video_shade_lut_set(GRAY_LUT(0, 3, 5, 7, 9, 11, 13, 15));

		// fill bg / fg
		video_screen_fill(0x6000, 219 | ws_subpal_tile[0], 0, 0, 32, 32);
		video_screen_fill(0x6800, 0 | WS_SUBPAL_TRANS | ws_subpal_tile[0], 0, 0, 32, 32);

		// set palettes
		for (uint8_t i = 0; i < 8; i++) {
			outportw(0x20 + (ws_subpal_idx[i] << 1), i << 4);
			outportw(0x28 + (ws_subpal_idx[i] << 1), i << 4);
		}

		font_8x8_install(0x2000, 0, 1);

		// configure screen 1 (bg) / 2 (fg)
		outportb(IO_SCR_BASE, SCR1_BASE(0x3000) | SCR2_BASE(0x3800));
		outportw(IO_DISPLAY_CTRL, DISPLAY_SCR1_ENABLE | DISPLAY_SCR2_ENABLE);
	}
}

void text_reinit(uint8_t mode) {
	// TODO
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
		*((volatile uint16_t*) (0x6800 + offset)) = chr | WS_SUBPAL_TRANS | ws_subpal_tile[col & 0x0F];
	} else {
		*((volatile uint16_t*) (0x3000 + offset)) = 219 | ws_subpal_tile_mono[(col & 0x70) >> 4];
		*((volatile uint16_t*) (0x3800 + offset)) = chr | WS_SUBPAL_TRANS | ws_subpal_tile_mono[col & 0x0F];
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
