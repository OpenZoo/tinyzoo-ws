#include <wonderful.h>

	.arch	i186
	.code16
	.intel_syntax noprefix

	.section .text
	.global get_stat_id_at
	.global zoo_stats
	.global zoo_stat_count

get_stat_id_at:
	mov ah, dl
	mov cl, byte ptr [zoo_stat_count]
	inc cl
	xor ch, ch
	mov dl, cl
	mov bx, offset zoo_stats + 16
get_stat_id_at_loop:
	cmp ax, [bx]
	je get_stat_id_at_found
	add bx, 16
	loop get_stat_id_at_loop
	mov ax, 0xFFFF
	IA16_RET
get_stat_id_at_found:
	mov ah, 0xFF
	mov al, dl
	sub al, cl
	IA16_RET
