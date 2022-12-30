#ifndef __OOP_H__
#define __OOP_H__

#include <stdint.h>
#include <stdbool.h>
#include "p_banking.h"
#include "config.h"
#include "gamevars.h"

// defines
#define OOP_DATA_POS_END 0xFFFF

#define OOP_LABEL_BOMBED 249
#define OOP_LABEL_TOUCH 250
#define OOP_LABEL_THUD 251
#define OOP_LABEL_ENERGIZE 252
#define OOP_LABEL_SHOT 253
#define OOP_LABEL_RESTART 254
#define OOP_LABEL_VOID 255

#define OOP_TARGET_EMPTY 251
#define OOP_TARGET_SELF 252
#define OOP_TARGET_OTHERS 253
#define OOP_TARGET_ALL 254
#define OOP_TARGET_VOID 255

// oop_flags.c (bank 0)
#define FLAG_ID_NONE 255

uint8_t world_get_flag_pos(uint8_t flag_id);
void world_set_flag(uint8_t flag_id);
void world_clear_flag(uint8_t flag_id);

// oop_bank2.c (bank 2)
bool find_tile_on_board(uint8_t *x, uint8_t *y, uint8_t element, uint8_t color);
void oop_place_tile(uint8_t x, uint8_t y, uint8_t element, uint8_t color);
uint16_t oop_dataofs_clone(uint16_t loc);
void oop_dataofs_free_if_unused(uint16_t loc, uint8_t except_id);

// oop.c (bank 0)
extern const char WS_FAR oop_object_name[];
extern const char WS_FAR oop_scroll_name[];
bool oop_send(uint8_t stat_id, bool respect_self_lock, uint8_t label_id, bool ignore_lock);
bool oop_send_target(uint8_t target_id, bool respect_self_lock, uint8_t label_id, bool ignore_lock);
bool oop_execute(uint8_t stat_id, const char __far* name);

#endif
