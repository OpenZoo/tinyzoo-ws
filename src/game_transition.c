#include "game.h"

#include "config.h"
#include "game_transition.h"
#include "renderer.h"

#ifdef FEAT_BOARD_TRANSITIONS

#include "../res/game_transition_table.inc"

static volatile uint16_t transition_pos = 0xFFFF;
static uint8_t transition_color;

extern int8_t viewport_x;
extern int8_t viewport_y;

void game_transition_start(uint8_t color) {
	ZOO_BUSYLOOP(transition_pos < (TRANSITION_TABLE_28_18_ENTRY_COUNT * 2));

	transition_pos = 0;
	transition_color = color;
}

void game_transition_step(void) {
        if (transition_pos >= (TRANSITION_TABLE_28_18_ENTRY_COUNT * 2)) return;

        const uint8_t __far* arr = transition_table_28_18 + transition_pos;
	if (transition_color == 0xFF) {
                for (uint8_t i = 0; i < 9; i++) {
                        uint8_t x = *(arr++);
                        uint8_t y = *(arr++);
			board_draw_tile(viewport_x + x, viewport_y + y);
		}
		transition_pos += 18;
	} else {
                for (uint8_t i = 0; i < 9; i++) {
                        uint8_t x = *(arr++);
                        uint8_t y = *(arr++);
                        text_draw(x, y, 178, transition_color);
                }
		transition_pos += 18;
	}
}

bool game_transition_running(void) {
	return transition_pos < (TRANSITION_TABLE_28_18_ENTRY_COUNT * 2);
}

#endif

/* void game_transition_tick_to_end(void) {
	while (transition_pos != TRANSITION_TABLE_28_18_ENTRY_COUNT) {
		game_transition_step();
	}
} */
