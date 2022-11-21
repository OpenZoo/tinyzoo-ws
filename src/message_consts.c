#include <stdint.h>
#include "../res/message_consts.inc"

const char __far* msg_key_pickup_get(uint8_t x) {
	switch (x & 0x07) {
		case 0x00: return msg_key_pickup_08;
		case 0x01: return msg_key_pickup_09;
		case 0x02: return msg_key_pickup_0a;
		case 0x03: return msg_key_pickup_0b;
		case 0x04: return msg_key_pickup_0c;
		case 0x05: return msg_key_pickup_0d;
		case 0x06: return msg_key_pickup_0e;
		case 0x07: return msg_key_pickup_0f;
	}
}

const char __far* msg_door_open_get(uint8_t x) {
	switch (x & 0x07) {
		case 0x00: return msg_door_open_08;
		case 0x01: return msg_door_open_09;
		case 0x02: return msg_door_open_0a;
		case 0x03: return msg_door_open_0b;
		case 0x04: return msg_door_open_0c;
		case 0x05: return msg_door_open_0d;
		case 0x06: return msg_door_open_0e;
		case 0x07: return msg_door_open_0f;
	}
}
