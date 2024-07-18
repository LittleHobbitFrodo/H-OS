##
##  init.asm
##      part of the H-OS project
##

.section .bss
STACK_START:
resb 65536
STACK_END:

.section .text
	.extern init
	.extern kernel
	.extern kernel_ptr
	.extern set_rsp


	.global halt
	.global _start
	.global set_cs

_start:

	#   my last ettempt to re-initialize stack
	movq %rsp, STACK_END

	#   initialize data
	call init
		#   init function initializes memory, including gdt, set the kernel_ptr global variable



	#   my previous attempt (kernel ptr is defined as extern "C" in kernel/src/lib/tss.h and modified in kernel/src/lib/src/memory.ccp, line 550)
	#   change stack location
	#mov kernel_ptr(%rip), %rax
	#mov %rax, %rsp
	#mov %rsp, [%rax]

	#   perform kernel
	call kernel

halt:
	cli
	.hang:
	hlt
	jmp .hang
