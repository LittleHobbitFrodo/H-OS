//
//	memory/paging.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#define PAGE_COUNT 512
#define PAGE_SIZE 4096

#include "../integers.h"
#include "../vector.h"
//#include <immintrin.h>

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

__attribute__((always_inline, nonnull(1, 2)))
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

page_table_t* page_find();

void* physical(virtual_address address);
void* virtual_(void* physical);

void va_info(virtual_address address);
