//
//	memory/heap/page-heap.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../../memory/heap/page-heap.h"

void page_heap_init() {
	//	allocate page tables into regular heap
	//	add them into paging structure (test it)
	//	reserve memory for page heap
	//	cover page heap in virtual addresses
	//	allocate tables (in page heap) and add them into paging structure
	//	make the map the page heap
	//	delete allocated page tables (regular heap)
	//	initialize page heap

	page_heap_segment_t* seg = vvec_push(&page_heap.segments, 1);
	seg->entries = page_heap.base.virtual;
	seg->table_count = 1;
	seg->used = false;
}

bool page_heap_reserve_memory() {
	//	updates the page_heap.physical.start and .end variable

	memnull(&page_heap, sizeof(page_heap_t));
	vvecs(&page_heap.segments, sizeof(page_heap_segment_t));
		//	initializes volatile vector

	struct limine_memmap_entry* ent;
	struct {
		struct limine_memmap_entry **entries;
		size_t size;
	} mmp;
	mmp.entries = req_memmap.response->entries;
	mmp.size = req_memmap.response->entry_count;


	//	calculate size for page heap
	page_heap.size = (size_t)heap.physical.end - (size_t)heap.physical.start;
		//	get heap size
	page_heap.size = (page_heap.size / PAGE_SIZE) * (page_heap.size % PAGE_SIZE != 0);
		//	get page count
	page_heap.size = ((page_heap.size / PAGE_COUNT) + 1) * PAGE_COUNT;
		//	round up to 512


	//	find good spot for it
	for (size_t i = 0; i < mmp.size; i++) {
		//	find usable memmap entry big enough
		ent = mmp.entries[i];
		if ((ent->type == LIMINE_MEMMAP_USABLE) && (ent->base != (u64)heap.physical.start)) {
			if (ent->length >= (page_heap.size * PAGE_SIZE)) {
				//	pre-initialize page heap
				page_heap.physical.start = (void*)ent->base;
				page_heap.physical.end = (void*)((size_t)page_heap.physical.start + (page_heap.size * PAGE_SIZE));
				page_heap.base.virtual = (void*)((size_t)pages.hhdm + (size_t)page_heap.physical.start);
				return true;
			}
		}
	}
	return false;
}

void page_heap_debug() {
	u32 c = output.color;
	output.color = col.blue;
	printl("page heap scheme:");
	page_heap_segment_t* seg;
	for (size_t i = 0; i < page_heap.segments.len; i++) {
		seg = vvec_at_unlocked(&page_heap.segments, i);
		printu(i); print(":\t"); printp(seg->entries); print(" : "); printp((void*)(((size_t)seg->entries - (size_t)page_heap.base.virtual) + (size_t)page_heap.base.physical)); endl();
		printc('\t'); printu(seg->table_count); print("\t"); printl((seg->used)? "used" : "unused");
	}
	output.color = c;
}


/*void page_heap_map() {
	//	1)	map one pml4 entry to page_heap.physical.start
	//	2)	statically allocate pages here (rheap + pheap)
	//	3)	remap the pml4 entry to point to the allocated pages
	//	direct map the memory

	//	1.1)	find unused pml4 entry
	page_entry* pml4 = null;
	virtual_address* va = (virtual_address*)&pages.heap.page;
	for (ssize_t i = PAGE_COUNT - 1; (i & (1 << 8)) != 0; i--) {
		if ((*pages.pml4)[i].address == 0) {
			pml4 = &(*pages.pml4)[i];
			va->pml4 = i;
			print("pml4 i:\t"); printu(i); endl();
			break;
		}
	}
	if (pml4 == null) {
		report("cannot find enpty pml4 entry for heap mapping\n", report_critical);
		panic(panic_code_cannot_allocate_memory_for_kernel_heap);
		__builtin_unreachable();
	}
	va->sign = 0xffff;

	//	1.2)	map the entry to selected location
	*((u64*)pml4) = 0;
	pml4->address = (size_t)page_heap.physical.start >> PAGE_SHIFT;
	pml4->present = true;
	pml4->execute_disable = true;
	pml4->write = true;

	print("address:\t"); printp(pages.heap.page); endl();

	printl("testing pre-map:");
	for (size_t i = 0; i < WAIT_INT; i++) {
		iowait();
	}

	size_t* ptr = (size_t*)pages.heap.page;
	*ptr = 69;
	print("ptr:\t"); printu(*ptr);

}*/