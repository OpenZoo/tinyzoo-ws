#include <ws.h>
#include "bank_switch.h"

static const uint8_t __far bank_counts[] = {0, 0, 1, 2, 4, 8};
uint8_t _current_bank;

uint8_t zoo_get_ram_bank_count(void) {
	return bank_counts[* ((uint8_t __far*) MK_FP(0xFFFF, 0x000B))];
}
