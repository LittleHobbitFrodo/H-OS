//
//	memory/heap/table-heap/structures.h
//		part of the CORE kernel belonging to the H-OS project
//

//	IMPORTANT:	do NOT include in include.h
	//	included in paging.h

//	each allocator is 2mb in size (uses one sized page entry)
	//	can allocate max 511 tables
	//	uses first page for metadata

#pragma once
#include "../../../integers.h"

typedef struct table_ptr {
	//	4 bytes in size (the smaller the better)
	u32 index:		9;		//	table index in the allocator
	u32 count:		9;		//	table count
	u32 used:		1;
	u32 _reserved:	13;
} __attribute__((packed)) table_ptr;

//typedef any_page_table table_heap_t[512] __attribute__((aligned(0x200000)));		//	exactly 2mb of memory, aligned to 2mb
typedef struct table_heap_t {

	//	first 4kb is used as metadata
	table_ptr ptr[512];
	u64 bitmap[64];		//	each bit for each table used/free
	heap_metadata meta;		//	metadata for *data



	__attribute__((aligned(4096))) any_page_table table[511];

} __attribute__((aligned(0x200000))) table_heap_t;

typedef struct table_allocator_t {

	table_heap_t* data;
		//	if multiple tables allocated (bit for each table is marked as used too)

	struct table_allocator_t* next;

} table_allocator_t;
