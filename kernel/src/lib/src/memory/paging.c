//
//	memory/paging.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../../memory/paging.h"

#ifdef H_OS_LIB_MEMORY_PAGING_H
	#ifndef H_OS_LIB_MEMORY_PAGING_C
		#define H_OS_LIB_MEMORY_PAGING_C



		page_entry* page_find() {
			page_entry* ret;
			asm volatile("mov %0, cr3" : "=r"(ret));
			return ret;
		}

		void page_init() {

			enum page_flags nullf = present | no_exec;

			pml4 = page_find();
			pd.ptr = (aligned_ptr){.ptr = null, .align = 4096, .offset = 4096};
			pd.count = PAGE_COUNT;
			pt.ptr = (aligned_ptr){.ptr = null, .align = 4096, .offset = 4096};

			base_virtual = (void*)k_address.response->virtual_base;
			base_physical = (void*)k_address.response->physical_base;

			memmap_entry *hent = null, *sent = null, *pent = null;
			{	//	calculate page count
				memmap_entry* mem;
				for (size_t iii = 0; iii < memmap.len; iii++) {
					switch ((mem = ((memmap_entry*)vec_at(&memmap, iii)))->type) {
						case heap: {
							hent = mem;
							break;
						}
						case stack: {
							sent = mem;
							break;
						}
						case paging: {
							pent = mem;
							break;
						}
						default: break;
					}
				}
				{
					u8 check = (hent == null) | ((sent == null) << 1) | ((pent == null) << 2);
					if (check != 0) {
						if ((check & 0b1) != 0) {
							report("paging init: cannot locate kernel heap memory map entry\n", critical);
						}
						if ((check & 0b10) != 0) {
							report("paging init: cannot locate kernel stack memory map entry\n", critical);
						}
						if ((check & 0b100) != 0) {
							report("paging init: cannot loacte paging table memory map entry\n", critical);
						}
						panic(paging_initialization_failure);
					}
				}
				pt.count = (hent->len / PAGE_SIZE) + (hent->len % PAGE_SIZE != 0);
				pt.count += (sent->len / PAGE_SIZE) + (sent->len % PAGE_SIZE != 0);
				pt.count += (pent->len / PAGE_SIZE) + (pent->len % PAGE_SIZE != 0);
				//	round up to 512
				pt.count = ((pt.count / PAGE_COUNT) + 1) * PAGE_COUNT;

				//	allocate pt
				aptr_alloc(&pt.ptr, pt.count * sizeof(page_entry));
				for (size_t i = 0; i < pt.count; i++) {
					((page_entry*)pt.ptr.ptr)[i] = (page_entry)nullf;
				}

			}
			//	hent, sent, kent != null

			//	find highest used pml4 entry
			page_entry* ptr;
			for (ssize_t i = PAGE_COUNT - 1; (pd.ptr.ptr == null) && (i >= 0); i--) {
				if (page_address(pml4[i]) != null) {
					ptr = page_address(pml4[i]);
					for (ssize_t ii = PAGE_COUNT - 1; i >= 0; i--) {
						if (page_address(ptr[i]) == null) {
							//	allocate pd
							aptr_alloc(&pd.ptr, pd.count * sizeof(page_entry));
							for (size_t iii = 0; iii < pd.count; iii++) {
								((page_entry*)pd.ptr.ptr)[iii] = (page_entry)nullf;
							}

							//	connect pdpt to pd
							page_set_address(&ptr[ii], pd.ptr.ptr);

							//	fill pt pages
								//	pt[0 .. x]	->	pages
								//	pt[x .. y]	->	heap
								//	pt[512- z, 512]	->	stack
							{
								size_t count = (pent->len / PAGE_SIZE) + (pent->len % PAGE_SIZE != 0);
								size_t iii = 0;
								for (; iii < count; iii++) {
									page_set_address(&((page_entry*)pt.ptr.ptr)[iii],  (void*)(pent->base + (iii * PAGE_SIZE)));
								}
								count = iii + ((hent->len / PAGE_SIZE) + (hent->len % PAGE_SIZE != 0));
								for (; iii < count; iii++) {
									page_set_address(&((page_entry*)pt.ptr.ptr)[iii], (void*)(hent->base + (iii * PAGE_SIZE)));
								}
								count = (sent->len / PAGE_SIZE) + (sent->len % PAGE_SIZE != 0);
								size_t start = pt.count - count;
								for (iii = start; iii < pt.count; iii++) {
									page_set_address(&((page_entry*)pt.ptr.ptr)[iii], (void*)(sent->base + ((iii - start) * PAGE_SIZE)));
								}
							}

							//	connect pts to pd

							size_t count = pt.count / PAGE_COUNT;
							for (size_t iii = 0; iii < count; iii++) {
								page_set_address(&((page_entry*)pd.ptr.ptr)[iii], (void*)((page_entry*)pt.ptr.ptr)[iii * PAGE_COUNT]);
							}
							break;
						}
					}
				}
			}

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
			page_entry ent = pml4[va_index(virt, 3)];
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

		/*void* physical(void* virt) {
			page_entry ent = pages.pml4.base[va_index(virt, 3)];
			if (unlikely((!(ent & present)) || (page_address(ent) == null))) {
				return null;
			}
			for (i16 i = 2; i >= 0; --i) {
				ent = ((page_entry*)page_address(ent))[va_index(virt, i)];
				if (unlikely((!(ent & present)) || (page_address(ent) == null))) {
					print("physical: ret layer:\t"); printu(i); endl();
					return null;
				}
			}
			return (void*)((size_t)page_address(ent) + va_offset(virt));
		}*/

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

	#endif
	//	#warning memory/paging.c already included
	//#endif
#else
	#error memory/paging.c: memory/paging.h not included
#endif