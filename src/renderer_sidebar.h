#ifndef __RENDERER_SIDEBAR_H__
#define __RENDERER_SIDEBAR_H__

#include <stdint.h>
#include <stdbool.h>
#include "renderer.h"

void sidebar_set_message_color(uint8_t color);

void sidebar_draw_empty(uint8_t x, uint8_t width);
void sidebar_draw_panel(uint8_t x, uint8_t chr, uint8_t col, int16_t value, bool wide);
void sidebar_draw_keys(uint8_t x, uint8_t value);

void sidebar_show_message(const char __far* line1, uint8_t bank1, const char __far* line2, uint8_t bank2, const char __far* line3, uint8_t bank3);
void sidebar_hide_message(void);

#endif /* __RENDERER_SIDEBAR_H__ */
