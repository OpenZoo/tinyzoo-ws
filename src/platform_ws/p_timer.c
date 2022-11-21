#include <ws.h>
#include "game_transition.h"
#include "input.h"
#include "renderer.h"
#include "timer.h"

extern uint16_t dhsecs;
extern uint8_t vbl_ticks;

void __attribute__ ((interrupt, save_all)) timer_int_handler(void) {
	dhsecs += 11;
	outportb(IO_INT_ACK, INTR_MASK_HBLANK_TIMER);
}

void __attribute__ ((interrupt, save_all)) vblank_int_handler(void) {
	vbl_ticks++;
	input_update_vbl();
#ifdef FEAT_BOARD_TRANSITIONS
	game_transition_step();
#endif
	outportb(IO_INT_ACK, INTR_MASK_VBLANK);
}

void vbl_timer_init(void) {
	cpu_irq_disable();

        outportb(IO_INT_VECTOR, 0x08);
        outportb(IO_INT_ENABLE, INTR_ENABLE_VBLANK);

        *((uint16_t*) 0x0038) = FP_OFF(vblank_int_handler);
        *((uint16_t*) 0x003A) = FP_SEG(vblank_int_handler);

	cpu_irq_enable();
}

void timer_init(void) {
	cpu_irq_disable();

	dhsecs = 0;

        outportb(IO_INT_ENABLE, INTR_ENABLE_VBLANK | INTR_ENABLE_HBLANK_TIMER);
        outportw(IO_HBLANK_TIMER, 660);
        outportb(IO_TIMER_CTRL, HBLANK_TIMER_ENABLE | HBLANK_TIMER_REPEAT);

        *((uint16_t*) 0x003C) = FP_OFF(timer_int_handler);
        *((uint16_t*) 0x003E) = FP_SEG(timer_int_handler);

	cpu_irq_enable();
}
