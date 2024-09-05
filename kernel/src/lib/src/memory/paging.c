//
//	memory/paging.c
//		part of the CORE kernel belonging to the H-OS project
//

#include "../../memory.h"
#ifdef H_OS_LIB_MEMORY_PAGING_H
	#ifndef H_OS_LIB_MEMORY_PAGING_C
		#define H_OS_LIB_MEMORY_PAGING_C

		page_entry* page_find() {
			page_entry* ret;
			asm volatile("mov %0, cr3" : "=r"(ret));
			return ret;
		}

		void page_init() {

			//	default paging layout
			//		pml4[pml4.size - 1] is for kernel stuff
			//			pdpt[0] is kernel data
			//			pdpt[pdp.size-1] is kernel stack
			//			kernel pd structure:
			//				[start ... kernel pages] -> kernel data (code, etc)
			//				[end kernel pages ... end kernel pages + heap pages] -> kernel heap
			//				[end - stack pages ... end] -> kernel stack
			//		other entries are for processes

			//	initialize structures
			pages.pml4 = (page_table){.base = null, .offset = 4096, .size = 512};
			pages.pdpt = (page_table){.base = null, .offset = 4096, .size = 0};
			pages.pd = (page_table){.base = null, .offset = 4096, .size = 0};
			pages.pt.table = (page_table){.base = null, .offset = 4096, .size = 0};

			kernel_stack_address = null;

			enum page_flags nullf = present | no_exec;

			//	calculate sizes for each level
				//	pml4.size is initialized in header
			pages.pdpt.size = (meminfo.total / GB);

			//	find memmap entries
			memmap_entry *kent = null, *sent = null, *hent = null;
			{
				memmap_entry* ent;
				for (size_t i = 0; i < memmap.len; i++) {
					ent = vec_at(&memmap, i);
					switch (ent->type) {
						case kernel_: {
							kent = ent;
							break;
						}
						case stack: {
							sent = ent;
							break;
						}
						case heap: {
							hent = ent;
							break;
						}
						case undefined: {
							report("memory map entry number ", warning);
							printu(i);
							printl("is undefined");
							break;
						}
						default: {
							break;
						}
					}
				}
				u8 check = (kent == null) | ((sent == null) << 1) | ((hent == null) << 2);
				if (check != 0) {
					if (check & 0b1) {
						report("kernel memory map entry was not found\n", critical);
					}
					if ((check & 0b10) != 0) {
						report("kernel stack memory map entry was not found\n", critical);
					}
					if ((check & 0b100) != 0) {
						report("kernel heap memory map entry was not found\n", critical);
					}
					hang();
				}
			}

			//	memmap entry pointers are not null

			pages.pt.table.size = (pages.pt.heap.size = (hent->len / PAGE_SIZE) + (hent->len % PAGE_SIZE != 0));
			pages.pt.table.size += (pages.pt.stack.size = (sent->len / PAGE_SIZE) + (sent->len % PAGE_SIZE != 0));
			pages.pt.table.size += (pages.pt.kernel.size = (kent->len / PAGE_SIZE) + (kent->len % PAGE_SIZE != 0));

			//	round up (to 512)
			pages.pt.table.size = ((pages.pt.table.size / PAGE_COUNT) + 1) * PAGE_COUNT;

			{
				pages.pd.size = (pages.pt.table.size / PAGE_COUNT) + (pages.pt.table.size % PAGE_COUNT != 0);
				pages.pd.size += pages.pd.size == 0;	//	make sure that size is not 0
			}

			//	allocate memory for each entry
			pages.pml4.base		= align_alloco(sizeof(page_entry) * pages.pml4.size, &pages.pml4.offset);
			pages.pdpt.base		= align_alloco(sizeof(page_entry) * pages.pdpt.size, &pages.pdpt.offset);
			pages.pd.base		= align_alloco(sizeof(page_entry) * pages.pd.size, &pages.pd.offset);

			pages.pt.table.base	= align_alloco(sizeof(page_entry) * pages.pt.table.size, &pages.pt.table.offset);

			//	set all entries to null
			for (size_t i = 0; i < pages.pml4.size; i++) {
				pages.pml4.base[i] = (page_entry)nullf;
			}
			for (size_t i = 0; i < pages.pdpt.size; i++) {
				pages.pdpt.base[i] = (page_entry)nullf;
			}
			for (size_t i = 0; i < pages.pd.size; i++) {
				pages.pd.base[i] = (page_entry)nullf;
			}

			for (size_t i = 0; i < pages.pt.table.size; i++) {
				pages.pt.table.base[i] = (page_entry)nullf;
			}

			//	connect entries
			//	pml4 -> pdpt
			size_t sub = pages.pdpt.size / PAGE_COUNT;
			sub += sub == 0;
			size_t i = 0;
			for (; i < pages.pdpt.size / PAGE_COUNT; ++i) {
				page_set_address(&pages.pml4.base[pages.pml4.size - sub + i], &pages.pdpt.base[i * PAGE_COUNT]);
			}
			va_set_index(&kernel_stack_address, pages.pml4.size - 1, 3);

			//	pdpt -> pd
			sub = pages.pd.size / PAGE_COUNT;
			sub += sub == 0;
			for (i = 0; i < sub; ++i) {
				page_set_address(&pages.pdpt.base[pages.pdpt.size - sub + i], &pages.pd.base[i * PAGE_COUNT]);
			}
			va_set_index(&kernel_stack_address, pages.pdpt.size - 1, 2);

			//	pd -> pt (kernel, heap, ... , stack)
			sub = pages.pt.table.size / PAGE_COUNT;
			sub += sub == 0;
			for (i = 0; i < sub; ++i) {
				page_set_address(&pages.pd.base[pages.pd.size - sub + i], &pages.pt.table.base[i * PAGE_COUNT]);
			}
			va_set_index(&kernel_stack_address, --i, 1);

			//	fill pt entries (to point to correct locations)
			pages.pt.kernel.ptr = pages.pt.table.base;
			for (i = 0; i < pages.pt.kernel.size; ++i) {
				page_set_address(&pages.pt.table.base[i], (void*)(kent->base + (i * PAGE_SIZE)));
			}
			sub = pages.pt.kernel.size + pages.pt.heap.size;
			for (; i < sub; ++i) {
				page_set_address(&pages.pt.table.base[i], (void*)(hent->base + ((i - pages.pt.kernel.size) * PAGE_SIZE)));
			}
			sub = pages.pt.table.size - pages.pt.stack.size;
			pages.pt.stack.ptr = &pages.pt.table.base[sub];
			for (i = sub; i < pages.pt.table.size; ++i) {
				page_set_address(&pages.pt.table.base[i], (void*)(sent->base + ((i - sub) * PAGE_SIZE)));
			}
			va_set_index(&kernel_stack_address, pages.pt.table.size - 1, 0);
			va_set_offset(&kernel_stack_address, 0xfff);

			interrupt_stack_address = kernel_stack_address;
			va_set_index(&interrupt_stack_address, pages.pt.table.size - (KERNEL_STACK_SIZE / 4), 0);


			//	pre-apply paging
				//	rsp -> kernel_stack_address
				//	rbp -> ?
				//	rax -> resolution
			//pml4_address = pages.pml4.base;

		}

		void page_entry_info(page_entry ent) {
			enum page_flags pflags = present;
			u32 c = output.color;
			output.color = col.cyan;
			if (ent & pflags) {
				print("present ");
			}
			pflags = global;
			if (ent & pflags) {
				print("global ");
			}
			{
				void* addr = page_address(ent);
				print("page entry:\t");
				if (addr == null) {
					printp(addr);
					endl();
				} else {
					printl("NULL");
				}
			}
			pflags = write;
			print("\tperms:\t"); print((ent & pflags)? "rw" : "r"); pflags = no_exec; printc('x' * (ent & pflags)); endl();
			pflags = cache_disable;
			print("\tcaching:\t");
			if (ent & pflags) {
				printl("disabled");
			} else {
				pflags = write_through;
				printl((ent & pflags)? "write-through" : "write-back");
			}
			pflags = accessed;
			print("\tstate:\t");
			if (ent & pflags) {
				print("accessed, ");
			}
			pflags = dirty;
			printl((ent & pflags)? "dirty" : "clean");
			pflags = PS;
			print("\tsize:\t\t"); printl((ent & pflags)? "2MiB" : "4kb");
			output.color = c;
		}


		void* physical(void* virt) {
			page_entry ent = pages.pml4.base[va_index(virt, 3)];
			if (unlikely((!(ent & present)) || (page_address(ent) == null))) {
				return null;
			}
			for (i16 i = 2; i >= 0; --i) {
				ent = ((page_entry*)page_address(ent))[va_index(virt, i)];
				if (unlikely((!(ent & present)) || (page_address(ent) == null))) {
					return null;
				}
			}
			return (void*)((size_t)page_address(ent) + va_offset(virt));
		}

		//	virtual_() is deprecated, not implementing it unless it is NEEDED
			//	needed for implementation? : round up all page table allocations to 512 entries
		/*void* virtual_(void* phys) {
			page_entry ent;
			for (ssize_t i = pages.pml4.size - 1; i >= 0; --i) {
				ent = pages.pml4.base[i];
				if (unlikely(((ent & present) && (page_address(ent) != null))) {
					for (size_t ii = 0; ii < PAGE_COUNT)
				}
			}
		}*/

	#else
		#warning memory/paging.c already included
	#endif
#else
	#error memory/paging.c: memory/paging.h not included
#endif