#ifndef __BANK_SWITCH_H__
#define __BANK_SWITCH_H__

#include <stdint.h>
#include <ws.h>

uint8_t zoo_get_ram_bank_count(void);
extern uint8_t _current_bank;

#define ZOO_SWITCH_ROM(bank) \
	{ \
		_current_bank = (bank); \
		ws_bank_rom0_set(bank); \
	}

#define ZOO_SWITCH_RAM(bank) \
	{ \
		ws_bank_ram_set(bank); \
	}

#define ZOO_ENABLE_RAM \
	{ }

#define ZOO_DISABLE_RAM \
	{ }

#define ZOO_INIT_ROM \
	{ \
		ZOO_SWITCH_ROM(0); \
	}

#endif
