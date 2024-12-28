//
//	memory/heap/table-heap/structures.h
//		part of the CORE kernel belonging to the H-OS project
//

//	each allocator is 2mb in size (uses one sized page entry)

#pragma once
#include "../../../integers.h"

typedef struct page_ptr {
	//	4 bytes in size (the smaller the better)
	u32 index:		9;		//	table index in the allocator
	u32 count:		9;		//	table count
	u32 used:		1;
	u32 _reserved:	13;
} __attribute__((packed)) page_ptr;

typedef any_page_table page_heap_t[512] __attribute__((aligned(0x200000)));		//	exactly 2mb of memory, aligned to 2mb

typedef struct table_allocator_t {

	//	this structure is fokun huge wt

	page_heap_t* data;
	u64 bitmap[64];	//	each bit for each table used/free
		//	if multiple tables allocated (bit for each table is marked as used too)
	struct page_ptr ptr[512];

	heap_metadata meta;		//	metadata for *data

	struct table_allocator_t* next;

} table_allocator_t;






/*typedef struct page_ptr {
	u64* table;
	u8 count;		//	table count (heap contains max 256 tables)
	u8 used;
} page_ptr;

typedef page_ptr page_header[256] __attribute__((aligned(4096)));
#define PAGE_HEAP_SIZE (sizeof(page_ptr) * 256)


typedef struct page_heap {
	//	stored in per-core multipurpose heap
	page_header* header;
	heap_metadata meta;
	struct page_heap* next;
	u32 bitmap[8];		//	each bit represent used or unused entry
	bool free;		//	indicates if at least one of the pointers is free
} page_heap;

struct page_allocator_t;

void page_header_construct(page_header* self, page_heap* heap);
void page_heap_construct(page_heap* self, struct page_allocator_t* alloc);

vector_with_different_allocator_type(page_heaps, page_heap, page_heap_vec_t, page_heap_construct, to_be_optimized, struct page_allocator_t)

typedef struct page_allocator_t {
	page_heap_vec_t heaps;

	u64* (*alloc)(struct page_allocator_t* alloc, size_t physical, u32 count);
	//page_table_t* (*realloc)(page_table_t* specific, u32 size);
	void (*free)(struct page_allocator_t* alloc, u64* table);

} page_allocator_t;*/

//void page_allocator_init(page_allocator_t* self);



/*typedef struct page_segment {
	page_table_t* entries;		//	virtual address of entries
	u32 count;
	volatile bool used;
} page_segment;

__attribute__((always_inline, nonnull(1)))
inline void page_seg_construct(page_segment* seg) {
	seg->entries = null;
	seg->count = 0;
	seg->used = false;
}

volatile_vector_type(page_segs, page_segment, page_segment_vec_t, page_seg_construct, to_be_optimized);

typedef struct page_region {

	page_segment_vec_t segments;
	heap_metadata meta;

} page_region;

__attribute__((always_inline, nonnull(1)))
static inline void page_region_construct(page_region* reg) {
	page_segs_construct(&reg->segments, 0, &heap.global);
	reg->meta.virtual.start = (reg->meta.virtual.end = 0);
	reg->meta.physical.start = (reg->meta.physical.end = 0);
}

__attribute__((always_inline, nonnull(1)))
static inline void page_region_destruct(page_region* reg) {
	page_segs_destruct(&reg->segments);
}*/

//vector_type(page_regs, page_region, page_region_vec_t, page_region_construct, page_region_destruct);
