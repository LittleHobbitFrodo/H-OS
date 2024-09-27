//
//	memory/heap.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../memory/heap.h"

void* heap_virtual_base = null;

bool heap_reserve_memory(bool include_reclaimable_entries) {

	struct limine_memmap_entry* mstart = null;
	size_t mlen = 0;

	size_t size = req_memmap.response->entry_count;
	struct limine_memmap_entry* ent = null;

	if (include_reclaimable_entries) {
		for (size_t i = 0; i < size; i++) {
			ent = req_memmap.response->entries[i];
			if (ent == null) {
				output.color = col.critical;
				print("ERROR");
				output.color = col.white;
				print(":\tmemory map entry ");
				printu(i);
				printl(" is NULL");
				continue;
			}
			if ((ent->type == LIMINE_MEMMAP_USABLE) || (ent->type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE)) {
				if (mstart == null) {
					mstart = ent;
				}
				mlen += ent->length;
			} else {
				mstart = null;
				mlen = 0;
			}
			if (mlen >= HEAP_MINIMAL_ENTRY_SIZE * KB) {
				//	mlen != 0 => mstart != null
				heap_start = (heap_segment*)mstart->base;
				heap_end_physical = (void*)((mstart->base + mlen));
				return true;
			}
		}
	} else {
		for (size_t i = 0; i < size; i++) {
			ent = req_memmap.response->entries[i];
			if (ent == null) {
				output.color = col.critical;
				print("ERROR");
				output.color = col.white;
				print(":\tmemory map entry ");
				printu(i);
				printl(" is NULL");
				continue;
			}
			if (ent->type == LIMINE_MEMMAP_USABLE) {
				if (mstart == null) {
					mstart = ent;
				}
				mlen += ent->length;
			} else {
				mstart = null;
				mlen = 0;
			}
			if (mlen >= HEAP_MINIMAL_ENTRY_SIZE * KB) {
				//	mlen != 0 => mstart != null
				heap_start = (heap_segment*)mstart->base;
				heap_end_physical = (void*)((mstart->base + mlen));
				return true;
			}
		}
	}
	return false;
}

void heap_debug() {
	u32 c = output.color;
	heap_segment* i = heap_start;
	output.color = col.cyan;
	printl("heap scheme: ");
	size_t ii = 0;
	for (;; i = i->next) {
		output.color = col.white;
		printu(ii); print("\t");
		output.color = col.cyan;
		printp(i);
		print("):\tsize("); printu(i->size); print("):\t");
		print("\tnext("); printp(i->next);
		printl((i->used)? "used" : "free");
		if (i->next == null) {
			break;
		}
		++ii;
	}
	endl();
	output.color = c;
}
void heap_init() {
	//	heap_reserve_memory() must be called before this function
	heap_start->next = null;
	heap_start->used = false;
	heap_start->size = HEAP_INITIAL_BLOCK_SIZE;
	heap_end = (heap_used_until = heap_start);
}

#include "./heap/physical.c"