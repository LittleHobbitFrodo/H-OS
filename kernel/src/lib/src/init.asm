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
	.extern physical

_start:

	#   initialize data
	call init
		#   init function initializes memory, including gdt, set the kernel_ptr global variable

	#   initialize paging
	#movq kernel_stack_address(%rip), %cr3
	#   set registers
	#movq %rip,

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
