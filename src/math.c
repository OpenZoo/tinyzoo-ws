#include <stdlib.h>
#include "math.h"

// helper functions

int8_t signum8(int8_t x) {
	if (x < 0) {
		return -1;
	} else if (x > 0) {
		return 1;
	} else {
		return 0;
	}
}

int8_t difference8(int8_t a, int8_t b) {
	return abs(a - b);
}

// RNG

uint32_t rand_seed;

void zsrand(uint16_t seed) {
	rand_seed = seed == 0 ? 1 : seed;
}

static void rand_step(void) {
	rand_seed = (rand_seed * 134775813) + 1;
}

int16_t zrand(void) {
	rand_step();
	return (rand_seed >> 16);
}

uint8_t zrand_mask8(uint8_t max) {
	rand_step();
	return (rand_seed >> 16) & max;
}

int16_t zrand_mod(int16_t max) {
	rand_step();
	return (rand_seed >> 16) % max;
}
