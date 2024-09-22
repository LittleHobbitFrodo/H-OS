#
#   exceptions.asm
#       part of the CORE kernel belonging to the H-OS project
#

#NOTE:  index is always pushed and poped

.intel_syntax noprefix

.extern handle_exception

.global isr_ex_divide_by_zero
.global isr_ex_debug
.global isr_ex_nonmaskable_interrupt
.global isr_ex_breakpoint
.global isr_ex_overflow
.global isr_ex_out_of_range
.global isr_ex_invalid_opcode
.global isr_ex_device_not_available
.global isr_ex_double_fault
#   9 is unused
.global isr_ex_invalid_tss
.global isr_ex_segment_not_present
.global isr_ex_stack_segment_fault
.global isr_ex_general_protection
.global isr_ex_page_fault
#   15 is unused
.global isr_ex_floating_point
.global isr_ex_alignment
.global isr_ex_machine_check
.global isr_ex_simd
.global isr_ex_virtualization
.global isr_ex_control_protection
.global isr_ex_hypervisor_injection
.global isr_ex_vmm_communication
.global isr_ex_security

.section .text

isr_ex_divide_by_zero:
	push 0  #  error index (no)
	push 0  #  exception type
	jmp isr_exception_wrapper

isr_ex_debug:
	push 0
	push 1
	jmp isr_exception_wrapper

isr_ex_nonmaskable_interrupt:
	push 0
	push 2
	jmp isr_exception_wrapper

isr_ex_breakpoint:
	push 0
	push 3
	jmp isr_exception_wrapper

isr_ex_overflow:
	push 0
	push 4
	jmp isr_exception_wrapper

isr_ex_out_of_range:
	push 0
	push 5
	jmp isr_exception_wrapper

isr_ex_invalid_opcode:
	push 0
	push 6
	jmp isr_exception_wrapper

isr_ex_device_not_available:
	push 0
	push 7
	jmp isr_exception_wrapper

isr_ex_double_fault:
	#   0 is pushed
	push 8
	jmp isr_exception_wrapper

isr_ex_invalid_tss:

	push 10
	jmp isr_exception_wrapper

isr_ex_segment_not_present:

	push 11
	jmp isr_exception_wrapper

isr_ex_stack_segment_fault:

	push 12
	jmp isr_exception_wrapper

isr_ex_general_protection:

	push 13
	jmp isr_exception_wrapper

isr_ex_page_fault:

	push 14
	jmp isr_exception_wrapper

isr_ex_floating_point:
	push 0
	push 16
	jmp isr_exception_wrapper

isr_ex_alignment:

	push 17
	jmp isr_exception_wrapper

isr_ex_machine_check:
	push 0
	push 18
	jmp isr_exception_wrapper

isr_ex_simd:
	push 0
	push 19
	jmp isr_exception_wrapper

isr_ex_virtualization:
	push 0
	push 20
	jmp isr_exception_wrapper

isr_ex_control_protection:

	push 21
	jmp isr_exception_wrapper

isr_ex_hypervisor_injection:
	push 0
	push 28
	jmp isr_exception_wrapper

isr_ex_vmm_communication:

	push 29
	jmp isr_exception_wrapper

isr_ex_security:

	push 30
	jmp isr_exception_wrapper









isr_exception_wrapper:
	push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    mov rax, cr2
    push rax
    cld
    mov rdi, rsp
    call handle_exception
    add rsp, 8
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    add rsp, 0x10
    iretq