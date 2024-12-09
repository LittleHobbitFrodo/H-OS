//
//	memory/paging.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#define PAGE_COUNT 512
#define PAGE_SIZE 4096

#include "../integers.h"

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
	u64 exec_disable : 1;
} __attribute__((packed)) page_entry;

enum page_bits {
	page_bit_present = 1,
	page_bit_write = 1 << 1,
	page_bit_user = 1 << 2,
	page_bit_caching = 1 << 3,
	page_bit_disable_caching = 1 << 4,
	page_bit_accessed = 1 << 5,
	page_bit_dirty = 1 << 6,
	page_bit_page_size = 1 << 7,
	page_bit_global = 1 << 8,
	//	reserved
	page_bit_address = (size_t)0xffffffffff << 12,	//	40 bits
	page_bit_expansion = (size_t)0xff << 52,
	page_bit_key = (size_t)0b111 << 60,
	page_bit_exec_disable = (size_t)1 << 63
};

typedef page_entry page_table_t[512];

#include "./heap/page-heap/structures.h"

typedef struct virtual_address {
	u64 offset: 12;
	u64 pt: 9;
	u64 pd: 9;
	u64 pdpt: 9;
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

__attribute__((always_inline))
static inline size_t va_index(void* address, u8 level) {
	return ((size_t)address >> (PAGE_COUNT + (level * 9)));
}

__attribute__((always_inline))
static inline size_t va_offset(void* address) {
	return (size_t)address & 0xFFF;
}

__attribute__((always_inline))
static inline void* page_address(page_entry ent) {
	return (void*)((size_t)ent.address << VA_SHIFT);
}

__attribute__((always_inline))
static inline void* page_align(void* address) {
	return (void*)((size_t)address & ~0xFFF);
}

__attribute__((nonnull(1, 2)))
static inline void page_cpy(page_table_t* src, page_table_t* dest, size_t table_count) {
	/*__m512i* src_ = (__m512i*)src;
	__m512i* dest_ = (__m512i*)dest;
	const size_t max = table_count * (PAGE_SIZE / 64);
	__m512i chunk
	for (size_t i = 0; i < max; i++) {
		 chunk = _mm512_load_si512(src_ + i);
		_mm512_store_si512(dest_ + i, chunk);
	}*/
	const size_t max = (sizeof(page_table_t)/sizeof(size_t)) * table_count;
	for (size_t i = 0; i < max; i++) {
		((size_t*)dest)[i] = ((size_t*)src)[i];
	}
}

typedef struct pages_t {
	page_table_t* pml4;		//	virtual address of the pml4 table

	size_t hhdm;		//	virtual base address of hhdm
		//	0 => any memory map region (<= 4GB)

	struct kernel {
		void* physical;
		void* virtual;
	} kernel;

	struct {
		page_region_vec_t regions;
		size_t size;		//	size of allocated space
		size_t physical;	//	physical base (heap initialization purposes)
		void* virtual;
	} heap;

	struct {
		//	specific page tables for system resources
		struct {
			size_t physical;
			void* virtual;
			__attribute__((aligned(4096))) page_table_t page;
		} pdpt;

		struct {
			void* virtual;
			size_t physical;
			__attribute__((aligned(4096))) page_table_t page;
			//	pd layer (recursive mapped)
		} random;


	} system;

} pages_t;

static pages_t pages = {0};

void* physical(void* address);

void va_info(void* a);

void* page_map(void* physical, size_t page_count, u64 perms);

void page_flush();

void* page_quick_map(void* physical, page_entry** ent);
	//	maps one page for one-time use

#define page_quick_unmap(pageptr) pageptr->address = 0;

page_entry* page_find_empty_pdpt();
	//	find empty pdpt entry


#include "./heap/page-heap/page-heap.h"