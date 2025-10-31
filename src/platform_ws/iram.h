#ifndef __IRAM_H__
#define __IRAM_H__

#include <wonderful.h>
#include <ws.h>

#ifdef IRAM_IMPLEMENTATION
#define IRAM_EXTERN
#else
#define IRAM_EXTERN extern
#endif

__attribute__((section(".iramx_1ff0"), used))
IRAM_EXTERN uint8_t _wave_data[0x10];
__attribute__((section(".iramx_2000"), used))
IRAM_EXTERN uint8_t _tile_data[0x1000];
__attribute__((section(".iramx_4000"), used))
IRAM_EXTERN uint8_t _tile2_data[0x3000];
__attribute__((section(".iramx_3000"), used))
IRAM_EXTERN uint8_t _screen_data[0x1000];

#undef IRAM_EXTERN

#endif /* __IRAM_H__ */
