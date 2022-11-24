#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "gamevars.h"
#include "game.h"
#include "renderer_sidebar.h"

void game_update_sidebar_health(void) {
#ifndef HACK_HIDE_STATUSBAR
	if (zoo_world_info.health < 0) {
		zoo_world_info.health = 0;
	}

	sidebar_draw_panel(0, 2, 3, zoo_world_info.health, true);
#endif
}

void game_update_sidebar_ammo(void) {
#ifndef HACK_HIDE_STATUSBAR
	sidebar_draw_panel(8, 132, 3, zoo_world_info.ammo, true);
#endif
}

void game_update_sidebar_gems_time(void) {
#ifndef HACK_HIDE_STATUSBAR
	if (zoo_board_info.time_limit_sec > 0) {
		sidebar_draw_panel(40, 'T', 3, zoo_board_info.time_limit_sec - zoo_world_info.board_time_sec, true);
	} else {
		sidebar_draw_empty(40, 8);
	}
	sidebar_draw_panel(16, 4, 2, zoo_world_info.gems, true);
#endif
}

void game_update_sidebar_torches(void) {
#ifndef HACK_HIDE_STATUSBAR
	sidebar_draw_panel(24, 157, 3, zoo_world_info.torches, true);
#endif
}

void game_update_sidebar_score(void) {
#ifndef HACK_HIDE_STATUSBAR
	sidebar_draw_panel(32, '+', 3, zoo_world_info.score, true);
#endif
}

void game_update_sidebar_keys(void) {
#ifndef HACK_HIDE_STATUSBAR
	sidebar_draw_keys(48, zoo_world_info.keys);
#endif
}

void game_update_sidebar_all(void) {
#ifndef HACK_HIDE_STATUSBAR
	game_update_sidebar_health();
	game_update_sidebar_ammo();
	game_update_sidebar_gems_time();
	game_update_sidebar_torches();
	game_update_sidebar_score();
	game_update_sidebar_keys();
#endif
}
