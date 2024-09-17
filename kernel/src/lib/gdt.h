//
//	gdt.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifndef H_OS_LIB_GDT_H
	#define H_OS_LIB_GDT_H

	#define GDT_LIMIT_MAX 1048575
	#define GDT_LIMIT_MAX_GRANULAR 4294963200


	typedef struct gdt_entry {
		//	data structure of regular gdt entry (descriptor)

		u16 limit;
		u16 base0;
		u8 base1;
		u8 access;
		u8 flags;
		u8 base2;
	} __attribute__((packed)) gdt_entry;

	enum gdt_access_bits {
		ga_bits_accessed = 1,						//	set by the cpu if accessed
		ga_bits_read = ((u8)1 << 1),				//	code segment ->	allows reading
		ga_bits_write = ((u8)1 << 1),				//	data segment ->	allows writing
		ga_bits_direction = ((u8)1 << 2),			//	data segment ->	0 = grows up, else grows down
		ga_bits_conforming = ((u8)1 << 2),			//	sys segment  -> 0 = code can be executed only from this ring
		ga_bits_executable = ((u8)1 << 3),			//	allows code execution
		ga_bits_descriptor_type = ((u8)1 << 4),		//	defines descriptor type
			//	0 = system (tss) segment
			//	1 = data/code segment
		ga_bits_privilege_level = ((u8)0x60),		//	indices ring
		ga_bits_present = ((u8)1 << 7),				//	makes segment valid
	} gdt_access_bits;

	enum gdt_access_flags {
		ga_flags_code_read = ((u8)1 << 1),
		ga_flags_data_write = ((u8)1 << 1),

		ga_flags_data_grow_up = 0,
		ga_flags_data_grow_down = ((u8)1 << 2),

		ga_flags_tss_conform = ((u8)1 << 2),

		ga_flags_type_data = ((u8)1 << 4),
		ga_flags_type_system = 0

	} gdt_access_flags;

	enum gdt_flags_bits {
		gf_bits_long_mode = ((u8)1 << 1),			//	enables long mode (64bit) code segment
		gf_bits_size = ((u8)1 << 2),				//	fallback execution size
			//	0 = 16bit code
			//	1 = 32bit code
		gf_bits_granular = ((u8)1 << 3),
			//	makes limit valued in 4kb pages instead of bytes

	} gdt_flags_bits;

	enum gdt_flags_flags {
		gf_flags_long_mode = ((u8)1 << 1),
		gf_flags_protected_mode = ((u8)1 << 2),
		gf_flags_real_mode = 0,
	} gdt_flags_flags;

	__attribute__((always_inline, nonnull(1))) inline void* gdt_base(gdt_entry* ent) {
		return (void*)(((size_t)ent->base0) | ((size_t)ent->base1 << 16) | ((size_t)ent->base2 << 24));
	}

	__attribute__((always_inline, nonnull(1))) inline u32 gdt_limit(gdt_entry* ent) {
		return ((u32)ent->limit | (((u32)ent->flags & 0x0f) << 16)) * (((ent->flags & gf_bits_granular) == 0)? 1 : 4096);
	}

	__attribute__((always_inline, nonnull(1))) inline u8 gdt_flags(gdt_entry* ent) {
		//	granular bit is set by gdt_set_limit()
		return ent->flags & 0xf0;
	}

	__attribute__((always_inline, nonnull(1))) inline void gdt_set_base(gdt_entry* ent, void* base) {
		ent->base0 = (size_t)base & 0xffff;
		ent->base1 = ((size_t)base >> 16) & 0xff;
		ent->base2 = ((size_t)base >> 24) & 0xff;
	}

	__attribute__((always_inline, nonnull(1))) inline void gdt_set_limit(gdt_entry* ent, u32 limit) {
		if (limit > GDT_LIMIT_MAX) {
			//	enable granular
			limit /= 4096;
			ent->flags |= gf_bits_granular;
		} else {
			//	disable granular flag
			ent->flags &= ~gf_bits_granular;
		}
		ent->limit = limit & 0xffff;
		ent->flags &= 0xf0;
		ent->flags |= (limit >> 16) & 0x0f;
	}

	__attribute__((always_inline, nonnull(1))) inline void gdt_set_flags(gdt_entry* ent, u8 flags) {
		ent->flags &= (0x0f | gf_bits_granular);
		ent->flags |= flags & (0xf0 & ~gf_bits_granular);
	}





	typedef struct gdt_tss_entry {
		//	gdt entry for system (tss) segments

		u16 limit;
		u16 base0;
		u8 base1;
		u8 access;
		u8 flags;
		u8 base2;
		u32 base3;
		u32 reserved;	//	unused
	} __attribute__((__packed__)) gdt_tss_entry;

	enum gdt_tss_access_bits {
		gta_bits_type = 0x07,		//	type of system segment
			//	(marked only types available in long mode)
			//	0x2		LDT
			//	0x9		64bit TSS (available)
			//	0xb		64bit TSS (busy)

	} gdt_tss_access_bits;

	enum gdt_tss_type {
		gt_type_ldt = 0x2,
		gt_type_tss_available = 0x9,
		gt_type_tss_busy = 0xb
	} gdt_tss_type;

	__attribute__((always_inline, nonnull(1))) inline void* gdt_tss_base(gdt_tss_entry* ent) {
		return (void*)(((size_t)ent->base0) | ((size_t)ent->base1 << 16) | ((size_t)ent->base2 << 24) | ((size_t)ent->base3 << 32));
	}

	__attribute__((always_inline, nonnull(1))) inline u32 gdt_tss_limit(gdt_tss_entry* ent) {
		return (u32)ent->limit | ((u32)(ent->flags & 0x0f) << 16);
	}

	__attribute__((always_inline, nonnull(1))) inline u8 gdt_tss_flags(gdt_tss_entry* ent) {
		return ent->flags & 0xf0;
	}

	__attribute__((always_inline, nonnull(1))) inline void gdt_tss_set_base(gdt_tss_entry* ent, void* base) {
		ent->base0 = (u16)((size_t)base & 0xffff);
		ent->base1 = (u8)(((size_t)base >> 16) & 0xff);
		ent->base2 = (u8)(((size_t)base >> 24) & 0xff);
		ent->base3 = (u32)((size_t)base >> 32);
	}

	__attribute__((always_inline)) inline void gdt_tss_set_limit(gdt_tss_entry* ent, u32 limit) {
		ent->limit = (u16)(limit & 0xffff);
		ent->flags &= 0xf0;
		ent->flags |= (limit >> 16) & 0x0f;
	}

	__attribute__((always_inline, nonnull(1))) inline void gdt_tss_set_flags(gdt_tss_entry* ent, u8 flags) {
		ent->flags &= 0x0f | gf_bits_granular;
		ent->flags |= flags & 0xf0;
	}


	__attribute__((always_inline, nonnull(1))) inline void gdt_null(gdt_entry* ent) {
		*((u64*)ent) = 0;
	}

	typedef struct gdt_ptr {
		u16 size;
		void* entries;
	} __attribute__((packed)) gdt_ptr;


	static struct gdt {
		gdt_entry _null;
		gdt_entry code;
		gdt_entry heap;

		gdt_tss_entry tss;
	} __attribute__((packed, aligned(8))) gdt;

	static gdt_ptr gdt_pointer;

	static void gdt_init();

	static void gdt_update();

	extern void gdt_load(gdt_ptr*);

#endif