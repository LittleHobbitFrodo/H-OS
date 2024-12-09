//
//	memory.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../memory.h"

#include "../vector/vector.h"

void memory_init() {

	kernel_status = k_state_init_memory;
	if (req_k_address.response == null) {
		report("unable to get virtual/physical base address\n", report_critical);
		panic(panic_code_base_addresses_not_available);
		__builtin_unreachable();
	}

	base.virtual = (void*)req_k_address.response->virtual_base;
	base.physical = (void*)req_k_address.response->physical_base;

	if ((req_memmap.response == null) || (req_memmap.response->entries == null)) {
		report("memory map not found", report_critical);
		panic(panic_code_memmap_not_found);
	}

	//	initialize paging
	page_init();

	//	initialize heap for page table allocations
	page_heap_init();

	//	initialize regular heap
	heap_init();

	//	parse memory map
	memmap_parse();

	//	initialize task state segment (needed for GDT initialization)
	tss_init();

	//	initialize global descriptor table
	gdt_init();

	if (vocality >= vocality_vocal) {
		report("memory initialization completed\n", report_note);
	}
}



void memmap_parse() {
	//	parse limine memory map and simplify it
		//	join entries of same type ...


	memmap_construct(1);
	ssize_t msize = req_memmap.response->entry_count, heap_index = -1, page_heap_index = -1;
	struct limine_memmap_entry **ents = req_memmap.response->entries;
	enum memmap_types tmp = memmap_undefined;

	struct limine_memmap_entry *ent;
	ssize_t i = 0;
	{
		//	prepare first entry
		memmap_entry *first = memmap.data;
		first->base = ents[0]->base;
		first->type = memmap_entry_type(ents[i]->type);
		for (++i; memmap_entry_type(ents[i]->type) == tmp; i++);
		--i;
		first->len = ents[i]->base + ents[i]->length - first->base;
		++i;
	}

	for (; i < msize; i++) {
		ent = ents[i];

		if (ent->base == (size_t)heap.physical.start) {
			//	initialize heap entry
			memmap_entry *h = memmap_push(1);
			h->base = ent->base;
			h->len = HEAP_MINIMAL_ENTRY_SIZE * KB;
			h->type = memmap_heap;
			heap_index = memmap.len - 1;
		} else if (ent->base == (size_t)pages.heap.physical) {
			memmap_entry* h = memmap_push(1);
			h->base = ent->base;
			h->len = pages.heap.size;
			h->type = memmap_heap;
			page_heap_index = memmap.len - 1;
		}

		tmp = memmap_entry_type(ent->type);
		//	skip entries of same type
		for (++i; (i < msize) && (memmap_entry_type(ents[i]->type) == tmp); i++);
		--i;

		memmap_entry *new = memmap_push(1);
		new->base = ent->base;
		new->type = tmp;
		if (i + 1 < msize) {
			new->len = ents[i + 1]->base - new->base;
		} else {
			new->len = ents[i]->base + ents[i]->length - new->base;
		}
	}

	//	make heap entry not overlap other entries
	memmap_entry* es = memmap.data;
	es[heap_index + 1].base += es[heap_index].len;
	es[heap_index + 1].len -= es[heap_index].len;
	es[page_heap_index + 1].base += es[page_heap_index].len;
	es[page_heap_index + 1].len -= es[page_heap_index].len;


	//	gather info about memory usage
	memmap_analyze();

	//	apply stacks (stck != null)
		//	paging is not initialized yet -> special allocated memory will be used for stacks (memory.h)
	//stack.kernel = (void*)(stck->base + stck->len - 1);
	stack.kernel = (void*)((size_t)&KERNEL_STACK + (32*KB) - 1);
	for (i = 0; i < 7; i++) {
		//stack.interrupt[i] = (void*)(stck->base + ((INTERRUPT_STACK_SIZE * KB) * (i+1)) - 1);
		stack.interrupt[i] = (void*)((size_t)&INTERRUPT_STACK + ((i+1) * (8*KB)) - 1);
	}

	if (vocality >= vocality_report_everything) {
		report("memory map parsed successfully\n", report_note);
	}
}


enum memmap_types memmap_entry_type(u64 constant) {
	//	converts limine memmap entry types into enum memmap_types
	switch (constant) {
		case LIMINE_MEMMAP_USABLE: {
			return memmap_usable;
		}
		case LIMINE_MEMMAP_RESERVED: {
			return memmap_reserved;
		}
		case LIMINE_MEMMAP_FRAMEBUFFER: {
			return memmap_other;
		}
		case LIMINE_MEMMAP_BAD_MEMORY: {
			return memmap_bad;
		}
		case LIMINE_MEMMAP_ACPI_NVS: {
			return memmap_acpi;
		}
		case LIMINE_MEMMAP_KERNEL_AND_MODULES: {
			return memmap_kernel;
		}
		case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE: case LIMINE_MEMMAP_ACPI_RECLAIMABLE: {
			return memmap_reclaimable;
		}
		default: {
			return memmap_undefined;
		}
	}
}

void memmap_display() {

	u32 c = output.color;
	memmap_entry *ents = (memmap_entry *) memmap.data;

	if (ents == null) {
		report("custom memmap does not exist\n", report_problem);
		return;
	}

	printl("custom memory map");
	for (size_t i = 0; i < memmap.len; i++) {
		output.color = col.white;
		printu(i + 1);
		print(":\t");
		switch (ents[i].type) {
			case memmap_usable: {
				output.color = col.green;
				print("usable:\t\t");
				break;
			}
			case memmap_reserved: {
				output.color = col.grey;
				print("reserved:\t");
				break;
			}
			case memmap_bad: {
				output.color = col.red;
				print("bad memory:\t");
				break;
			}
			case memmap_other: {
				output.color = col.cyan;
				print("other:\t\t");
				break;
			}
			case memmap_kernel: {
				output.color = col.yellow;
				print("kernel:\t\t");
				break;
			}
			case memmap_reclaimable: {
				output.color = col.green;
				print("reclaim:\t");
				break;
			}
			case memmap_paging: {
				output.color = col.yellow;
				print("paging:\t\t");
				break;
			}
			case memmap_acpi: {
				output.color = col.yellow;
				print("acpi:\t\t");
				break;
			}
			case memmap_heap: {
				output.color = col.orange;
				print("heap:\t\t");
				break;
			}
			case memmap_stack: {
				output.color = col.blue;
				print("stack:\t\t");
				break;
			}
			default: {
			output.color = col.critical;
				print("UNKNOWN:\t");
				break;
			}
		}
		printp((void *) ents[i].base);
		print("\tto\t");
		printp((void *) (ents[i].base + ents[i].len));
		endl();
	}
	output.color = c;
}

void memmap_display_original() {
	u32 c = output.color;
	struct limine_memmap_entry **ents = req_memmap.response->entries;
	size_t size = req_memmap.response->entry_count;

	if (ents == null) {
		report("original memmap does not exist\n", report_problem);
		return;
	}

	if (ents == null) {
		report("original memmap does not exist\n", report_problem);
		return;
	}

	printl("original memory map:");
	struct limine_memmap_entry *ent;
	for (size_t i = 0; i < size; i++) {
		output.color = col.white;
		ent = ents[i];
		printu(i + 1);
		print(":\t");
		switch (ent->type) {
			case LIMINE_MEMMAP_USABLE: {
				output.color = col.green;
				print("usable:\t\t");
				break;
			}
			case LIMINE_MEMMAP_RESERVED: {
				output.color = col.grey;
				print("reserved:\t");
				break;
			}
			case LIMINE_MEMMAP_BAD_MEMORY: {
				output.color = col.red;
				print("bad mem:\t");
				break;
			}
			case LIMINE_MEMMAP_FRAMEBUFFER: {
				output.color = col.cyan;
				print("fbuffer:\t");
				break;
			}
			case LIMINE_MEMMAP_KERNEL_AND_MODULES: {
				output.color = col.yellow;
				print("kernel:\t\t");
				break;
			}
			case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE: {
				output.color = col.green;
				print("reclaim:\t");
				break;
			}
			case LIMINE_MEMMAP_ACPI_NVS:
			case LIMINE_MEMMAP_ACPI_RECLAIMABLE: {
				output.color = col.yellow;
				print("ACPI:\t\t");
				break;
			}
			default: {
				output.color = col.critical;
				print("UNKNOWN:\t");
				break;
			}
		}
		printp((void *) ent->base);
		print("\tto\t");
		printp((void *) (ent->base + ent->length)); tab();
		printu(ent->length);
		endl();
	}
	output.color = c;
}

void memmap_analyze() {
	//	gather info about memory usage

	meminfo.total = 0;
	meminfo.usable = 0;
	meminfo.used = 0;
	meminfo.reserved = 0;
	meminfo.ring0 = 0;
	meminfo.unmapped = 0;

	memmap_entry *es = memmap.data;

	for (size_t i = 0; i < memmap.len; i++) {
		switch (es[i].type) {
			case memmap_usable: {
				//	usable memory
				meminfo.usable += es[i].len;
				break;
			}
			case memmap_heap:
			case memmap_stack: {
				//	used + ring0
				meminfo.ring0 += es[i].len;
				meminfo.used += es[i].len;
				break;
			}
			case memmap_reserved: {
				//	reserved memory
				meminfo.reserved += es[i].len;
				break;
			}
			case memmap_kernel:
			case memmap_acpi:
			case memmap_other: {
				//	ring 0
				meminfo.ring0 += es[i].len;
				break;
			}
			case memmap_bad: {
				meminfo.unmapped += es[i].len;
				break;
			}
			case memmap_undefined: {
				report("memory map entry index ", report_error);
				printu(i);
				printl(" has undefined type");
				break;
			}
			default: break;
		}
	}

	meminfo.total = es[memmap.len - 1].base + es[memmap.len - 1].len;
}

/*void memmap_reclaim() {
	//	reclaims reclaimable entries

	if (memmap.data == null) {
		report("memmap_reclaim: memory map does not exist\n", report_critical);
		panic(panic_code_memmap_not_found);
	}

	memmap_entry* olds = memmap.data;

	//	change all reclaimable entries to usable
	for (size_t i = 0; i < memmap.len; i++) {
		if (olds[i].type == memmap_reclaimable) {
			olds[i].type = memmap_usable;
		}
	}

	memmap_vector_t old;
	vec_take_over(&old, &memmap);
	vecs(&memmap, sizeof(memmap_entry));

	enum memmap_types tmp = memmap_undefined;

	ssize_t i = 0;
	{
		//	prepare first entry
		memmap_entry *first = (memmap_entry *) vec_push(&memmap, 1);
		first->base = olds[0].base;
		first->type = olds[0].type;
		tmp = first->type;
		for (++i; (i < (ssize_t)old.len) && (olds[i].type == tmp); i++);
		--i;
		first->len = olds[i].base + olds[i].len - first->base;
		++i;
	}

	memmap_entry* ent;
	for (; i < (ssize_t)old.len; i++) {
		ent = &olds[i];

		tmp = ent->type;
		for (++i; (i < (ssize_t)old.len) && (olds[i].type == tmp); i++);
		--i;

		memmap_entry *new = (memmap_entry *) vec_push(&memmap, 1);
		new->base = ent->base;
		new->type = tmp;
		if (i + 1 < (ssize_t)old.len) {
			new->len = olds[i + 1].base - new->base;
		} else {
			new->len = olds[i].base + olds[i].len - new->base;
		}

	}

	vec_free(&old);

	//	gather info about memory usage
	memmap_analyze();

	if (vocality >= vocality_report_everything) {
		report("memory reclaimed\n", report_note);
	}
}*/

memmap_entry* memmap_find(enum memmap_types type) {
	memmap_entry* ret;
	for (size_t i = 0; i < memmap.len; i++) {
		ret = memmap_at(i);
		if (ret->type == type) {
			return ret;
		}
	}
	return null;
}