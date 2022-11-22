#ifndef __SRAM_ALLOC_H__
#define __SRAM_ALLOC_H__

#include <stdint.h>
#include <stdbool.h>
#include "config.h"
#include "gamevars.h"

typedef struct __attribute__((packed)) {
	uint8_t bank;
	int16_t position;
} sram_ptr_t;

#define SRAM_DATA_POS 4
#define SRAM_BOARD_PTRS_POS (SRAM_DATA_POS + 2 + sizeof(zoo_world_info_t))
#define SRAM_TEXT_WINDOW_POS (SRAM_BOARD_PTRS_POS + (MAX_BOARD * sizeof(sram_ptr_t)))

typedef struct {
	uint8_t magic[4];
	uint8_t flags;
	uint8_t world_id;
	zoo_world_info_t world_info;
	sram_ptr_t board_pointers[MAX_BOARD];
	far_ptr_t text_window_lines[MAX_TEXT_WINDOW_LINES];
} sram_header_t;

// Pre-setup required: ENABLE_RAM, switch bank
void sram_add_ptr(sram_ptr_t *ptr, uint16_t val);
void sram_sub_ptr(sram_ptr_t *ptr, uint16_t val);

#if defined(DEBUG_SRAM_WRITES) && !defined(SRAM_ALLOC_INTERNAL)
uint8_t sram_read8_debug(sram_ptr_t *ptr);
void sram_write8_debug(sram_ptr_t *ptr, uint8_t value);
void sram_read_debug(sram_ptr_t *ptr, void *data, uint16_t len);
void sram_write_debug(sram_ptr_t *ptr, const void __far* data, uint16_t len);
#endif

#if defined(DEBUG_SRAM_WRITES) && !defined(SRAM_ALLOC_INTERNAL)
#define sram_read8 sram_read8_debug
#define sram_write8 sram_write8_debug
#else
uint8_t sram_read8(sram_ptr_t *ptr);
void sram_write8(sram_ptr_t *ptr, uint8_t value);
#endif

#if defined(SRAM_ALLOC_INTERNAL)
void sram_read(sram_ptr_t *ptr, uint8_t *data, uint16_t len);
void sram_write(sram_ptr_t *ptr, const uint8_t __far* data, uint16_t len);
#else
#if defined(DEBUG_SRAM_WRITES)
#define sram_read sram_read_debug
#define sram_write sram_write_debug
#else
void sram_read(sram_ptr_t *ptr, void *data, uint16_t len);
void sram_write(sram_ptr_t *ptr, const void __far* data, uint16_t len);
#endif
#endif

bool sram_alloc(uint16_t len, sram_ptr_t *ptr);
void sram_free(sram_ptr_t *ptr);
void sram_toggle_write(void);

// No pre-setup required.
void sram_init(bool force);

#endif
