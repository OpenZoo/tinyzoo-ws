#include <wonderful-asm.h>

	.arch	i186
	.code16
	.intel_syntax noprefix

	.section .text
	.global dhsecs
	.global line_int_handler
	.global timer_int_handler

	.align 2
#include "./sound_freqs.inc"

// #define USE_24000HZ_DRUMS

line_int_handler:
	push ax
	push bx

	// load current drum position
	mov bx, ss:[drum_position]
	cmp bx, ss:[drum_position_end]
	je line_int_handler_stop
#ifdef USE_24000HZ_DRUMS
	add bx, 2
#else
	inc bx
#endif
	mov al, cs:[bx]

	// play current drum frequency
	out 0x89, al
	mov ss:[drum_position], bx

	// set next line
	in al, 0x02
	inc al
	cmp al, 159
	jb line_int_handler_next_line_fits
	sub al, 159
line_int_handler_next_line_fits:
	out 0x03, al

	// acknowledge interrupt
	mov al, 0x10
	out 0xB6, al

	pop bx
	pop ax	
	iret

line_int_handler_stop:
	in al, 0xB2
	and al, 0xEF
	out 0xB2, al

	xor al, al
	out 0x90, al

	// acknowledge interrupt
	mov al, 0x10
	out 0xB6, al

	pop bx
	pop ax	
	iret

timer_int_handler:
	push ax

	push ds
	xor ax, ax
	mov ds, ax

	// increment timer
	add word ptr [dhsecs], 11

	// play audio
	cmp byte ptr [sound_enabled], 0
	je timer_int_handler_no_sound

	cmp byte ptr [sound_is_playing], 0
	je timer_int_handler_end

	dec byte ptr [sound_duration_counter]
	jg timer_int_handler_end

	// generate audible pop
	in al, 0x90
	test al, 0x08
	jz timer_int_handler_no_pop

	xor ax, ax
	out 0x90, al
	push cx
	mov cx, 100
timer_int_handler_pop:
	loop timer_int_handler_pop
	pop cx

timer_int_handler_no_pop:
	// next note
	mov al, [sound_buffer_pos]
	cmp al, [sound_buffer_len]
	jae timer_int_handler_no_sound
	add byte ptr [sound_buffer_pos], 2

	push bx
	mov bx, offset sound_buffer
	// ah = 0
	add bx, ax
	mov ax, [bx] // al = note, ah = duration

	// set duration
	mov [sound_duration_counter], ah

timer_int_handler_check_note:
	sub al, 16
	jl timer_int_handler_check_drum
	cmp al, 96
	jge timer_int_handler_check_drum

	mov ah, 0
	shl ax, 1
	add ax, offset sound_freqs
	mov bx, ax
	mov ax, cs:[bx]
	out 0x86, ax

	pop bx
	jmp timer_int_handler_emit_sound

timer_int_handler_check_drum:
	sub al, 224
	jl timer_int_handler_checks_done
	cmp al, 10
	jge timer_int_handler_checks_done

	mov ah, 0
#ifdef USE_24000HZ_DRUMS
	shl ax, 7
#else
	shl ax, 6
#endif
	add ax, ax
	add ax, ax // ax *= 192
	add ax, offset sound_drums
	mov bx, ax
	mov [drum_position], ax

	// are we color?
	in al, 0xA0
	test al, 0x02
	jnz timer_int_handler_drum_dma

#ifdef USE_24000HZ_DRUMS
	add ax, 384
#else
	add ax, 192
#endif
	mov [drum_position_end], ax
	mov al, cs:[bx]
	out 0x89, al

	// enable channel 2 voice
	mov al, 0x22
	out 0x90, al

	// set next line
	in al, 0x02
	inc al
	cmp al, 159
	jb timer_int_handler_next_line_fits
	sub al, 159
timer_int_handler_next_line_fits:
	out 0x03, al

	// enable line interrupt
	in al, 0xB2
	or al, 0x10
	out 0xB2, al

	pop bx
	jmp timer_int_handler_end

timer_int_handler_drum_dma:
	// configure sound DMA source
	mov ax, cs
	ror ax, 12
	out 0x4C, ax
	and ax, 0xFFF0
	add ax, [drum_position]
	out 0x4A, ax
	xor ax, ax
	out 0x50, ax
#ifdef USE_24000HZ_DRUMS
	mov ax, 384
#else
	mov al, 192
#endif
	out 0x4E, ax

	// configure channel 2 voice
	mov al, 0x80
	out 0x89, al
	mov al, 0x22
	out 0x90, al

	// start DMA
#ifdef USE_24000HZ_DRUMS
	mov al, 0x83
#else
	mov al, 0x82
#endif
	out 0x52, al

timer_int_handler_checks_done:
	pop bx
	jmp timer_int_handler_end
	
timer_int_handler_emit_sound:
	// enable channel 4
	mov al, 0x08
	out 0x90, al
	jmp timer_int_handler_end

timer_int_handler_no_sound:
	xor ax, ax
	mov byte ptr [sound_is_playing], al
	out 0x90, al

timer_int_handler_end:
	// acknowledge interrupt
	mov al, 0x80
	out 0xB6, al

	pop ds
	pop ax
	iret

	.section .bss
drum_position:
	.word 0
drum_position_end:
	.word 0
