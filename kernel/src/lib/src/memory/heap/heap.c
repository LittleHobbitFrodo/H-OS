//
//	memory/heap.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../../memory/heap/multipurpose/heap.h"
#include "../../../memory/paging.h"


void heap_init() {
	//	initializes global alloc

	//	1)	find place for global heap
	//	2)	construct virtual address space for it
	//	3)	initialize heap

	size_t line = 0;

	if (vocality >= vocality_report_everything) {
		line = report("initializing kernel multipurpose heap\n", report_note);
	}

	heap.global.meta.size = HEAP_GLOBAL_MINIMAL_SIZE * KB;

	{	//	find place for global heap
		struct limine_memmap_entry* ent;
		bool found = false;
		size_t msize = req_memmap.response->entry_count;
		for (size_t i = 0; i < msize; i++) {
			ent = req_memmap.response->entries[i];
			if ((ent->type == LIMINE_MEMMAP_USABLE) && (ent->length >= HEAP_GLOBAL_MINIMAL_SIZE * KB)) {
				heap.global.meta.physical.start = ent->base;
				heap.global.meta.physical.end = ent->base + heap.global.meta.size;
				heap.global.meta.allocator = &heap.global;
				heap.global.meta.virtual.start = pages.system.heap.virtual;
				heap.global.meta.virtual.end = (void*)((size_t)heap.global.meta.virtual.start + heap.global.meta.size);
				found = true;
				break;
			}
		}
		if (!found) {
			if (vocality >= vocality_report_everything) {
				report_status("FAILURE", line, col.critical);
			}
			report("could not allocate memory for kernel heap\n", report_critical);
			panic(panic_code_cannot_allocate_memory_for_kernel_heap);
		}
	}


	{	//	construct address space
		page_entry* ent = pages.system.heap.table;
		for (size_t i = 1; (i < PAGE_COUNT) && ((i-1)*4096 < heap.global.meta.size); i++) {
			ent[i].address = (heap.global.meta.physical.start + (4096 * i)) >> PAGE_SHIFT;
		}
	}

	//	initialize alloc
	heap.global.alloc = multipurpose_alloc;
	heap.global.realloc = multipurpose_realloc;
	heap.global.realloca = multipurpose_realloca;
	heap.global.free = multipurpose_free;

	//	initialize heap
	heap.global.first = (heap.global.first_free = heap.global.meta.virtual.start);
	heap.global.last = heap.global.first;

	heap_block* b = heap.global.first;
	b->next = null;
	b->size = HEAP_INITIAL_BLOCK_SIZE;
	b->used = false;
	b->lock = false;

	if (vocality >= vocality_report_everything) {
		report_status("SUCCESS", line, col.green);
	}

}

void* multipurpose_alloc(allocator_t* alloc, heap_size_t bytes) {
	heap_wait_and_mark_pending(alloc);
	heap_block* con = null;
	size_t c_size = 0;
	bytes = align(bytes, 8);

	for (heap_block* block = alloc->first;; block = block->next) {

		if (block->used) {
			con = null;
			c_size = 0;
		} else {

			if (block->size >= bytes) {
				block->used = true;
				if (block->size >= bytes + sizeof(heap_block) + 32) {
					_heap_divide(alloc, block, bytes);
				}
				heap_pending_unmark(alloc);
				return ++block;
			}

			c_size += block->size;
			if (con == null) {
				con = block;
			} else if (c_size >= bytes) {
				_heap_connect(alloc, con, block);
				con->used = true;
				heap_pending_unmark(alloc);
				return ++con;
			}
		}

		if (block->next == null) {
			break;
		}
	}
	void* ret = _heap_enlarge(alloc, bytes);
	return ret;
}

void* multipurpose_realloc(allocator_t* alloc, void* ptr, heap_size_t bytes) {
	heap_size_t block_size = heap_bsize(ptr);
	bytes = align(bytes, 8);
	if (bytes <= block_size) {
		if (block_size > sizeof(heap_block) + 32 + bytes) {
			_heap_divide(alloc, ((heap_block*)ptr)-1, bytes);
		}
		heap_pending_unmark(alloc);
		return ptr;
	}

	//	block_size < new->size
	heap_pending_unmark(alloc);
	u64* new = alloc->alloc(alloc, bytes);
	block_size /= sizeof(u64);
	for (size_t i = 0; i < block_size; i++) {
		new[i] = ((u64*)ptr)[i];
	}
	(((heap_block*)ptr)-1)->used = false;
	return new;
}

void* multipurpose_realloca(allocator_t* alloc, void* ptr, heap_size_t bytes, heap_size_t add) {
	heap_wait_and_mark_pending(alloc);
	heap_size_t block_size = heap_bsize(ptr);
	bytes = align(bytes, 8);
	if (bytes <= block_size) {
		if (block_size > sizeof(heap_block) + 32 + bytes) {
			_heap_divide(alloc, ((heap_block*)ptr)-1, bytes);
		}
		heap_pending_unmark(alloc);
		return ptr;
	}

	//	block_size < new->size
	heap_pending_unmark(alloc);
	u64* new = alloc->alloc(alloc, bytes + add);
	block_size /= sizeof(u64);
	for (size_t i = 0; i < block_size; i++) {
		new[i] = ((u64*)ptr)[i];
	}
	(((heap_block*)ptr)-1)->used = false;
	heap_pending_unmark(alloc);
	return new;
}


void* _heap_enlarge(allocator_t* alloc, heap_size_t bytes) {
	if (alloc->last->used) {
		//	create new block and set the last pointer to it

		heap_block* old = alloc->last;
		alloc->last->size = align(alloc->last->size, 8);
		heap_block* new = (void*)align(((size_t)(old + 1) + old->size), 8);
		alloc->last = new;
		new->next = null;
		new->size = align(bytes, 8);
		new->lock = false;
		new->used = true;
		old->next = new;
		heap_pending_unmark(alloc);
		return new+1;
	}

	//	resize last (unused) block
	alloc->last->size = align(bytes, 8);
	alloc->last->used = true;
	heap_pending_unmark(alloc);
	return alloc->last + 1;
}

void _heap_connect(allocator_t* alloc, heap_block* beg, heap_block* fin) {
	if (fin->next == null) {
		beg->size = (size_t)fin - (size_t)beg + fin->size;
	} else {
		beg->next = fin;
		beg->size = (size_t) fin - (size_t) beg;
	}
	heap_pending_unmark(alloc);
}

void _heap_divide(allocator_t* alloc, heap_block* block, heap_size_t bytes) {
	if (block->next == null) {
		printl("\t\t\t\tdivide: last");
		block->size = align(bytes, 8);
	} else {
		print("\t\t\t\tdivide: not last: size("); printu(block->size); print(") bytes("); printu(bytes); print(") block("); printp(block); printl(")");
		bytes = align(bytes, 8);
		heap_block* new = (void*)align((size_t)block + sizeof(heap_block) + bytes, 8);
		new->next = block->next;
		new->size = block->size - sizeof(heap_block) - bytes;
		new->lock = (new->used = false);
		block->next = new;
	}
	heap_pending_unmark(alloc);
}


void multipurpose_free(allocator_t* alloc, void* ptr) {
	heap_block* block = ((heap_block*)ptr - 1);
	block->used = false;
	if ((size_t)block < (size_t)alloc->first_free) {
		alloc->last = block;
	}
}

void heap_debug(allocator_t* alloc) {
	u32 c = output.color;
	output.color = col.cyan;
	print("allocator "); printp(alloc); printl(" scheme:");
	for (heap_block* i = alloc->first;; i = i->next) {
		tab(); printp(i); print((i->used)? "\tused" : "\tfree"); tab(); printu(i->size); print("\tnext("); printp(i->next); printl(")");

		if (i->next == null) {
			break;
		}
	}
	output.color = c;
}
