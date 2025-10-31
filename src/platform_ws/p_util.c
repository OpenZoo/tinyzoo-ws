#include <ws.h>

extern volatile uint8_t vbl_ticks;

void wait_vbl_done(void) {
	uint8_t vbl_ticks_last = vbl_ticks;

	while (vbl_ticks == vbl_ticks_last) {
		ia16_halt();
	}
}
