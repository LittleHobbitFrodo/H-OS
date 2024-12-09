//
//	memory/heap/page-heap/structures.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../../../integers.h"
#include "../../../vector/volatile-vector.h"

typedef struct page_segment {
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
	struct {
		//	physical addresses
		size_t start;
		size_t end;
	} physical;
	struct {
		struct page_table_t* start;
		struct page_table_t* end;
	} virtual;
} page_region;

__attribute__((always_inline, nonnull(1)))
static inline void page_region_construct(page_region* reg) {
	page_segs_construct(&reg->segments, 0);
	reg->virtual.start = (reg->virtual.end = 0);
	reg->physical.start = (reg->physical.end = 0);
}

__attribute__((always_inline, nonnull(1)))
static inline void page_region_destruct(page_region* reg) {
	page_segs_destruct(&reg->segments);
}

vector_type_cd(page_regs, page_region, page_region_vec_t, page_region_construct, page_region_destruct);