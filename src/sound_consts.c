#include <stdint.h>
#include "p_banking.h"
#include "sound_consts.h"

#include "sound_freqs.inc"

const uint8_t WS_FAR sound_transporter_move[] = {
	18,
	48, 1, 66, 1, 52, 1, 70, 1, 56, 1, 74, 1, 64, 1, 82, 1
};

const uint8_t WS_FAR sound_ricochet[] = {
	2,
	249, 1
};

const uint8_t WS_FAR sound_damage[] = {
	2,
	16, 1
};

const uint8_t WS_FAR sound_player_zapped[] = {
	10,
	32, 1, 35, 1, 39, 1, 48, 1, 16, 1
};

const uint8_t WS_FAR sound_player_damage[] = {
	8,
	16, 1, 32, 1, 19, 1, 35, 1
};

const uint8_t WS_FAR sound_player_shoot[] = {
	6,
	64, 1, 48, 1, 32, 1
};

const uint8_t WS_FAR sound_player_game_over[] = {
	26,
	32, 3, 35, 3, 39, 3, 48, 3,
	39, 3, 42, 3, 50, 3, 55, 3,
	53, 3, 56, 3, 64, 3, 69, 3,
	16, 10
};

const uint8_t WS_FAR sound_time_running_out[] = {
	14,
	64, 6, 69, 6, 64, 6, 53, 6,
	64, 6, 69, 6, 64, 10
};

const uint8_t WS_FAR sound_destroy_bullet[] = {
	2,
	32, 1
};

const uint8_t WS_FAR sound_destroy[] = {
	8,
	64, 1, 16, 1, 80, 1, 48, 1
};

const uint8_t WS_FAR sound_push[] = {
	2,
	21, 1
};

const uint8_t WS_FAR sound_forest[] = {
	2,
	57, 1
};

const uint8_t WS_FAR sound_invisible[] = {
	4,
	18, 1, 16, 1
};

const uint8_t WS_FAR sound_key_failure[] = {
	4,
	48, 2, 32, 2
};

const uint8_t WS_FAR sound_key_success[] = {
	20,
	64, 1, 68, 1, 71, 1,
	64, 1, 68, 1, 71, 1,
	64, 1, 68, 1, 71, 1,
	80, 2
};

const uint8_t WS_FAR sound_door_failure[] = {
	4,
	23, 1, 16, 1
};

const uint8_t WS_FAR sound_door_success[] = {
	14,
	48, 1, 55, 1, 59, 1,
	48, 1, 55, 1, 59, 1,
	64, 4
};

const uint8_t WS_FAR sound_ammo_pickup[] = {
	6,
	48, 1, 49, 1, 50, 1
};

const uint8_t WS_FAR sound_gem_pickup[] = {
	8,
	64, 1, 55, 1, 52, 1, 48, 1
};

const uint8_t WS_FAR sound_torch_burnout[] = {
	6,
	48, 1, 32, 1, 16, 1
};

const uint8_t WS_FAR sound_torch_pickup[] = {
	6,
	48, 1, 57, 1, 52, 2
};

const uint8_t WS_FAR sound_water_splash[] = {
	4,
	64, 1, 80, 1
};

const uint8_t WS_FAR sound_bomb_activated[] = {
	10,
	48, 1, 53, 1, 64, 1, 69, 1, 80, 1
};

const uint8_t WS_FAR sound_bomb_explosion[] = {
	12,
	96, 1, 80, 1, 60, 1, 48, 1, 32, 1, 16, 1
};

const uint8_t WS_FAR sound_bomb_tick1[] = {
	2,
	248, 1
};

const uint8_t WS_FAR sound_bomb_tick2[] = {
	2,
	248, 1
};

const uint8_t WS_FAR sound_energizer_loop[] = {
	112,
	32, 3, 35, 3, 36, 3, 37, 3, 53, 3, 37, 3, 35, 3, 32, 3, // 16
	48, 3, 35, 3, 36, 3, 37, 3, 53, 3, 37, 3, 35, 3, 32, 3, // 16
	48, 3, 35, 3, 36, 3, 37, 3, 53, 3, 37, 3, 35, 3, 32, 3, // 16
	48, 3, 35, 3, 36, 3, 37, 3, 53, 3, 37, 3, 35, 3, 32, 3, // 16
	48, 3, 35, 3, 36, 3, 37, 3, 53, 3, 37, 3, 35, 3, 32, 3, // 16
	48, 3, 35, 3, 36, 3, 37, 3, 53, 3, 37, 3, 35, 3, 32, 3, // 16
	48, 3, 35, 3, 36, 3, 37, 3, 53, 3, 37, 3, 35, 3, 32, 3 // 16
};

const uint8_t WS_FAR sound_energizer_finish[] = {
	14,
	32, 3, 26, 3, 23, 3, 22, 3,
	21, 3, 19, 3, 16, 3
};

const uint8_t WS_FAR sound_slime_touch[] = {
	4,
	32, 1, 35, 1
};

const uint8_t WS_FAR sound_duplicator_success[] = {
	10,
	48, 2, 50, 2, 52, 2, 53, 2, 55, 2
};

const uint8_t WS_FAR sound_duplicator_failure[] = {
	4,
	24, 1, 22, 1
};

const uint8_t WS_FAR sound_passage_teleport[] = {
	30,
	48, 1, 52, 1, 55, 1, 49, 1,
	53, 1, 56, 1, 50, 1, 54, 1,
	57, 1, 51, 1, 55, 1, 58, 1,
	52, 1, 56, 1, 64, 1
};

const uint8_t WS_FAR sound_scroll_touch[] = {
	20,
	48, 1, 32, 1, 50, 1, 34, 1,
	52, 1, 36, 1, 53, 1, 37, 1,
	55, 1, 39, 1
};

const uint8_t WS_FAR sound_cheat[] = {
	2,
	39, 4
};
