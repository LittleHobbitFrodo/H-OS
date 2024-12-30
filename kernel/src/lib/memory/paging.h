//
//	memory/paging.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#define PAGE_COUNT 512
#define PAGE_SIZE 4096

#include "../integers.h"

static void paging_init();

#define VA_SHIFT 12
#define PAGE_SHIFT 12
#define SIZED_PAGE_SHIFT 20


union virtual_address {
	u64 u64;
	void* voidptr;
	struct {
		u64 offset: 12;
		u64 pt: 9;
		u64 pd: 9;
		u64 pdpt: 9;
		u64 pml4: 9;
		u64 sign: 16;
	} virtual_address;
} virtual_address;

typedef struct page_table_entry {
	//	pt layer entries
		//	no PAT bit (sized pages (pml4, ...) has it
		//	aligned to 4096 (4kb)

	u64 present:	1;
	u64 write:	1;
	u64 user:	1;		//	1 = can be accessed by user -> key
	u64 cache_type:	1;
	u64 cache_disable:	1;
	u64 accessed:	1;
	u64 dirty:	1;		//	1 = been written to
	u64 attributes:	1;		//	some caching stuff (maybe)
	u64 global:	1;		//	1 = globally accessed (writing into cr3 will not flush it)
	u64 avail:	3;
	u64 address:	40;
	u64 available:	7;
	u64 key:	4;		//	if user = 1
	u64 exec_disable:	1;

} __attribute__((packed)) page_table_entry;

typedef struct unsized_page_entry {
	//	non-sized entries
		//	pml4, pdpt, table
		//	aligned to 4096 (4kb)

	u64 present:	1;
	u64 write:	1;		//	1 = can write
	u64 user:	1;		//	1 = user accessed page
	u64 cache_type:	1;
	u64 cache_disable:	1;
	u64 accessed:	1;
	u64 avail:	6;
	u64 address:	40;
	u64 available:	11;
	u64 exec_disable:	1;
} __attribute__((packed)) unsized_page_entry;

typedef struct sized_page_entry {
	//	pml4, pdpt, table with page_size bit set
		//	not pt layer
		//	aligned to 2mb

	u64 present:	1;
	u64 write:	1;
	u64 user:	1;
	u64 cache_type:	1;
	u64 cache_disable:	1;
	u64 accessed:	1;
	u64 dirty:	1;
	u64 page_size:	1;
	u64 global:	1;
	u64 avail:	3;		//	12th bit
	u64 attributes:	1;		//	PAT
	u64 available:	7;
	u64 address:	32;
	u64 _avail:	7;
	u64 key:	4;
	u64 exec_disable:	1;

} __attribute__((packed)) sized_page_entry;

union any_page_entry {
	page_table_entry pt;
	unsized_page_entry unsized;
	sized_page_entry sized;
	u64 u64;
};

typedef unsized_page_entry unsized_page_table[512];	//	pml4, pdpt, table (page_set bit clear)
typedef sized_page_entry sized_page_table[512];	//	pml4, pdpt, table (page_size bit set)
typedef page_table_entry page_table[512];		//	pt layer
typedef union any_page_entry any_page_table[512];



#define unsized_page_set_address(page, addr) (page).address = (addr) >> PAGE_SHIFT
#define sized_page_set_address(page, addr) (page).address = (addr) >> SIZED_PAGE_SHIFT
#define unsized_page_address(page) (page.address << PAGE_SHIFT)
#define sized_page_address(page) (page.address << SIZED_PAGE_SHIFT)


#include "./heap/table-heap/structures.h"
	//	NOTE:	needs structures in this header and this header needs page heap structures



__attribute__((always_inline))
static inline size_t va_index(void* address, u8 level) {
	return ((size_t)address >> (PAGE_COUNT + (level * 9)));
}

__attribute__((always_inline))
static inline size_t va_offset(void* address) {
	return (size_t)address & 0xFFF;
}

__attribute__((always_inline))
static inline void* page_align(void* address) {
	return (void*)((size_t)address & ~0xFFF);
}

__attribute__((nonnull(1, 2)))
static inline void page_cpy(const u64* src, u64* dest, size_t table_count) {
	const size_t max = (sizeof(any_page_table)/sizeof(size_t)) * table_count;
	for (size_t i = 0; i < max; i++, src++, dest++) {
		*dest = *src;
	}


	/*__m512i* src_ = (__m512i*)src;
	__m512i* dest_ = (__m512i*)dest;
	const size_t max = table_count * (PAGE_SIZE / 64);
	__m512i chunk
	for (size_t i = 0; i < max; i++) {
		 chunk = _mm512_load_si512(src_ + i);
		_mm512_store_si512(dest_ + i, chunk);
	}*/
}

typedef struct pages_t {
	unsized_page_table* pml4;		//	virtual address of the pml4 table

	size_t hhdm;		//	virtual base address of hhdm
		//	0 => any memory map region (<= 4GB)

	struct kernel {
		void* physical;
		void* virtual;
	} kernel;

	struct {
		table_allocator_t global;


		size_t size;		//	size of allocated space
		struct {
			//	initialization purposes only
			size_t physical;    //	physical base
			void *virtual;
			any_page_table* table;
		} init;
	} heap;

	struct {
		//	specific page tables for system resources
		struct {
			size_t physical;
			void* virtual;
			__attribute__((aligned(4096))) unsized_page_table table;
		} pdpt;

		struct {
			void* virtual;
			size_t physical;
			__attribute__((aligned(4096))) sized_page_table table;
			//	2mb entries
		} quick;

		struct {
			void* virtual;
			size_t physical;
			__attribute__((aligned(4096))) sized_page_table table;
		} heap;

		struct {
			void* virtual;
			size_t physical;
			__attribute__((aligned(4096))) sized_page_table table;
			//	other page tables will be allocated in page heap
		} table_heap;


	} system;

} pages_t;

static pages_t pages = {0};

void* physical(void* address);

void va_info(void* a);

void* page_map(void* physical, size_t page_count, u64 perms);

void page_flush();

[[deprecated]] void* page_quick_map(size_t physical, sized_page_entry** ent);
	//	maps one page for one-time use
		//	needs to be fixed (crashes)

#define page_quick_unmap(pageptr) pageptr->address = 0; you should not use that :(

unsized_page_entry* page_find_empty_pdpt();
	//	find empty pdpt entry


#include "./heap/table-heap/table-heap.h"