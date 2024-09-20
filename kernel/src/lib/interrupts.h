//
//	interrupts.h
//		part of the CORE kernel belonging to the H-OS project
//

///NOTE:	interrupt handlers are compiled separatedly


#pragma once

#ifndef H_OS_LIB_INTERRUPTS_H
	#define H_OS_LIB_INTERRUPTS_H

	typedef struct int_stack_frame {
		u64 rip;
		u64 cs;
		u64 rflags;
		u64 rsp;
		u16 ss;
	} __attribute__((packed)) int_stack_frame;

	typedef struct idt_entry {

		u16 off1;
		u16 cs;
		u8 ist;			//	2bits -> ist, else is reserved
		u8 flags;
		u16 off2;
		u32 off3;
		u32 reserved2;

	} __attribute__((packed)) idt_entry;

	#define IDT_SIZE 256
	#define IDT_USED_SIZE 31

	typedef idt_entry idt_t[IDT_SIZE];

	typedef struct idt_ptr {
		u16 size;
		u64 address;
	} __attribute__((packed)) idt_ptr;

	enum idt_bits {
		idt_bits_gate_type = 0b111,
		idt_bits_privilege = 0x60,
		idt_bits_present = (u8)1 << 7
	} idt_bits;

	enum idt_gate_types {
		idt_gate_type_interrupt = 0xe,
		idt_gate_type_trap = 0xf
	} idt_gate_types;


	__attribute__((always_inline, nonnull(1))) inline size_t idt_address(const idt_entry* ent) {
		return ((size_t)ent->off1) | ((size_t)ent->off2 << 16) | ((size_t)ent->off3 << 32);
	}

	__attribute__((always_inline, nonnull(1))) inline u8 idt_ist(const idt_entry* ent) {
		return (u8)ent->ist & 0b11;
	}

	__attribute__((always_inline, nonnull(1))) inline void idt_set_address(idt_entry* ent, void (*address)(void)) {
		ent->off1 = (size_t)address & 0xffff;
		ent->off2 = ((size_t)address >> 16) & 0xffff;
		ent->off3 = ((size_t)address >> 32) & 0xffffffff;
	}

	__attribute__((always_inline, nonnull(1))) inline void idt_null(idt_entry* ent) {
		((u64*)ent)[0] = 0;
		((u64*)ent)[1] = 0;
	}

	__attribute__((aligned(16))) static idt_t idt;
	__attribute__((aligned(16))) static idt_ptr idt_pointer;

	static void idt_init();

#endif