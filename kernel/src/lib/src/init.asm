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
	.extern kernel_stack_address
	.extern pml4_address
	.extern kernel_stack_base

_start:

	movq %rsp, kernel_stack_base(%rip)

	#   initialize data
	call init
		#   initializates hardware and memory, set everything needed to migrate stack, paging structures

	#   initialize paging, migrate stack
	#movq pml4_address(%rip), %rax
	#movq %rax, %cr3
	#movq kernel_stack_address(%rip), %rsp
	#movq kernel_stack_address(%rip), %rbp


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
