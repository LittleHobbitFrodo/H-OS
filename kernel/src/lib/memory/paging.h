//
//	memory/paging.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#define PAGE_COUNT 512
#define PAGE_SIZE 4096

#include "../integers.h"
#include "../vector.h"
//#include "../memory/aligned_ptr.h"

//typedef u64 page_entry;

static void page_init();

typedef struct page_entry {
	u64 present: 1;
	u64 write: 1;
	u64 user: 1;
	u64 caching: 1;
	u64 disable_caching: 1;
	u64 accessed: 1;
	u64 dirty: 1;
	u64 page_size: 1;
		//	change page size: pml4 = 512GB, pdpt = 1GB, pd = 2MB, pt = undefined
	u64 global: 1;
	u64 reserved: 3;
	u64 address: 40;
	//	address must be shifted before accessing/setting it
	u64 expansion: 8;
	u64 key: 3;		//	process individual protection key
	u64 execute_disable : 1;
} __attribute__((packed)) page_entry;

typedef page_entry page_table_t[512];

typedef struct virtual_address {
	u64 offset: 12;
	u64 pt: 9;
	u64 pdpt: 9;
	u64 pd: 9;
	u64 pml4: 9;
	u64 sign: 16;
} virtual_address;

union virtual_union {
	virtual_address virtual_address;
	u64 u64;
	void* voidptr;
} virtual_union;

union page_union {
	page_entry page_entry;
	u64 u64;
	void* voidptr;
} page_union;

#define VA_SHIFT 12
#define PAGE_SHIFT 12

#ifndef KERNEL_DEBUG
	__attribute__((always_inline)) inline
#endif
size_t va_index(void* address, u8 level) {
	return ((size_t)address >> (PAGE_COUNT + (level * 9)));
}
#ifndef KERNEL_DEBUG
	__attribute__((always_inline)) inline
#endif
size_t va_offset(void* address) {
	return (size_t)address & 0xFFF;
}

#ifndef KERNEL_DEBUG
	__attribute__((always_inline)) inline
#endif
void* page_address(page_entry ent) {
	return (void*)((size_t)ent.address << VA_SHIFT);
}

#ifndef KERNEL_DEBUG
	__attribute((always_inline)) inline
#endif
void* page_align(void* address) {
	return (void*)((size_t)address & ~0xFFF);
}

page_table_t* page_find();

void* physical(virtual_address address);
void* virtual_(void* physical);

void va_info(virtual_address address);


/*#define PAGE_ALIGN_DOWN(x) (((x) / PAGE_SIZE)*PAGE_SIZE)
//	each pdpt entry covers 1GB of RAM

[[maybe_unused]] static void *virtual_base = null;
[[maybe_unused]] static void *physical_base = null;

#define PAGE_COUNT 512
#define PAGE_SIZE 4096

#define VA_INDEX_L4 (0x1ffUL << 48)	//	48 = 12 + (9 * 4)
#define VA_INDEX_L3 (0xfffUL << 39)
#define VA_INDEX_L2 (0x1ffUL << 30)
#define VA_INDEX_L1 (0x1ffUL << 21)

static void page_init();

enum page_flags {
	//	structure of page_entry
	present = 1, //	page is present?
	write = (1 << 1), //	writable or only readable?
	user = (1 << 2), //	can user processes read it?
	write_through = (1 << 3), //	write-through caching?
	cache_disable = (1 << 4), //	disable caching?
	accessed = (1 << 5), //	accessed?
	ignored1 = (1 << 6), //	ignored bit
	//	in pml4, pdpt, pd if PS == 0
	dirty = (1 << 6), //	has been written to?
	//	only for PTs or larger (if PS == 1)
	//	else ignored
	PS = (1 << 7), //	page size
	//			pml4, pt -> ignored
	//			pdpt -> 1Gig
	//			pt ->
	global = (1 << 8), //	global? (used by more processes)
	//	only in PT entries
	available = (1 << 9 | 1 << 10 | 1 << 11), //	ignored by hardware
	address = (0xffffffffff000),
	available2 = (0x3fUL << 52),
	reserved_ = (0b111UL << 59), //	used by hardware (must be set to 0)

	//	bits 9,10,11 are ignored (can be used by OS)

	no_exec = (1UL << 63), //	disable executing

	address_shift = 12
};


typedef u64 page_entry;

[[maybe_unused]] page_entry *page_find();

[[maybe_unused]] void page_entry_info(page_entry ent);

typedef struct page_table {
	//	helps managing paging on heap
	page_entry *base; //	base address (on heap)
	size_t offset; //	offset (from block start) (before allocation: align)
	size_t size;
} page_table;


[[maybe_unused]] __attribute__((always_inline)) inline void *page_address(page_entry entry) {
	return (void *) (entry & 0xFFFFFFFFF000);
}

[[maybe_unused]] __attribute__((always_inline)) inline void page_set_address(page_entry *entry, void *ptr) {
	*entry &= ~0x000ffffffffff000;
	*entry |= ((size_t) ptr) & 0xFFFFFFFFF000;
}

[[maybe_unused]] __attribute__((always_inline)) inline u64 page_flags(page_entry entry) {
	return entry & (0x8ff | ((u64) 1 << (u64) 63));
}

[[maybe_unused]] __attribute__((always_inline)) inline void page_set_flags(page_entry *entry, u64 flags) {
	*entry &= ~(0x8ff | ((u64) 1 << (u64) 63));
	*entry |= flags & (0x8ff | ((u64) 1 << (u64) 63));
}

[[maybe_unused]] __attribute__((always_inline)) inline bool page_exec(page_entry entry) {
	return (entry & no_exec) != 0;
}

[[maybe_unused]] __attribute__((always_inline)) inline bool page_present(page_entry entry) {
	return (entry & present);
}

typedef void *virtual_addr;

[[maybe_unused]] __attribute__((always_inline)) inline size_t va_offset(virtual_addr addr) {
	return ((size_t) addr & 0xfff);
}

size_t va_index(virtual_addr addr, u8 level) {
	//	level starts at 0 (0 = pt)
	switch (level) {
		case 0: {
			return ((size_t)addr >> 12) & 0x1ff;
		}
		case 1: {
			return ((size_t)addr >> (12 + 9)) & 0x1ff;
		}
		case 2: {
			return ((size_t)addr >> (12 + 9 + 9)) & 0x1ff;
		}
		case 3: {
			return ((size_t)addr >> (12 + 9 + 9 + 9)) & 0x1ff;
		}
	}
	return 0;
	//return ((size_t) addr >> (12 + (level * 9))) & 0x1ff;
}

[[maybe_unused]] __attribute__((always_inline)) __attribute__((nonnull)) inline void va_set_index(virtual_addr *addr, u16 index, u8 level) {
	size_t shift = 12 + (level * 9);
	*((size_t *) addr) &= ~(0x1ffUL << shift);
	*((size_t *) addr) |= (index & 0x1ff) << shift;
}

[[maybe_unused]] __attribute__((always_inline)) __attribute__((nonnull)) inline void va_set_offset(virtual_addr *addr, u16 offset) {
	*((size_t *) addr) &= ~0xfff;
	*((size_t *) addr) |= offset & 0xfff;
}

[[maybe_unused]] extern void *physical(void *virt);

[[maybe_unused]] extern void *virtual_(void *phys);

[[maybe_unused]] static page_entry *pml4 = null;

void page_map_all();*/