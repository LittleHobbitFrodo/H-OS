;;
;;  init.asm
;;      part of the CORE kernel belonging to the H-OS project
;;

section .text
	extern init
	extern kernel
	extern shutdown
	extern kernel_stack_ptr

	global halt
	global hang
	global _start
	global update_segment_registers

_start:
	cli

	;   initialize kernel stack
	mov rsp, [rel kernel_stack_ptr]

	;   initialize machine
	call init   ;   returns pointer to kernel stack

	call kernel

	call shutdown

	.hlt:
	hlt
	jmp .hlt

hang:
	cli
	hlt
	jmp hang
	;   endless loop

halt:
	hlt
	ret

update_segment_registers:
	push rax

	mov ax, 16
	mov ds, ax
	mov ss, ax

	mov ax, 0
	mov es, ax
	mov fs, ax
	mov gs, ax
	pop rax
	ret
