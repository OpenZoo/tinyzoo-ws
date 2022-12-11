#ifndef __INPUT_H__
#define __INPUT_H__

#include <stdbool.h>
#include <stdint.h>
#include <ws.h>

extern int8_t input_delta_x, input_delta_y;
extern uint16_t input_keys, input_held;

#define KEY_UP KEY_X1
#define KEY_DOWN KEY_X3
#define KEY_LEFT KEY_X4
#define KEY_RIGHT KEY_X2

#define KEY_AUP KEY_Y1
#define KEY_ADOWN KEY_Y3
#define KEY_ALEFT KEY_Y4
#define KEY_ARIGHT KEY_Y2

#define input_a_pressed ((input_held & KEY_A) != 0)
#define input_b_pressed ((input_held & KEY_B) != 0)
#define input_select_pressed ((input_held & (KEY_AUP|KEY_ADOWN|KEY_ALEFT|KEY_ARIGHT)) != 0)
#define input_start_pressed ((input_held & KEY_START) != 0)
#define input_shift_pressed input_a_pressed

void input_update_vbl(void);
void input_reset(void);
void input_update(void);
void input_wait_clear(void);

#endif /* __INPUT_H__ */
