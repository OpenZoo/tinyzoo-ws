#include "game.h"
#include "input.h"

uint8_t vbl_ticks;

uint16_t input_keys = 0;
uint16_t input_keys_repressed = 0;
uint16_t input_keys_released = 0;
uint16_t input_held;

int8_t input_delta_x, input_delta_y;

void input_update_vbl(void) {
	uint16_t keys = ws_keypad_scan();
	input_keys |= keys;
	input_keys_repressed |= (keys & input_keys_released);
	input_keys_released |= (input_held & (~keys));
}

void input_reset(void) {
	ia16_critical({
		input_keys = 0;
		input_keys_repressed = 0;
		input_keys_released = 0;
		input_update_vbl();		
	});
}

//#define JOY_REPEAT_DELAY 15
#define JOY_REPEAT_DELAY 11
#define JOY_REPEAT_DELAY_NEXT 2

static uint8_t input_vbls_next[4];
static uint8_t input_last = 0;

void input_update(void) {
	uint16_t keys_pressed;
	uint16_t keys_repressed;
	uint16_t keys_released;

	ia16_critical({
		keys_pressed = input_keys;
		keys_repressed = input_keys_repressed;
		keys_released = input_keys_released;\
	});

	input_delta_x = 0;
	input_delta_y = 0;

	if (keys_pressed & 0x0F0) {
		input_held = (input_held & 0x0F0) | (keys_pressed & 0xF0F);
		for (uint8_t i = 0; i < 4; i++) {
			uint8_t input_id = (input_last + 1) & 3;
			uint8_t input_mask = 1 << (4 | input_id);
			input_last = input_id;

			if (keys_pressed & input_mask) {
				if (keys_repressed & input_mask) {
					goto KeyRepressed;
				} else if (input_held & input_mask) {
					if (keys_released & input_mask) {
						input_held &= ~input_mask;
					}
					if (((uint8_t) (input_vbls_next[input_id] - vbl_ticks)) < 0x80) continue;
					if (!(keys_released & input_mask)) {
						input_vbls_next[input_id] = vbl_ticks + JOY_REPEAT_DELAY_NEXT;
					}
				} else {
					if (!(keys_released & input_mask)) {
KeyRepressed:
						input_held |= input_mask;
						input_vbls_next[input_id] = vbl_ticks + JOY_REPEAT_DELAY;
					}
				}
				if (input_mask == WS_KEY_UP) {
					input_delta_y = -1;
				} else if (input_mask == WS_KEY_DOWN) {
					input_delta_y = 1;
				} else if (input_mask == WS_KEY_LEFT) {
					input_delta_x = -1;
				} else if (input_mask == WS_KEY_RIGHT) {
					input_delta_x = 1;
				}
				break;
			}
		}
	} else {
		input_held = keys_pressed;
	}

	input_reset();
}

void input_wait_clear(void) {
	while (input_keys != 0) {
		input_reset();
		wait_vbl_done();
	}
}
