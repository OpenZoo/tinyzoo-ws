#ifndef __CHEAT_DETECT_H__
#define __CHEAT_DETECT_H__

#include <stdint.h>
#include <stdbool.h>
#include "config.h"
#include "gamevars.h"

#ifdef SHOW_CHEATS
void cheat_detect_update(void);
#endif

#endif
