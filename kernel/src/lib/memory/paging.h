//
//	memory/paging.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../integers.h"
#include "../memory/aligned_ptr.h"
#include "../vector.h"


#define PAGE_ALIGN_DOWN(x) (((x) / PAGE_SIZE)*PAGE_SIZE)
//	each pdpt entry covers 1GB of RAM

#define PAGE_COUNT 512
#define PAGE_SIZE 4096

#define VA_INDEX_L4 (0x1ffUL << 48)	//	48 = 12 + (9 * 4)
#define VA_INDEX_L3 (0xfffUL << 39)
#define VA_INDEX_L2 (0x1ffUL << 30)
#define VA_INDEX_L1 (0x1ffUL << 21)

[[maybe_unused]] static void page_init();

enum page_flags {
	//	structure of page_entry
	pf_present = 1, //	page is present?
	pf_write = (1 << 1), //	writable or only readable?
	pf_user = (1 << 2), //	can user processes read it?
	pf_write_through = (1 << 3), //	write-through caching?
	pf_cache_disable = (1 << 4), //	disable caching?
	pf_accessed = (1 << 5), //	accessed?
	pf_ignored1 = (1 << 6), //	ignored bit
	//	in pml4, pdpt, pd if PS == 0
	pf_dirty = (1 << 6), //	has been written to?
	//	only for PTs or larger (if PS == 1)
	//	else ignored
	pf_PS = (1 << 7), //	page size
	//			pml4, pt -> ignored
	//			pdpt -> 1Gig
	//			pt ->
	pf_global = (1 << 8), //	global? (used by more processes)
	//	only in PT entries
	pf_available = (1 << 9 | 1 << 10 | 1 << 11), //	ignored by hardware
	pf_address = (0xffffffffff000),
	pf_available2 = (0x3fUL << 52),
	pf_reserved_ = (0b111UL << 59), //	used by hardware (must be set to 0)

	//	bits 9,10,11 are ignored (can be used by OS)

	pf_no_exec = (1UL << 63), //	disable executing

	pf_address_shift = 12
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
	return (entry & pf_no_exec) != 0;
}

[[maybe_unused]] __attribute__((always_inline)) inline bool page_present(page_entry entry) {
	return (entry & pf_present);
}

typedef void *virtual_addr;

[[maybe_unused]] __attribute__((always_inline)) inline size_t va_offset(virtual_addr addr) {
	return ((size_t) addr & 0xfff);
}

[[maybe_unused]] __attribute__((always_inline)) inline size_t va_index(virtual_addr addr, u8 level) {
	//	level starts at 0 (0 = pt)
	return ((size_t) addr >> (12 + (level * 9))) & 0x1ff;
}

#define VA_INDEX_PML4 3
#define VA_INDEX_PDPT 2
#define VA_INDEX_PD 1
#define VA_INDEX_PT 0

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

typedef struct pages_t {
	aligned_ptr pages;
	size_t used;
	size_t allocated;
} pages_t;

typedef struct pages_base_t {

	struct base {
		void* virtual;
		void* physical;
	} base;

	aligned_ptr pml4;
	struct kernel {
		aligned_ptr pdpt;

		struct pd {
			aligned_ptr ptr;
			size_t size;
		} pd;

		struct pt {
			aligned_ptr ptr;
			size_t size;
		} pt;

		pages_t heap;
		pages_t stack;
		pages_t kernel;

	} kernel;
} pages_base_t;

static pages_base_t pages;

