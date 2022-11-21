#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../game_transition.h"
#include "../math.h"
#include "../renderer_sidebar.h"
#include "../sound_consts.h"
#include "../timer.h"

static const zoo_tile_t __far empty_tile = {E_EMPTY, 0};

void ElementPassageTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	uint8_t col = ZOO_TILE(x, y).color;
	uint8_t old_board = zoo_world_info.current_board;

#ifdef FEAT_BOARD_TRANSITIONS
	game_transition_board_change_start();
	sound_queue(4, sound_passage_teleport); // gbzoo: moved from near board_enter() to here
#endif
	sidebar_hide_message();
	board_change(ZOO_STAT_AT(x, y).p3);

	uint8_t new_x = 0, new_y;
	for (uint8_t ix = 1; ix <= BOARD_WIDTH; ix++) {
		for (uint8_t iy = 1; iy <= BOARD_HEIGHT; iy++) {
			zoo_tile_t tile;
			ZOO_TILE_ASSIGN(tile, ix, iy);
			if (tile.element == E_PASSAGE && tile.color == col) {
				new_x = ix;
				new_y = iy;
			}
		}
	}

	ZOO_TILE_COPY(ZOO_TILE(ZOO_STAT(0).x, ZOO_STAT(0).y), empty_tile);
	if (new_x != 0) {
		ZOO_STAT(0).x = new_x;
		ZOO_STAT(0).y = new_y;
	}

	zoo_game_state.paused = true;
#ifdef FEAT_BOARD_TRANSITIONS
	board_enter_stage1();
	game_transition_board_change_end();
	board_enter_stage2();
	ZOO_BUSYLOOP(game_transition_running());
	board_enter_stage3();
#else
	sound_queue(4, sound_passage_teleport);
	board_enter_stage1();
	board_enter_stage2();
	board_redraw();
	board_enter_stage3();
#endif

	*dx = 0;
	*dy = 0;
}
