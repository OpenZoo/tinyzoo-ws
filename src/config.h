#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "platform_config.h"

#define MAX_BOARD 101

#define FEAT_BLACK_KEYS
#define FEAT_BOARD_TRANSITIONS
#define BUGFIX_BLINKWALL_OFFSET
// #define BUGFIX_DIE_UNDER
#define BUGFIX_DIEMOVE_SCROLL
#define BUGFIX_DIEMOVE_OBJECT
#define BUGFIX_DIEMOVE_OOP_EXEC
#define BUGFIX_DIEMOVE_MESSAGE
// #define BUGFIX_KOOPO_BUG
// #define BUGFIX_PUT_RANGE

// adds 81 bytes, performance untested
// #define OPT_UNROLL_LINE_DRAW

// saves 124 (!!!) bytes of RAM; however, loses 124 bytes of bank0 ROM
// this makes some parts of the board edge immutable
#define OPT_BOARD_EDGE_IMMUTABLE

// #define RESET_SAVE_ON_START
// #define SHOW_TITLE
// #define HACK_HIDE_STATUSBAR
// #define HACK_PLAYER_INVINCIBLE
#define SHOW_CHEATS

#define MAX_SCROLL_DISTANCE_BEFORE_REDRAW 15

#define TORCH_DX 8
// #define TORCH_DY 5
#define TORCH_DY 4
#define TORCH_DIST_SQR 50
#define TORCH_DURATION 200
#define MAX_FLAG 10
#define MAX_STAT 150
#define MAX_ELEMENT 53
#define BOARD_WIDTH 60
#define BOARD_HEIGHT 25
#define MAX_OOP_INSTRUCTION_COUNT 32

#define MAX_DATA_OFS_SIZE 600
#define MAX_SOUND_BUFFER_SIZE 254
#define MAX_TEXT_WINDOW_LINES 512

// #include "platform_debug.h"
// #define DEBUG_PRINTFS
// #define DEBUG_PRINTFS_OOP_EXEC
// #define DEBUG_SRAM_WRITES
// #define DEBUG_SOUND_QUEUE

#endif /* __CONFIG_H__ */
