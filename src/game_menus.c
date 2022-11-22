#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "../res/menu_entry_consts.h"
#include "board_manager.h"
#include "config.h"
#include "elements.h"
#include "game.h"
#include "input.h"
#include "renderer.h"
#include "sound_consts.h"
#include "timer.h"
#include "txtwind.h"

static void board_pause_enter(void) {
	zoo_game_state.paused = true; // TODO: not here...

	board_enter_stage1();
	board_enter_stage2();
	board_enter_stage3();
}

void game_menu_act_enter_world(uint8_t world_id, bool new_game, bool first_launch) {
	world_create();
	load_world(world_id, new_game);
	uint8_t starting_board = zoo_world_info.current_board;

#ifdef SHOW_TITLE
	if (first_launch) {
		load_board(0);
		viewport_reset();

		zoo_game_state.game_state_element = E_MONITOR;
		zoo_game_state.paused = false;
		game_play_loop(true);

		input_wait_clear();

		sound_clear_queue();
	}
#endif

#ifndef HACK_HIDE_STATUSBAR
	viewport_full_board = false;
#endif

	zoo_game_state.game_state_element = E_PLAYER;
	load_board(starting_board);
	board_pause_enter();
}

static void game_menu_act_about(void) {
	txtwind_open_license();
	txtwind_run(RENDER_MODE_TXTWIND);
}

#ifdef SHOW_CHEATS
static void game_menu_act_trainer(void) {
	txtwind_init();
	txtwind_append((uint16_t) menu_entry_trainer_zap, 3);
	txtwind_append((uint16_t) menu_entry_trainer_health, 3);
	txtwind_append((uint16_t) menu_entry_trainer_ammo, 3);
	txtwind_append((uint16_t) menu_entry_trainer_keys, 3);
	txtwind_append((uint16_t) menu_entry_trainer_torches, 3);
	txtwind_append((uint16_t) menu_entry_trainer_time, 3);
	txtwind_append((uint16_t) menu_entry_trainer_gems, 3);
	txtwind_append((uint16_t) menu_entry_trainer_dark, 3);
	switch (txtwind_run(RENDER_MODE_MENU)) {
	case 0: {
		for (uint8_t i = 0; i < 4; i++) {
			uint8_t ix = ZOO_STAT(0).x + neighbor_delta_x[i];
			uint8_t iy = ZOO_STAT(0).y + neighbor_delta_y[i];
			if (ZOO_TILE_WRITEBOUNDS(ix, iy)) {
				board_damage_tile(ix, iy);
				ZOO_TILE(ix, iy).element = E_EMPTY;
				board_draw_tile(ix, iy);
			}
		}
	} break;
	case 1: zoo_world_info.health += 50; break;
	case 2: zoo_world_info.ammo += 20; break;
	case 3: zoo_world_info.keys = 0xFF; break;
	case 4: zoo_world_info.torches += 5; break;
	case 5: zoo_world_info.board_time_sec -= 60; break;
	case 6: zoo_world_info.gems += 15; break;
	case 7: zoo_board_info.flags ^= BOARD_IS_DARK; board_redraw(); break;
	}
	sound_queue(10, sound_cheat);
	game_update_sidebar_all();
}
#endif

bool game_pause_menu(void) {
MenuStart:
	if (zoo_world_info.health <= 0) {
		// "Game Over" menu
		txtwind_init();
		txtwind_append((uint16_t) menu_entry_new_game, 3);
		txtwind_append((uint16_t) menu_entry_continue, 3);
		txtwind_append((uint16_t) menu_entry_about, 3);
#ifdef SHOW_CHEATS
		if (cheat_active == 255) {
			txtwind_append((uint16_t) menu_entry_trainer, 3);
		}
#endif
		switch (txtwind_run(RENDER_MODE_MENU)) {
		case 0: { /* NEW GAME */
			game_menu_act_enter_world(zoo_game_state.world_id, true, false);
			return true;
		} break;
		case 1: { /* CONTINUE */
			clear_saved_board(zoo_world_info.current_board);
			game_menu_act_enter_world(zoo_game_state.world_id, false, false);
			return true;
		} break;
		case 2: { /* ABOUT */
			game_menu_act_about();
			goto MenuStart;
		} break;
#ifdef SHOW_CHEATS
		case 3: { /* TRAINER */
			game_menu_act_trainer();
			return false;
		} break;
#endif
		}
	} else {
		// Regular menu
		txtwind_init();
		txtwind_append((uint16_t) menu_entry_continue, 3);
		txtwind_append((uint16_t) menu_entry_restart, 3);
		txtwind_append((uint16_t) menu_entry_about, 3);
#ifdef SHOW_CHEATS
		if (cheat_active == 255) {
			txtwind_append((uint16_t) menu_entry_trainer, 3);
		}
#endif
		switch (txtwind_run(RENDER_MODE_MENU)) {
		case 1: { /* RESTART */
			game_menu_act_enter_world(zoo_game_state.world_id, true, false);
			return true;
		} break;
		case 2: { /* ABOUT */
			game_menu_act_about();
			goto MenuStart;
		} break;
#ifdef SHOW_CHEATS
		case 3: { /* TRAINER */
			game_menu_act_trainer();
			return false;
		} break;
#endif
		}
	}

	return false;
}
