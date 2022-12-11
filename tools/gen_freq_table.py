#!/usr/bin/python3
#
# Copyright (c) 2020, 2021, 2022 Adrian Siekierka
#
# Based on a reconstruction of code from ZZT,
# Copyright 1991 Epic MegaGames, used with permission.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import math, sys

APPLY_ZZT_ROUNDING = False
APPLY_PIT_ROUNDING = False

FREQS = [0] * 16 * 6
DRUM_FREQS = [
                [ 1, 3200,  0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0],
                [14, 1100,  1200,   1300,   1400,   1500,   1600,   1700,   1800,   1900,   2000,   2100,   2200,   2300,   2400,   0],
                [14, 4800,  4800,   8000,   1600,   4800,   4800,   8000,   1600,   4800,   4800,   8000,   1600,   4800,   4800,   8000],
                [14, 0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0],
                [14, 500,   2556,   1929,   3776,   3386,   4517,   1385,   1103,   4895,   3396,   874,    1616,   5124,   606,    0],
                [14, 1600,  1514,   1600,   821,    1600,   1715,   1600,   911,    1600,   1968,   1600,   1490,   1600,   1722,   1600],
                [14, 2200,  1760,   1760,   1320,   2640,   880,    2200,   1760,   1760,   1320,   2640,   880,    2200,   1760,   0],
                [14, 688,   676,    664,    652,    640,    628,    616,    604,    592,    580,    568,    556,    544,    532,    0],
                [14, 1207,  1224,   1163,   1127,   1159,   1236,   1269,   1314,   1127,   1224,   1320,   1332,   1257,   1327,   0],
                [14, 378,   331,    316,    230,    224,    384,    480,    320,    358,    412,    376,    621,    554,    426,    0]
]

# print("const uint16_t __far sound_freqs[%d] = {" % len(FREQS))
print("sound_freqs:")

freqC1 = 32.0
ln2 = math.log(2)
noteStep = math.exp(ln2 / 12.0)

for octave in range(1, 6+1):
	noteBase = math.exp(octave * ln2) * freqC1
	noteBase = round(noteBase) # 64, 128, 256, 512, 1024, 2048
	for note in range(0, 12):
		if APPLY_ZZT_ROUNDING:
			FREQS[(octave - 1) * 16 + note] = int(noteBase)
		else:
			FREQS[(octave - 1) * 16 + note] = noteBase
		noteBase = noteBase * noteStep

print(FREQS, file=sys.stderr)

for i in FREQS:
	if i == 0:
		i = 0x0000
	else:
		if APPLY_PIT_ROUNDING:
			# PIT transformation
			i = math.floor(1193182.0 / i)
			# PIT de-transformation
			i = 1193182.0 / i
		i = round(2048 - (96000.0 / i))
	print("\t.word %d" % i)

print("sound_drums:")

# 192 bytes x 10 drums = 1920 bytes...
audio_freq = 12000
pos_samples_fixed = 0
step_size = 256

for drum in DRUM_FREQS:
	len = drum[0]
	data = drum[1:]
	for i in range(0, 16):
		freq = data[i] if i < len else 0
		if freq <= 0:
			for isub in range(0, int(audio_freq / 1000)):
				print("\t.byte 0")
		else:
			freq_samples_fixed = audio_freq * 512 / freq
			for isub in range(0, int(audio_freq / 1000)):
				pos_local = pos_samples_fixed % (freq_samples_fixed * 2)
				if pos_local < freq_samples_fixed:
					if pos_local < step_size:
						trans_val = pos_local
						print("\t.byte %d" % trans_val)
					else:
						print("\t.byte 255")
				else:
					mid_pos = pos_local - freq_samples_fixed
					if mid_pos < step_size:
						trans_val = 255 - mid_pos
						print("\t.byte %d" % trans_val)
					else:
						print("\t.byte 0")
				pos_samples_fixed += step_size

# print("};")
