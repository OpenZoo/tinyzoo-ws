#include <ws.h>
#include "game_transition.h"
#include "input.h"
#include "renderer.h"
#include "timer.h"
#include "ws/sound.h"
#include "ws/system.h"

extern uint16_t dhsecs;
extern uint8_t vbl_ticks;

extern void __attribute__((interrupt)) line_int_handler(void);
extern void __attribute__((interrupt)) timer_int_handler(void);

void __attribute__((interrupt)) vblank_int_handler(void) {
	vbl_ticks++;

	input_update_vbl();

#ifdef FEAT_BOARD_TRANSITIONS
	game_transition_step();
#endif

	ws_int_ack(WS_INT_ACK_VBLANK);
}

void vbl_timer_init(void) {
	ia16_critical({
        ws_int_set_handler(WS_INT_VBLANK, vblank_int_handler);
        ws_int_enable(WS_INT_ENABLE_VBLANK);		
	});
}

void sound_reset(void) {
	outportb(WS_SOUND_CH_CTRL_PORT, 0x00);
}

void timer_init(void) {
	// init audio
	ws_sound_reset();
	ws_sound_set_wavetable_address((void*) 0x1FC0);
	outportb(WS_SOUND_VOL_CH4_PORT, 0x77);
	outportb(WS_SOUND_VOICE_VOL_PORT, WS_SOUND_VOICE_VOL_LEFT_HALF | WS_SOUND_VOICE_VOL_RIGHT_HALF);
	outportb(WS_SOUND_OUT_CTRL_PORT,
		WS_SOUND_OUT_CTRL_SPEAKER_VOLUME_400 | WS_SOUND_OUT_CTRL_HEADPHONE_ENABLE | WS_SOUND_OUT_CTRL_SPEAKER_ENABLE);
	outportb(WS_SOUND_CH_CTRL_PORT, 0x00);

	for (uint8_t i = 0; i < 16; i++) {
		*((uint16_t*) (0x1FF0 + i)) = (i & 8) ? 0x00 : 0xFF;
	}

	ia16_critical({
		dhsecs = 0;

		ws_int_set_handler(WS_INT_HBL_TIMER, timer_int_handler);
		ws_int_set_handler(WS_INT_LINE_MATCH, line_int_handler);

		ws_timer_hblank_start_repeat(660);

		ws_int_enable(WS_INT_ENABLE_HBL_TIMER);
	});
}
