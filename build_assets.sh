#!/bin/sh
echo "[ Compiling 8x8 font ]"
python3 tools/font2raw.py res/font_default.png 8 8 a res/font_default.bin
python3 tools/bin2c.py --bank 3 res/font_default.c res/font_default.h res/font_default.bin
echo "[ Compiling 4x8 font ]"
python3 tools/font2raw.py res/font_small.png 4 8 b res/font_small.bin
python3 tools/bin2c.py --bank 3 res/font_small.c res/font_small.h res/font_small.bin
echo "[ Compiling message constants ]"
python3 tools/gen_message_consts.py res/message_consts.properties res/message_consts.inc res/message_consts.h
python3 tools/gen_message_consts.py res/menu_entry_consts.properties res/menu_entry_consts.inc res/menu_entry_consts.h text_window_message
echo "[ Compiling text messages ]"
python3 tools/gen_static_txtwind_data.py res/license.txt res/txtwind_license.inc txtwind_license_data
