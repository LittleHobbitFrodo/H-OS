##
##  init.asm
##      part of the CORE kernel belonging to the H-OS project
##

.section .text
	.extern init
	.extern kernel
;	.extern kernel_ptr
	.global halt
	.global hang
	.global _start
	.extern kernel_stack_virtual
	.extern kernel_stack_base
	#.extern interrupt_stack_virtual

_start:

	#   read stack base
	movq %rsp, kernel_stack_base(%rip)
		#   used later for stack re-initialization (paging.c)

	#   initialize data
	call init
		#   init function initializes memory, including gdt, set the kernel_ptr global variable

	#   re-initialize stack
	#movq kernel_stack_virtual(%rip), %rsp

	#   call kernel
	call kernel


hang:
	cli     #   disable interrupts
	.hang:
	hlt     #   waits for next interrupt (interrupts are disabled)
	jmp .hang   #   just to be sure

halt:
	hlt
	ret



#set_stack:
#	mov kernel_ptr(%rip), %rax
#    	movq %rax, %rsp
#    	movq $0, %rbp
#    ret
