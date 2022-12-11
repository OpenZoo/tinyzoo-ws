#pragma once

#include <stdint.h>

extern uint16_t *screen1_table;

#define USE_COLOR_RENDERER ws_system_is_color()
// #define USE_COLOR_RENDERER 0

// palettes:
// 4, 5, 6, 7, 12, 13, 14, 15 - text mode emulation
#define PAL_MENU 0
#define PAL_SIDEBAR0 8
#define PAL_SIDEBAR1 9
#define PAL_SIDEBAR2 10
#define PAL_MESSAGE 11

