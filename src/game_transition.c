#include "game.h"

#include "config.h"
#include "game_transition.h"

#ifdef FEAT_BOARD_TRANSITIONS

static uint8_t transition_color;

extern int8_t viewport_x;
extern int8_t viewport_y;

void game_transition_start(uint8_t color) {
	// TODO
	transition_color = color;
}

void game_transition_step(void) {
	// TODO
}

bool game_transition_running(void) {
	// TODO
	return false;
}

#endif

/* void game_transition_tick_to_end(void) {
	// TODO
} */
