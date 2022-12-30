#!/usr/bin/python3
#
# Copyright (c) 2021, 2022 Adrian Siekierka
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
# SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
# RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
# CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
# CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

from pathlib import Path
import os, re, sys

format = "length_prefixed_string"
if len(sys.argv) > 4:
	format = sys.argv[4]

with (
	open(sys.argv[1], "r") as fp_i,
	open(sys.argv[2], "w") as fp_c,
	open(sys.argv[3], "w") as fp_h,
):
	hdr_define = '__%s__' % re.sub(r'[^a-zA-Z0-9]', '_', Path(sys.argv[3]).name).upper()

	print("// Auto-generated file. Please do not edit directly.\n", file = fp_c)
	print("#include <stdint.h>\n", file = fp_c)
	print("#include \"p_banking.h\"\n", file = fp_c)
	print("// Auto-generated file. Please do not edit directly.\n", file = fp_h)
	print(f"#ifndef {hdr_define}\n#define {hdr_define}\n", file = fp_h)
	print("#include <stdint.h>\n", file = fp_h)
	print("#include \"p_banking.h\"\n", file = fp_h)

	for i in fp_i:
		i: str = i.strip()
		if "=" in i:
			kv = i.split("=", maxsplit=1)
			print("extern const char WS_FAR %s[];" % kv[0], file = fp_h)
			print("const char WS_FAR %s[] = {" % kv[0], file = fp_c)
			vb = kv[1].encode("CP437")
			if format == "length_prefixed_string":
				s = "\t%d," % len(vb)
			elif format == "text_window_message":
				s = "\t%d, %d," % (0, len(vb))
			else:
				raise Exception(f"Unknown format: {format}")
			for vbi in vb:
				s += " %d," % vbi
			print("%s\n};" % s, file = fp_c)

	print("#endif", file = fp_h);
