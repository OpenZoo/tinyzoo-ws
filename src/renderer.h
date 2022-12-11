#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

#define RENDER_MODE_PLAYFIELD 0
#define RENDER_MODE_TITLE 1
#define RENDER_MODE_TXTWIND 2
#define RENDER_MODE_MENU 3
#define RENDER_MODE_NONE 4

extern uint8_t renderer_id;
extern uint8_t renderer_mode;
extern uint8_t renderer_scrolling; // if 1, do not remove colors
extern uint8_t draw_offset_x;
extern uint8_t draw_offset_y;

void text_init(uint8_t mode);
void text_reinit(uint8_t mode);

#if defined(__WONDERFUL_WSWAN__)
static inline void text_sync_hblank_safe(void) {}
static inline void text_undraw(uint8_t x, uint8_t y) {}
static inline void text_free_line(uint8_t y) {}
#else
void text_sync_hblank_safe(void);
void text_undraw(uint8_t x, uint8_t y);
void text_free_line(uint8_t y);
#endif
void text_draw(uint8_t x, uint8_t y, uint8_t chr, uint8_t col);
void text_scroll(int8_t dx, int8_t dy);
void text_update(void);

#endif /* __RENDERER_H__ */
