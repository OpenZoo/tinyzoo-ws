#ifndef __BOARD_MANAGER_H__
#define __BOARD_MANAGER_H__

#include <stdint.h>
#include <stdbool.h>
#include "sram_alloc.h"

// on bank 0 - do not use
void load_world_rom(uint8_t offset);
void load_board_rom(uint8_t offset);
void load_board_data_rom(uint8_t bank, const uint8_t __far* data);

// on bank 2 -
void load_world(uint8_t offset, bool new_game);
void load_board(uint8_t offset);
bool save_world(uint8_t offset);
bool save_board(uint8_t offset);
bool clear_saved_board(uint8_t offset);

#endif /* __BOARD_MANAGER_H__ */
