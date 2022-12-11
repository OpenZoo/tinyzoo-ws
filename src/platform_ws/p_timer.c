#include <ws.h>
#include "game_transition.h"
#include "input.h"
#include "renderer.h"
#include "timer.h"

extern uint16_t dhsecs;
extern uint8_t vbl_ticks;

extern void __attribute__((interrupt)) timer_int_handler(void);

void __attribute__((interrupt)) vblank_int_handler(void) {
	vbl_ticks++;

	input_update_vbl();

#ifdef FEAT_BOARD_TRANSITIONS
	game_transition_step();
#endif

	ws_hwint_ack(HWINT_VBLANK);
}

void vbl_timer_init(void) {
        cpu_irq_disable();

        ws_hwint_set_handler(HWINT_IDX_VBLANK, vblank_int_handler);
        ws_hwint_enable(HWINT_VBLANK);

        cpu_irq_enable();
}

void timer_init(void) {
	cpu_irq_disable();

	dhsecs = 0;

        ws_hwint_set_handler(HWINT_IDX_HBLANK_TIMER, timer_int_handler);

        outportw(IO_HBLANK_TIMER, 660);
        outportb(IO_TIMER_CTRL, HBLANK_TIMER_ENABLE | HBLANK_TIMER_REPEAT);

        ws_hwint_enable(HWINT_HBLANK_TIMER);

	cpu_irq_enable();
}
