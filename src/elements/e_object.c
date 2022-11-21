#include <stdbool.h>
#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../math.h"
#include "../oop.h"
#include "../sound_consts.h"
#include "../timer.h"

uint8_t ElementObjectDraw(uint8_t x, uint8_t y) {
	uint8_t stat_id = get_stat_id_at(x, y);
	return ZOO_STAT(stat_id).p1;
}

void ElementObjectTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);

	if (stat->data_pos != OOP_DATA_POS_END) {
		if (oop_execute(stat_id, oop_object_name)) {
#ifdef BUGFIX_DIEMOVE_OBJECT
			return;
#endif
		}
	}

	if (stat->step_x != 0 || stat->step_y != 0) {
		uint8_t nx = stat->x + stat->step_x;
		uint8_t ny = stat->y + stat->step_y;

		if (ZOO_TILE_READBOUNDS(nx, ny)
		&& (zoo_element_defs_flags[ZOO_TILE(nx, ny).element] & ELEMENT_WALKABLE)) {
			move_stat(stat_id, nx, ny);
		} else {
			oop_send(stat_id, true, OOP_LABEL_THUD, false);
		}
	}
}

void ElementObjectTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	uint8_t stat_id = get_stat_id_at(x, y);
	oop_send(stat_id, true, OOP_LABEL_TOUCH, false);
}
