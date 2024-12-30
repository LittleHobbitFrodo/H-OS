//
//	memory/heap/table-heap/table-heap.h
//		part of the CORE kernel belonging to the H-OS project
//


//	NOTE: do NOT include in include.h

//	heap implementation dedicated for page table management
//	works with aligned data with fixed sizes
//	needs regular heap to work
//	each region can store 255 tables (2mb -> one pt)
	//	if extended one table will be stored at the end of the region to map another region

#pragma once

#include "../../../integers.h"
#include "../multipurpose/heap.h"


static void table_heap_reserve_memory();

any_page_table* table_alloc(table_allocator_t* alloc, u16 count);

void table_free(table_allocator_t* alloc, any_page_table* table);

bool table_shrink(table_allocator_t* alloc, any_page_table* table, u16 count);

void table_heap_debug(table_allocator_t* alloc);

