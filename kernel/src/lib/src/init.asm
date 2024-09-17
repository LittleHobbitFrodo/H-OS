##
##  init.asm
##      part of the CORE kernel belonging to the H-OS project
##

.intel_syntax noprefix

.extern KERNEL_STACK_SIZE

.section .text
	.extern init
	.extern kernel
	.global halt
	.global hang
	.global _start
	.global gdt_load
	.global tss_update
	#.global reload_segments

_start:
	mov r8, KERNEL_STACK_END[rip]
	mov rsp, r8

	cli

	call init

	call kernel

hang:
	cli
	.hang:
	hlt
	jmp .hang

halt:
	hlt
	ret

gdt_load:
	lgdt [rdi]
	ret

tss_update:
	mov ax, 0x20
	ltr ax
	ret