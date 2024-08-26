##
##  init.asm
##      part of the H-OS project
##

.section .bss
STACK_START:
.lcomm stack, 65536
STACK_END:

.section .text
	.extern init
	.extern kernel
	.extern kernel_ptr
	.extern set_rsp


	.global halt
	.global _start
	.global set_cs

set_stack:
	mov kernel_ptr(%rip), %rax
    	movq %rax, %rsp
    	movq $0, %rbp
    ret

_start:

	#   my last ettempt to re-initialize stack
	#movq %rsp, STACK_END
	#movq STACK_END, %rbp
	#movq STACK_END, %rsp
	#movq $STACK_END, %rbp

	#   initialize data
	call init
		#   init function initializes memory, including gdt, set the kernel_ptr global variable
	#call set_stack


	#   my previous attempt (kernel ptr is defined as extern "C" in kernel/src/lib/tss.h and modified in kernel/src/lib/src/memory.ccp, line 550)
	#   change stack location
	#mov %rax, %rsp
	#mov %rsp, [%rax]

	#   perform kernel
	call kernel

halt:
	cli
	.hang:
	hlt
	jmp .hang
