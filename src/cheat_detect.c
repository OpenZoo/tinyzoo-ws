#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "p_banking.h"
#include "config.h"
#include "game.h"
#include "gamevars.h"
#include "input.h"
#include "sound_consts.h"
#include "timer.h"

#ifdef SHOW_CHEATS

static const uint16_t WS_FAR cheat_update_table[] = {
	WS_KEY_UP, 0, WS_KEY_UP,
	0, WS_KEY_DOWN, 0, WS_KEY_DOWN,
	0, WS_KEY_LEFT, 0, WS_KEY_RIGHT,
	0, WS_KEY_LEFT, 0, WS_KEY_RIGHT,
	0, WS_KEY_B, 0, WS_KEY_A
};
#define CHEAT_UPDATE_TABLE_LEN 19

void cheat_detect_reset(void) {
	if (cheat_active != 255) cheat_active = 0;
}

void cheat_detect_update(void) {
	if (cheat_active == 255) return;

	if (input_held == cheat_update_table[cheat_active]) {
		cheat_active++;
		if (cheat_active >= CHEAT_UPDATE_TABLE_LEN) {
			cheat_active = 255;
			sound_queue(10, sound_cheat);
		}
	}
}

#endif
