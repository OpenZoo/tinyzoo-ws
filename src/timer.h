#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdbool.h>
#include <stdint.h>

extern bool sound_block_queueing;

void sound_init(void);
void sound_clear_queue(void);
void sound_queue(int8_t priority, const uint8_t __far* data);

void vbl_timer_init(void);
void timer_init(void);
uint16_t timer_hsecs(void);
bool timer_has_time_elapsed(uint16_t *ctr, uint16_t duration);
void sound_reset(void);

#endif /* __TIMER_H__ */
