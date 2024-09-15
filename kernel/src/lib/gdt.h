//
//	gdt.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifndef H_OS_LIB_GDT_H
	#define H_OS_LIB_GDT_H

	typedef u16 segment;

	typedef struct gdt_entry {

		//	allocate in writable virtual memory
			//	access bit -> cpu will page fault

		u16 limit;
		u16 base0;
		u8 base1;
		u8 access;
		u8 flags;
		u8 base2;
		u32 base3;
		u32 reserved;	//	unused


		//	limit:	20 bit
		//		16 bits in limit
		//		4 bites in flags (first 4 bits of flags)

		//	base:	32 bit
		//		16 bits in base1
		//		8 bits in base2
		//		8 bits in base3
		//		32 bits in base4
	} __attribute__((__packed__)) gdt_entry;

	#define GDT_LIMIT_MAX 1048575
#define GDT_LIMIT_MAX_GRANULAR 4294963200

	typedef struct gdt_ptr {
		gdt_entry* entries;
		u16 size;
	} __attribute__((packed)) gdt_ptr;

	enum gdt_access_bits {
		gdt_bits_present = ((u8)1 << 7),		//	7th bit			//	makes entry valid
		gdt_bits_privilege = (u8)0x60,			//	6-5th bits		//	privilege level
			//		0 = supervisor (kernel)
			//		1 = firmware?
			//		2 = userspace
		gdt_bits_segment_type = ((u8)1 << 4),		//	4th bit			//	segment type
			//		0 = system segment (task state segment)
			//		1 = code or data segment
		gdt_bits_executable = ((u8)1 << 3),			//	3rd bit			//	enables execution in segment
		gdt_bits_direction = ((u8)1 << 2),			//	second bit
		gdt_bits_conform = ((u8)1 << 2),				//	second bit
			//	direction and conform
			//		data segment	->	0 = grows up
			//						->	1 = grows down

			//		code segments	->	0 = only executed from privilege level set in DPL (privilege)
			//						->	1 = can jump to equal or lower privilege level?
		gdt_bits_code_read = ((u8)1 << 1),				//	first bit
		gdt_bits_data_write = ((u8)1 << 1),
			//	code segment		->	allows reading (writing is never allowed)
			//	data segment		->	allows writing (reading is always enabled)
		gdt_bits_accessed = 1,						//	zero bit		//	indicates if segment was accessed (set by CPU)

	} gdt_access_bits;

	enum gdt_access_flags {
		gdt_data_segment = ((u8)1 << 4),
		gdt_code_segment = ((u8)1 << 4),
		gdt_system_segment = 0,
		gdt_data_grows_up = 0,
		gdt_data_grows_down = ((u8)1 << 2),
		gdt_system_nonconforming = 0,
		gdt_system_conforming = ((u8)1 << 2),
		gdt_code_readable = ((u8)1 << 1),
		gdt_data_writable = ((u8)1 << 1),
	} gdt_access_flags;

	enum gdt_flags_bits {
		granular = ((u8)1 <<  (3+4)),			//	3rd bit in flags
			//	0 = limit is taken as bytes
			//	1 = limit is takes as pages (4kb blocks)
		protected_mode = ((u8)1 << (2+4)),
			//	0 = 16bit protected mode segment
			//	1 = 32bit protected mode segment
		long_mode = ((u8)1 << (1+4)),
			//	0 = 32bit or 16bit (defined by protected_mode flag)
			//	1 = 64bit long mode segment
		//	first bit is reserved (not used)

	} gdt_flags_bits;

	__attribute__((always_inline, nonnull(1))) inline void gdt_entry_null(gdt_entry* entry) {
		((size_t*)entry)[0] = 0;
		((size_t*)entry)[1] = 0;
	}


	__attribute__((always_inline, nonnull(1))) inline u32 gdt_limit(const gdt_entry* ent) {
		return ((u32)ent->limit | ((u32)ent->flags << 16)) * (((ent->flags & granular) != 0)? 4096 : 1);
	}

	__attribute__((always_inline, nonnull(1))) inline void* gdt_base(const gdt_entry* ent) {
		return (void*)(((size_t)ent->base0) | ((size_t)ent->base1 << 16) | ((size_t)ent->base2 << 24) | ((size_t)ent->base3 << 32));
	}

	__attribute__((always_inline, nonnull(1))) inline u8 gdt_access(const gdt_entry* ent) {
		return ent->access;
	}

	__attribute__((always_inline, nonnull(1))) inline u8 gdt_flags(const gdt_entry* ent) {
		return ent->flags & 0xf0;
	}


	__attribute__((always_inline, nonnull(1))) inline void gdt_set_limit(gdt_entry* ent, u32 limit) {
		if (limit > GDT_LIMIT_MAX) {
			//	enable granular
			limit /= 4096;
			ent->flags |= granular;
		} else {
			//	disable granular flag
			ent->flags &= ~granular;
		}
		ent->limit = limit & 0xffff;
		ent->flags &= 0x0f;
		ent->flags |= (limit >> 16) & 0xf;
	}

	__attribute((always_inline, nonnull(1))) inline void gdt_set_base(gdt_entry* ent, void* base) {
		ent->base0 = (size_t)base & 0xffff;
		ent->base1 = ((size_t)base >> 16) & 0xff;
		ent->base2 = ((size_t)base >> 24) & 0xff;
		ent->base3 = ((size_t)base >> 32) & 0xffffffff;
	}

	__attribute__((always_inline, nonnull(1))) inline void gdt_set_flags(gdt_entry* ent, u8 flags) {
		ent->flags &= 0x0f;
		ent->flags |= flags & 0xf0;
	}

	__attribute__((always_inline, nonnull(1))) inline void gdt_set_access(gdt_entry* ent, u8 access) {
		ent->access &= 0x0f;
		ent->access |= access & 0xf0;
	}

	void gdt_entry_info(const gdt_entry* ent, bool simple);

	static void gdt_init();

	static aligned_ptr gdt_pointer;
	static aligned_vector gdt;

	//static void gdt_update(void*);
		//	update gdt vector allocated position (used in reallocation)

	//static void gdt_load();
		//	lgdt

#endif