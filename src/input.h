#ifndef __INPUT_H__
#define __INPUT_H__

#include <stdbool.h>
#include <stdint.h>
#include <ws.h>

extern int8_t input_delta_x, input_delta_y;
extern uint16_t input_keys, input_held;

#define WS_KEY_UP WS_KEY_X1
#define WS_KEY_DOWN WS_KEY_X3
#define WS_KEY_LEFT WS_KEY_X4
#define WS_KEY_RIGHT WS_KEY_X2

#define WS_KEY_AUP WS_KEY_Y1
#define WS_KEY_ADOWN WS_KEY_Y3
#define WS_KEY_ALEFT WS_KEY_Y4
#define WS_KEY_ARIGHT WS_KEY_Y2

#define input_a_pressed ((input_held & WS_KEY_A) != 0)
#define input_b_pressed ((input_held & WS_KEY_B) != 0)
#define input_select_pressed ((input_held & (WS_KEY_AUP|WS_KEY_ADOWN|WS_KEY_ALEFT|WS_KEY_ARIGHT)) != 0)
#define input_start_pressed ((input_held & WS_KEY_START) != 0)
#define input_shift_pressed input_a_pressed

void input_update_vbl(void);
void input_reset(void);
void input_update(void);
void input_wait_clear(void);

#endif /* __INPUT_H__ */
