#ifndef __MATH_H__
#define __MATH_H__

#include "config.h"

#include <stdint.h>
#include <stdlib.h>

int8_t signum8(int8_t x);
int8_t difference8(int8_t a, int8_t b);

#define zoo_modu16_8(a, b) ((uint8_t) (((uint16_t) (a)) % ((uint8_t) (b))))
#define zoo_mods16_8(a, b) ((uint8_t) (((int16_t) (a)) % ((uint8_t) (b))))

int16_t zrand(void);
uint8_t zrand_mask8(uint8_t max);
int16_t zrand_mod(int16_t max);
#define zrand_np1 zrand_mod
void zsrand(uint16_t seed);

#define ZRAND2() zrand_mask8(1)
#define ZRAND3() zrand_np1(3)
#define ZRAND4() zrand_mask8(3)
#define ZRAND7() zrand_mod(7)
#define ZRAND9() zrand_np1(9)
#define ZRAND10() zrand_mod(10)
#define ZRAND17() zrand_np1(17)

#endif /* __MATH_H__ */
