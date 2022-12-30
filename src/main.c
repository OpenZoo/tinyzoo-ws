#include <stdint.h>
#include "../res/font_default.h"
#include "p_banking.h"
#include "game.h"
#include "math.h"
#include "renderer.h"
#include "sram_alloc.h"
#include "timer.h"

#include "board_manager.h"

#ifdef PROFILING
#include <gb/bgb_emu.h>
#endif

void init_main(void) {
	vbl_timer_init();
	wait_vbl_done();

	zsrand(1);

	sound_init();

	text_init(RENDER_MODE_NONE);

	sram_init(false);

	zoo_game_state.world_id = 0;

	timer_init();

	game_menu_act_enter_world(zoo_game_state.world_id, false, true);
}

void main(void) {
	ZOO_INIT_ROM;

	init_main();

	game_play_loop(true);

	while (1);
}
