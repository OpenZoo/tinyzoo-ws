#include <wonderful.h>

	.arch	i186
	.code16
	.intel_syntax noprefix

	.section .text
	.global zsrand
	.global zrand
	.global zrand_mask8
	.global zrand_mod

zsrand:
	cmp ax, 0
	je _zsrand2
	mov [rand_seed_lo], ax
	xor ax, ax
	mov [rand_seed_hi], ax
	IA16_RET
_zsrand2:
	mov word ptr [rand_seed_lo], 1
	mov [rand_seed_hi], ax
	IA16_RET

zrand_step:
	mov ax, 0x8c0d
	mul word ptr [rand_seed_lo] // dx:ax = 
	mov cx, dx
	mov bx, ax // cx:bx = ((const_low + const_high) * seed_low)

	mov ax, 0x8405
	mul word ptr [rand_seed_hi] // dx:ax = (const_low * seed_high)

	add ax, bx
	adc dx, cx
	inc ax
	adc dx, 0

	mov [rand_seed_lo], ax
	mov [rand_seed_hi], dx
	ret

zrand:
	call zrand_step
	mov ax, dx
	IA16_RET

zrand_mask8:
	push ax
	call zrand_step
	pop ax
	and al, dl
	IA16_RET

zrand_mod:
	push ax
	call zrand_step
	pop cx
	mov ax, dx
	xor dx, dx
	idiv cx // 0:dx / cx = remainder in dx
	mov ax, dx
	IA16_RET

	.section .bss
rand_seed_lo:
	.word 0
rand_seed_hi:
	.word 0

