#include <string.h>
#include "config.h"
#include "renderer.h"
#include "timer.h"

uint8_t sound_buffer[MAX_SOUND_BUFFER_SIZE];
volatile uint8_t sound_buffer_pos;
volatile uint8_t sound_buffer_len;
volatile uint8_t sound_duration_counter;
bool sound_enabled;
bool sound_block_queueing;
bool sound_is_playing;
int8_t sound_current_priority;

void sound_queue(int8_t priority, const uint8_t __far* data) {
	if (!sound_block_queueing) {
		if (!sound_is_playing || (((priority >= sound_current_priority) && (sound_current_priority != -1)) || (priority == -1))) {
			uint8_t data_len = data[0];
#ifdef DEBUG_SOUND_QUEUE
			EMU_printf("queueing sound: %d prio %d addr %02X:%04X",
				(uint16_t) data_len, (int16_t) priority, (uint16_t) _current_bank, (uint16_t) data);
#endif
			if (priority >= 0 || !sound_is_playing) {
				sound_current_priority = priority;
				text_sync_hblank_safe();
				{
					cpu_irq_disable();
					sound_duration_counter = 1;
					sound_buffer_len = data_len;
					sound_buffer_pos = 0;
					sound_buffer[0] = data[1];
					sound_buffer[1] = data[2];
					cpu_irq_enable();
				};
				if (data_len > 2) {
					memcpy(sound_buffer + 2, data + 3, data_len - 2);
				}
			} else {
				uint8_t pos = sound_buffer_pos;
				_nmemmove(sound_buffer, sound_buffer + pos, sound_buffer_len - pos);
				text_sync_hblank_safe();
				{
					cpu_irq_disable();
					sound_buffer_len -= pos;
					sound_buffer_pos = 0;
					cpu_irq_enable();
				};
				if ((sound_buffer_len + data_len) <= MAX_SOUND_BUFFER_SIZE) {
					memcpy(sound_buffer + sound_buffer_len, data + 1, data_len);
					sound_buffer_len += data_len;
				}
			}
			sound_is_playing = true;
		}
	}
}

void sound_update(void) {
	// no-op
}

void sound_init(void) {
	sound_enabled = true;
	sound_block_queueing = false;
	sound_buffer_len = 0;
	sound_is_playing = false;
}

void sound_clear_queue(void) {
	sound_buffer_len = 0;
	sound_is_playing = false;

	// TODO: reset sound hardware
}
