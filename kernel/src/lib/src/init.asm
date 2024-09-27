;;
;;  init.asm
;;      part of the CORE kernel belonging to the H-OS project
;;

section .text
	extern init
	extern kernel
	global halt
	global hang
	global _start
	global update_segment_registers
	global long_jump

_start:
	cli

	;   initialize machine
	call init
	;   move kernel stack
	mov rsp, rax

	call kernel

hang:
	cli
	hlt
	jmp hang
	;   endless loop

halt:
	hlt
	ret

;update_segment_registers:
;	mov ax, 0x10    ; 2
;	mov ds, ax
;	mov es, ax
;	mov fs, ax
;	mov gs, ax
;	mov ss, ax
;	;pop rax
;	ret

