#ifndef __BANK_SWITCH_H__
#define __BANK_SWITCH_H__

#include <stdint.h>
#include <ws.h>
#define WS_FAR __far

void zoo_switch_init(void);
void zoo_switch_rom(uint8_t bank);
uint8_t zoo_get_ram_bank_count(void);
extern uint8_t _current_bank;

#define ZOO_SWITCH_ROM(bank) zoo_switch_rom(bank);

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
		zoo_switch_init(); \
		ZOO_SWITCH_ROM(0); \
		ZOO_SWITCH_RAM(0); \
	}

#endif
