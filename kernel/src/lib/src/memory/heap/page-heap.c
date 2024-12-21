//
//	memory/heap/page-heap.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../../memory/heap/page-heap/page-heap.h"
#include "../../../vector/volatile-vector.h"

void page_header_construct(page_header* self, page_heap* heap) {
	size_t* ptr = (size_t*)self;
	for (size_t i = 0; i < PAGE_HEAP_SIZE / sizeof(size_t); i++) {
		ptr[i] = 0;
	}
	(*self)[0].table = heap->meta.virtual.start;
	(*self)[0].count = 1;
	(*self)[0].used = false;

}

void page_heap_construct(page_heap* self, page_allocator_t* alloc) {
	memnull(self, sizeof(page_heap));
	self->meta.allocator = alloc;
}

void page_heap_init() {
	//	static pages for page heap are initialized

	//	1)	find place for page heap

	size_t line = 0;
	if (vocality >= vocality_report_everything) {
		line = report("kernel page table allocator initialization:\n", report_note);
	}

	pages.heap.size = 2*MB;

	{	//	find place for heap
		struct limine_memmap_entry* ent;
		size_t mlen = req_memmap.response->entry_count;
		bool found = false;
		for (size_t i = 0; i < mlen; i++) {
			ent = req_memmap.response->entries[i];
			if ((ent->type == LIMINE_MEMMAP_USABLE) && (ent->length >= 2*MB)) {
				found = true;
				pages.heap.init.physical = ent->base;
				pages.heap.init.table = &pages.system.page_heap.pd;
				pages.heap.init.virtual = pages.system.page_heap.virtual;
				break;
			}
		}
		if (!found) {
			if (vocality >= vocality_report_everything) {
				report_status("CRITICAL FAILURE", line, col.critical);
			}
			report("could not allocate memory for kernel page heap\n", report_critical);
			panic(panic_code_cannot_allocate_memory_for_kernel_heap);
		}
	}

	for (size_t i = 0; i*4096 < pages.heap.size; i++) {
		pages.system.page_heap.pt[i].address = (pages.heap.init.physical + (i*4096)) >> PAGE_SHIFT;
	}

	memnull(&pages.heap.global, sizeof(page_allocator_t));
	page_heaps_construct(&pages.heap.global.heaps, 1, &heap.global, &pages.heap.global);


	page_heap* global = pages.heap.global.heaps.data;
	global->meta.physical.start = pages.heap.init.physical;
	global->meta.physical.end = pages.heap.init.physical + pages.heap.size;
	global->meta.size = pages.heap.size;
	global->meta.virtual.start = pages.heap.init.virtual;
	global->meta.virtual.end = (void*)((size_t)pages.heap.init.virtual + pages.heap.size);
	global->meta.table = &pages.system.page_heap.pt;
	global->meta.allocator = &pages.heap.global;
	global->header = global->meta.virtual.start;

	for (size_t i = 0; i < 8; i++) {
		global->bitmap[i] = 0;
	}

	for (size_t i = 0; i < 256; i++) {
		(*global->header)[i].table = (page_table_t*)((size_t)pages.heap.init.virtual + (i*4096));
		(*global->header)[i].count = 1;
		(*global->header)[i].used = false;
	}

	if (vocality >= vocality_report_everything) {
		report_status("SUCCESS", line, col.green);
	}

}

page_table_t* page_alloc(page_allocator_t* alloc, u8 count) {
	//	go through each heap in allocator
	//	if one bitmap chunk is not MAX_U32 -> at least one page pointer is unused

	page_heap* current;
	page_ptr* ptr = null;
	u32 map;
	for (size_t i = 0; i < alloc->heaps.len; i++) {
		current = page_heaps_at(&alloc->heaps, i);

		for (size_t bitmap = 0; bitmap < 8; bitmap++) {
			//	check if all pointer are used
			if ((map = current->bitmap[i]) == 0xffffffff) {
				continue;
			}

			//	find empty bits
			for (size_t bit = 0; bit < 32; bit++) {
				for (size_t free_count = 0; !((map >> bit) & 1); bit++, free_count++) {
					//	if empty bit in row >= <count>
					if (free_count >= count) {
						ptr = &(*current->header)[(bitmap*32) + bit];
						print("found ptr: "); printu((bitmap*32) + bit); print(" fc: "); printu(free_count); endl();
						goto allocate;
					}
				}
			}
		}
	}
	return null;
	allocate:
	ptr->used = true;
	ptr->count = count;
	return ptr->table;
}

void page_heap_debug() {

}

//void page_allocator_init(page_allocator_t* self) {

//}



/*bool page_heap_reserve_memory() {

	struct limine_memmap_entry* ent;
	const size_t msize = req_memmap.response->entry_count;

	//	calculate page heap size
	pages.heap.size = (size_t)heap.meta.physical.end - (size_t)heap.meta.physical.end;
	pages.heap.size = (pages.heap.size / PAGE_SIZE) + (pages.heap.size % PAGE_SIZE != 0);
	pages.heap.size = ((pages.heap.size / PAGE_COUNT) + 1) * PAGE_COUNT;
	pages.heap.size *= PAGE_SIZE;

	//	find valid memory map entry (exclude bl. reclaimable)
	for (size_t i = 0; i < msize; i++) {
		if ((ent = req_memmap.response->entries[i])->type == LIMINE_MEMMAP_USABLE) {
			if (ent->length >= pages.heap.size) {
				pages.heap.init.physical = ent->base;
				return heap_map(pages.heap.init.physical, &pages.heap.init.virtual, pages.heap.size, (void**)&pages.heap.init.table);
			}
		}
	}
	return false;
}

void page_heap_init() {
	//	map whole heap memory

	report("proceeding to initialize page heap\n", report_warning);

	page_regs_construct(&pages.heap.regions, 1);
	page_region* reg = &pages.heap.regions.data[1];
	reg->meta.physical.start = pages.heap.init.physical;
	reg->meta.physical.end = reg->meta.physical.start + pages.heap.size;

	reg->meta.size = pages.heap.size;

	reg->meta.virtual.start = pages.heap.init.virtual;
	reg->meta.virtual.end = (void*)((size_t)pages.heap.init.virtual + pages.heap.size);

	reg->meta.table = pages.heap.init.table;

	//	allocate segments
	page_segs_construct(&reg->segments, 1);
	page_segment* seg = &reg->segments.data[0];
	seg->used = false;
	seg->count = 1;
	seg->entries = reg->meta.virtual.start;
	print("segment pushed:\t"); printu(reg->segments.len); print(" : "); printp(reg->segments.data); endl();

	printl("page heap initialized");
}





page_table_t* page_alloc([[maybe_unused]]u32 count) {
	return null;
}

page_table_t* page_realloc([[maybe_unused]] page_table_t* table, [[maybe_unused]] u32 count) {
	return null;
}

void page_free([[maybe_unused]] page_table_t* table) {

}

page_segment* page_heap_find(page_table_t* table) {
	const size_t tab = (size_t)table;
	page_region* reg = null;

	if (pages.heap.regions.len == 1) {
		reg = &pages.heap.regions.data[0];

		if ((tab >= (size_t)reg->meta.virtual.start) && (tab <= (size_t)reg->meta.virtual.end)) {
			return page_region_find(reg, table);
		}

		return null;
	}
	//	binary search
	u32 low = 0;
	u32 high = pages.heap.regions.len-1;
	u32 mid;

	while (low <= high) {

		mid = low + (high - low) / 2;
		reg = &pages.heap.regions.data[mid];

		if ((tab >= (size_t)reg->meta.virtual.start) && (tab <= (size_t)reg->meta.virtual.end)) {
			return page_region_find(reg, table);
		}

		if ((size_t)reg->meta.virtual.end < tab) {
			low = mid+1;
		} else {
			high = mid-1;
		}
	}
	return null;
}

page_segment* page_region_find(page_region* region, page_table_t* table) {
	const size_t tab = (size_t)table;
	u32 low = 0;
	u32 high = region->segments.len - 1;
	u32 mid;
	const page_segment* segs = region->segments.data;

	while (low <= high) {
		mid = low + (high - low) / 2;

		if ((size_t)segs[mid].entries == tab) {
			return (page_segment*)&segs[mid];
		}

		if (tab < (size_t)segs[mid].entries) {
			high = mid - 1;
		} else {
			low = mid + 1;
		}
	}
	return null;
}

void page_heap_debug() {
	u32 c = output.color;
	output.color = col.blue;
	printl("page heap scheme:");
	page_region* reg;
	page_segment* seg;
	for (size_t i = 0; i < pages.heap.regions.len; i++) {
		reg = &pages.heap.regions.data[i];
		print("\tregion "); printu(i); print(":\t"); printu(reg->segments.len); endl();
		for (size_t ii = 0; ii < reg->segments.len; i++) {
			seg = &reg->segments.data[i];
			tab(); tab(); printp(seg->entries); print(":\t"); printu(seg->count); print(":\t");printl((seg->used)? "used" : "unused");
		}
	}
	output.color = c;
}
*/
/*
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

//void page_heap_init() {

//}

void page_heap_init() {
	//	allocate page tables into regular heap
	//	add them into paging structure (test it)
	//	reserve memory for page heap
	//	cover page heap in virtual addresses
	//	allocate tables (in page heap) and add them into paging structure
	//	make the map the page heap
	//	delete allocated page tables (regular heap)
	//	initialize page heap

	if ((size_t)page_heap.physical.end + (4*MB) > (size_t)4*GB) {
		//	4 Mb -> leave space for future expansion
		//	create page table before page heap
		page_entry* entry;
		page_table_t* table = page_quick_map(page_heap.physical.start, &entry);
		if (table == null) {
			report("could not map page heap into virtual memory\n", report_error);
			panic(panic_code_cannot_allocate_memory_for_kernel_heap);
			__builtin_unreachable();
		}

		{	//	fill the table
			u64 *tmp = (u64 *) table;
			for (size_t i = 0; i < PAGE_COUNT; i++) {
				tmp[i] = 0b11 | ((u64) 1 << 63);
					//	present, write, exec disable
			}
		}


		//	construct virtual address and connect table to system pdpt
		union virtual_union virt = {.voidptr = pages.system.pdpt.virtual};
		for (ssize_t i = 511; i >= 0; i--) {
			if (pages.system.pdpt.page[i].address == 0) {
				virt.virtual_address.pdpt = i;
				pages.system.pdpt.page[i].address = (size_t)page_heap.physical.start >> PAGE_SHIFT;
				break;
			}
		}
		page_heap.base.virtual = virt.voidptr;


		entry->address = 0;
	}

	page_heap_segment_t* seg = vvec_push(&page_heap.segments, 1);
	seg->entries = page_heap.base.virtual;
	seg->table_count = 1;
	seg->used = false;
	vvec_unlock((&page_heap.segments));
}



void page_heap_debug() {
	vvec_wait_and_lock((&page_heap.segments));
	u32 c = output.color;
	output.color = col.blue;
	printl("page heap scheme:");
	page_heap_segment_t* seg;
	for (size_t i = 0; i < page_heap.segments.len; i++) {
		seg = vvec_at(&page_heap.segments, i);
		printu(i); print(":\t"); printp(seg->entries); print(" : "); printu(seg->table_count); print("\t:\t"); printl((seg->used)? "used" : "unused");
	}
	output.color = c;
	vvec_unlock((&page_heap.segments));
}

page_table_t* page_alloc(u32 tables) {
	//	returns virtual address
	vvec_wait_and_lock((&page_heap.segments));
	page_table_t* ret = __page_alloc_locked(tables);
	vvec_unlock((&page_heap.segments));
	return ret;
}

page_table_t* __page_alloc_locked(u32 tables) {
	page_heap_segment_t* segs = page_heap.segments.data;
	for (size_t i = page_heap.used_until; i < page_heap.segments.len; i++) {
		if ((!segs[i].used) && (segs[i].table_count >= tables)) {
			if ((segs[i].table_count > tables) && (segs[i].table_count > 1)) {
				__page_heap_divide_block(i, tables);
			}
			segs[i].used = true;
			if (page_heap.used_until < i) {
				page_heap.used_until = i;
			}
			return segs[i].entries;
		}
	}

	//	enlarges page heap
	page_heap_segment_t* last = vvec_last(&page_heap.segments);
	if (!last->used) {
		last->used = true;
		last->table_count = tables;
		return last->entries;
	}

	return page_heap_expand(tables, false);
}

page_table_t* page_realloc(page_table_t* ptr, u32 tables, bool* reallocated) {
	vvec_wait_and_lock((&page_heap.segments));
	ssize_t index = page_find_index(ptr);
	if (index == -1) {
		page_table_t* ret = __page_alloc_locked(tables);
		vvec_unlock((&page_heap.segments));
		return ret;
	}
	page_heap_segment_t* seg = vvec_at(&page_heap.segments, index);
	if (seg->table_count >= tables) {
		if (reallocated != null) {
			*reallocated = false;
		}
		if ((seg->table_count > tables) && (seg->table_count > 1)) {
			__page_heap_divide_block(index, tables);
		}
		vvec_unlock((&page_heap.segments));
		return ptr;
	}
	page_table_t* new = page_alloc(tables);
	page_cpy(ptr, new, PAGE_TABLE_SIZE * ((seg->table_count > tables)? tables : seg->table_count));
	seg->used = false;
	vvec_unlock((&page_heap.segments));
	return new;
}





page_table_t* page_heap_expand(size_t tables, bool unlock) {
	//	takes the vector as if it is already locked
	//	will unlock the vector

	page_heap_segment_t* last = vvec_last(&page_heap.segments);
	page_heap_segment_t* new = vec_push((multipurpose_vector*)&page_heap.segments, 1);
	new->used = true;
	new->entries = last->entries + last->table_count;
	new->table_count = tables;
	if (unlock) {
		vvec_unlock((&page_heap.segments));
	}
	return new->entries;
}

void __page_heap_divide_block(size_t segment, [[maybe_unused]] size_t tables) {
	//	divides page heap block -> resize page_heap.segments[segment] to tables
	//	function assumes:
		//	vector is already locked and not accessed
		//	tables < segments[segment].table_count

	vec_push((multipurpose_vector*)&page_heap.segments, 1);
	page_heap_segment_t* segs = page_heap.segments.data;
	for (size_t i = page_heap.segments.len - 1; i > segment + 1; i--) {
		segs[i] = segs[i-1];
	}
	page_heap_segment_t* old = &segs[segment];
	page_heap_segment_t* new = &segs[segment+1];
	new->entries = old->entries + tables;
	new->table_count = old->table_count - tables;
	new->used = false;
	old->table_count = tables;
}


void page_free(page_table_t* ptr) {
	vvec_wait_and_lock((&page_heap.segments));
	page_heap_segment_t* seg = page_find(ptr);
	if (seg != null) {
		seg->used = false;
	}
	vvec_unlock((&page_heap.segments));
}

page_heap_segment_t* page_find(page_table_t* ptr) {
	//	uses the vector as already locked
	//	binary search implementation
	page_heap_segment_t *segs = page_heap.segments.data;
	size_t low = 0, high = page_heap.segments.len - 1, mid;

	do {
		mid = low + (high - low) / 2;

		if (segs[mid].entries == ptr) {
			return &segs[mid];
		}

		if (segs[mid].entries < ptr) {
			low = mid + 1;
		} else {
			high = mid - 1;
		}

	} while (low <= high);

	return null;
}

ssize_t page_find_index(page_table_t* ptr) {
	//	uses the vector as already locked
	//	binary search implementation
	page_heap_segment_t *segs = page_heap.segments.data;
	size_t low = 0, high = page_heap.segments.len - 1, mid;

	do {
		mid = low + (high - low) / 2;

		if (segs[mid].entries == ptr) {
			return mid;
		}

		if (segs[mid].entries < ptr) {
			low = mid + 1;
		} else {
			high = mid - 1;
		}

	} while (low <= high);

	return -1;
}*/