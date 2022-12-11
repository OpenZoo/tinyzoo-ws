#include <ws.h>

#include "bank_switch.h"
#include "config.h"
#include "game.h"
#include "gamevars.h"
#include "input.h"
#include "oop.h"
#include "p_renderer.h"
#include "renderer.h"
#include "sram_alloc.h"
#include "txtwind.h"

#define TEXT_WIDTH_INNER 27
#define TEXT_WIDTH_OUTER (TEXT_WIDTH_INNER + 2)

extern uint16_t txtwind_lines;

static void txtwind_draw_line_txtwind(int16_t idx) {
	txtwind_line_t line;
	txtwind_read_line(idx, &line);
	uint8_t offset = 1;

	if ((idx == -1) || (idx == txtwind_lines)) {
		ws_screen_fill(screen1_table, SCR_ENTRY_PALETTE(PAL_SIDEBAR0) | 196, 0, idx & 31, TEXT_WIDTH_OUTER, 1);
		return;
	} else {
		ws_screen_put(screen1_table, SCR_ENTRY_PALETTE(PAL_SIDEBAR0) | 32, 0, idx & 31);
		ws_screen_put(screen1_table, SCR_ENTRY_PALETTE(PAL_SIDEBAR0) | 32, TEXT_WIDTH_OUTER - 1, idx & 31);
	}

	uint16_t pal_color;
	if (USE_COLOR_RENDERER) {
		pal_color = SCR_ENTRY_PALETTE(PAL_SIDEBAR0) | 0x2000;
		if (line.type == TXTWIND_LINE_TYPE_CENTERED) {
			pal_color = SCR_ENTRY_PALETTE(PAL_SIDEBAR0);
			offset = (TEXT_WIDTH_OUTER - line.len) >> 1;
		} else if (line.type == TXTWIND_LINE_TYPE_HYPERLINK) {
			pal_color = SCR_ENTRY_PALETTE(PAL_SIDEBAR0);
			offset = 4;
		}
	} else {
		// TODO: mono renderer
	}

	for (uint8_t i = 1; i <= TEXT_WIDTH_INNER; i++) {
		int8_t iofs = i - offset;
		uint8_t ch = iofs >= 0 && iofs < line.len ? line.text[iofs] : 32;
		ws_screen_put(screen1_table, pal_color | ch, i, idx & 31);
	}

	if (line.type == TXTWIND_LINE_TYPE_HYPERLINK) {
		if (USE_COLOR_RENDERER) {
			ws_screen_put(screen1_table, SCR_ENTRY_PALETTE(PAL_SIDEBAR2) | 0x2000 | 16, 2, idx & 31);
		} else {
			// TODO: mono renderer
		}
	}
}

bool txtwind_exec_line(uint16_t idx);

static uint8_t txtwind_run_txtwind(void) {
	uint8_t i;
	int16_t old_pos = -8;
	int16_t pos = -8;
	uint8_t result = 255;

	draw_offset_y = pos & 31;
	text_update();

	outportb(IO_SCR1_SCRL_X, 4);

	if (USE_COLOR_RENDERER) {
		ws_screen_fill(screen1_table, SCR_ENTRY_PALETTE(PAL_SIDEBAR0) | 32, 0, 0, TEXT_WIDTH_OUTER, 32);
	} else {
		// TODO: mono renderer
	}

	for (i = 0; i < 18; i++) {
		txtwind_draw_line_txtwind(pos + i);
	}

	wait_vbl_done();
	outportw(IO_DISPLAY_CTRL, DISPLAY_SCR1_ENABLE);

	while (true) {
		input_reset();
		wait_vbl_done();
		wait_vbl_done();
		wait_vbl_done();
		wait_vbl_done();

		if (input_keys & KEY_UP) {
			if (pos > -8) {
				pos--;
				txtwind_draw_line_txtwind(pos);
			}
		} else if (input_keys & KEY_DOWN) {
			if (pos < (((int16_t) txtwind_lines) - 9)) {
				txtwind_draw_line_txtwind(pos + 18);
				pos++;
			}
		} else if (input_keys & KEY_A) {
			result = txtwind_exec_line(pos + 8) ? 0 : 255;
			break;
		} else if (input_keys & KEY_B) {
			break;
		}

		draw_offset_y = pos & 31;
		text_update();
		outportb(IO_SCR1_SCRL_X, 4);
		wait_vbl_done();

		if (old_pos != -9 && old_pos != (txtwind_lines - 8)) {
			if (USE_COLOR_RENDERER) {
				ws_screen_put(screen1_table, SCR_ENTRY_PALETTE(PAL_SIDEBAR0) | 32, 0, (old_pos + 8) & 31);
				ws_screen_put(screen1_table, SCR_ENTRY_PALETTE(PAL_SIDEBAR0) | 32, TEXT_WIDTH_OUTER - 1, (old_pos + 8) & 31);
			} else {
				// TODO: mono renderer
			}
		}
		old_pos = pos;
		if (pos != -9 && pos != (txtwind_lines - 8)) {
			if (USE_COLOR_RENDERER) {
				ws_screen_put(screen1_table, SCR_ENTRY_PALETTE(PAL_SIDEBAR2) | 175, 0, (draw_offset_y + 8) & 31);
				ws_screen_put(screen1_table, SCR_ENTRY_PALETTE(PAL_SIDEBAR2) | 174, TEXT_WIDTH_OUTER - 1, (draw_offset_y + 8) & 31);
			} else {
				// TODO: mono renderer
			}
		}
	}

	return result;
}

static uint8_t txtwind_run_menu(void) {
	txtwind_line_t line;
	uint8_t wnd_width = 0;
	uint8_t wnd_height = txtwind_lines;
	uint8_t result = 255;

	draw_offset_x = 0;
	draw_offset_y = 0;
	text_update();

	for (uint8_t idx = 0; idx < wnd_height; idx++) {
		txtwind_read_line(idx, &line);
		if (wnd_width < line.len) {
			wnd_width = line.len;
		}
	}

	uint8_t wnd_x = (28 - (wnd_width + 6)) >> 1;
	uint8_t wnd_y = (18 - (wnd_height + 4)) >> 1;
	uint8_t arrow_y = 0;

	// clear + colors
	uint16_t pal_prefix, pal_prefix_hlt;
	if (USE_COLOR_RENDERER) {
		pal_prefix = SCR_ENTRY_PALETTE(PAL_MENU);
		pal_prefix_hlt = SCR_ENTRY_PALETTE(PAL_MENU) | 0x2000;
	} else {
		// TODO: mono renderer
	}
	ws_screen_fill(screen1_table, pal_prefix | 32, 0, 0, 28, 18);

	// border
	ws_screen_fill(screen1_table, pal_prefix | 205, wnd_x + 1, wnd_y, wnd_width + 4, 1);
	ws_screen_fill(screen1_table, pal_prefix | 205, wnd_x + 1, wnd_y + wnd_height + 3, wnd_width + 4, 1);
	ws_screen_fill(screen1_table, pal_prefix | 186, wnd_x, wnd_y + 1, 1, wnd_height + 2);
	ws_screen_fill(screen1_table, pal_prefix | 186, wnd_x + wnd_width + 5, wnd_y + 1, 1, wnd_height + 2);

	ws_screen_put(screen1_table, pal_prefix | 201, wnd_x, wnd_y);
	ws_screen_put(screen1_table, pal_prefix | 187, wnd_x + wnd_width + 5, wnd_y);
	ws_screen_put(screen1_table, pal_prefix | 200, wnd_x, wnd_y + wnd_height + 3);
	ws_screen_put(screen1_table, pal_prefix | 188, wnd_x + wnd_width + 5, wnd_y + wnd_height + 3);

	// arrow
	ws_screen_put(screen1_table, pal_prefix_hlt | 16, wnd_x + 2, wnd_y + 2 + arrow_y);

	// text
	for (uint8_t idx = 0; idx < wnd_height; idx++) {
		txtwind_read_line(idx, &line);
		for (uint8_t i = 0; i < line.len; i++) {
			ws_screen_put(screen1_table, pal_prefix | ((uint8_t) line.text[i]), wnd_x + 4 + i, wnd_y + 2 + idx);
		}
	}

	wait_vbl_done();
	outportw(IO_DISPLAY_CTRL, DISPLAY_SCR1_ENABLE);

	while (true) {
		input_reset();
		wait_vbl_done();
		wait_vbl_done();
		wait_vbl_done();

		if (input_keys & KEY_UP) {
			if (arrow_y > 0) {
				ws_screen_put(screen1_table, pal_prefix_hlt | 32, wnd_x + 2, wnd_y + 2 + arrow_y);
				arrow_y--;
				ws_screen_put(screen1_table, pal_prefix_hlt | 16, wnd_x + 2, wnd_y + 2 + arrow_y);
			}
		} else if (input_keys & KEY_DOWN) {
			if (arrow_y < (txtwind_lines - 1)) {
				ws_screen_put(screen1_table, pal_prefix_hlt | 32, wnd_x + 2, wnd_y + 2 + arrow_y);
				arrow_y++;
				ws_screen_put(screen1_table, pal_prefix_hlt | 16, wnd_x + 2, wnd_y + 2 + arrow_y);
			}
		} else if (input_keys & KEY_A) {
			result = arrow_y;
			break;
		} else if (input_keys & KEY_B) {
			break;
		}

		wait_vbl_done();
		wait_vbl_done();
		wait_vbl_done();
	}

	return result;
}

uint8_t txtwind_run(uint8_t render_mode) {
	uint8_t old_draw_offset_x = draw_offset_x;
	uint8_t old_draw_offset_y = draw_offset_y;

	uint8_t result;

	wait_vbl_done();
	outportw(IO_DISPLAY_CTRL, 0);
	text_reinit(render_mode);

	if (render_mode == RENDER_MODE_TXTWIND) {
		result = txtwind_run_txtwind();
	} else if (render_mode == RENDER_MODE_MENU) {
		result = txtwind_run_menu();
	}


	input_wait_clear();

	wait_vbl_done();
	outportw(IO_DISPLAY_CTRL, 0);

	draw_offset_x = old_draw_offset_x;
	draw_offset_y = old_draw_offset_y;
	board_redraw();
	// game_update_sidebar_all();
	input_reset();
	wait_vbl_done();
	text_reinit(RENDER_MODE_PLAYFIELD);
	text_update();
	return result;
}
