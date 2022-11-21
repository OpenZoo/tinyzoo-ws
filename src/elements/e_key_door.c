#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../input.h"
#include "../math.h"
#include "../../res/message_consts.h"
#include "../sound_consts.h"
#include "../timer.h"

const char __far* msg_key_pickup_get(uint8_t x);
const char __far* msg_door_open_get(uint8_t x);

static void show_key_pickup_no(uint8_t key) {
	display_message(200, NULL, msg_key_pickup_no, msg_key_pickup_get(key));
	sound_queue(2, sound_key_failure);
}

static void show_key_pickup_yes(uint8_t key) {
	display_message(200, NULL, msg_key_pickup_yes, msg_key_pickup_get(key));
	sound_queue(2, sound_key_success);
}

static void show_door_open_no(uint8_t key) {
	display_message(200, NULL, msg_door_open_get(key), msg_door_open_no);
	sound_queue(3, sound_door_failure);
}

static void show_door_open_yes(uint8_t key) {
	display_message(200, NULL, msg_door_open_get(key), msg_door_open_yes);
	sound_queue(3, sound_door_success);
}

void ElementKeyTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	zoo_tile_t *tile = &ZOO_TILE(x, y);
	uint8_t key = (tile->color & 7);
	if (key == 0) {
#ifdef FEAT_BLACK_KEYS
		if ((zoo_world_info.gems & 0xFF00) != 0) {
			show_key_pickup_no(0);
		} else {
			zoo_world_info.gems |= 0x100;
			tile->element = E_EMPTY;
			game_update_sidebar_gems_time();

			show_key_pickup_yes(0);
		}
#endif
	} else {
		uint8_t key_shift = 1 << key;

		if (zoo_world_info.keys & key_shift) {
			show_key_pickup_no(key);
		} else {
			zoo_world_info.keys |= key_shift;
			tile->element = E_EMPTY;
			game_update_sidebar_keys();

			show_key_pickup_yes(key);
		}
	}
}

void ElementDoorTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	zoo_tile_t *tile = &ZOO_TILE(x, y);
	uint8_t key = (tile->color >> 4) & 7;
	if (key == 0) {
#ifdef FEAT_BLACK_KEYS
		if ((zoo_world_info.gems & 0xFF00) == 0) {
			show_door_open_no(0);
		} else {
			tile->element = E_EMPTY;
			board_draw_tile(x, y);

			zoo_world_info.gems &= 0x00FF;
			game_update_sidebar_gems_time();

			show_door_open_yes(0);
		}
#endif
	} else {
		uint8_t key_shift = 1 << key;

		if (!(zoo_world_info.keys & key_shift)) {
			show_door_open_no(key);
		} else {
			tile->element = E_EMPTY;
			board_draw_tile(x, y);

			zoo_world_info.keys &= ~key_shift;
			game_update_sidebar_keys();

			show_door_open_yes(key);
		}
	}
}
