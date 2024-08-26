//
//	memory/paging.c
//		part of the CORE kernel belonging to the H-OS project
//

#include "../../memory/paging.h"

#include "../../k_management.h"
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

			//	align_alloco() allocate aligned data and optimizes memory
				//	heap headers are in kernel/src/lib/memory/heap.h
				//	heap sources are in kernel/src/lib/src/memory/heap.c

			enum page_flags dflags = present | no_exec;		//	default flags
			enum page_flags hflags = present | write | write_through | global | no_exec;	//	default flags for heap
			enum page_flags kflags = present | write | write_through | global;				//	default flags for kernel
			enum page_flags sflags = hflags;	//	default flags for stack

			virtual_base = (void*)k_address.response->virtual_base;
			physical_base = (void*)k_address.response->physical_base;
			page_entry* current_paging_setup = page_find();

			//	pml4
			pml4.size = PAGE_COUNT;
			pml4.base = align_alloco(sizeof(page_entry) * pml4.size, &pml4.offset);
			for (size_t i = 0; i < pml4.size; i++) {
				pml4.base[i] = dflags;		//	null entry
			}

			//	pdpt
			pdpt.size = meminfo.total / GB + ((meminfo.total % GB) != 0);	//	each pdpt entry covers 1GB
			pdpt.base = align_alloco(sizeof(page_entry) * pdpt.size, &pdpt.offset);
			page_set_address(pml4.base, pdpt.base);
			for (size_t i = 0; i < pdpt.size; i++) {
				pdpt.base[i] = dflags;
			}

			//	kpd
			size_t pd_i = 0;	//	pd iterator (for assinging each pt)
			kpd.size = PAGE_COUNT;
			kpd.base = align_alloco(sizeof(page_entry) * PAGE_COUNT, &kpd.offset);
			page_set_address(pdpt.base, kpd.base);
			for (size_t i = 0; i < kpd.size; i++) {
				kpd.base[i] = dflags;
			}

			//	pt
			//	pages for kernel memmap entry

			memmap_entry *hent = null, *kent = null, *sent = null;

			{
				memmap_entry* ent;
				for (size_t i = 0; i < memmap.len; i++) {
					ent = vec_at(&memmap, i);
					switch (ent->type) {
						case heap: {
							hent = ent;
							break;
						}
						case kernel_: {
							kent = ent;
							break;
						}
						case stack: {
							sent = ent;
							break;
						}
						default: {
							break;
						}
					}
				}
				u8 check = (hent == null) || ((kent == null) << 1) || ((sent == null) << 2);
				if (unlikely(check != 0)) {
					if (check & 1) {
						report("cannot find kernel heap memory map entry\n", critical);
					}
					if ((check & (1 << 1)) != 0) {
						report("cannot find kernel memory map entry\n", critical);
					}
					if ((check & (1 << 2)) != 0) {
						report("cannot find kernel stack memory map entry\n", critical);
					}
					hang();
				}
			}

			{
				size_t kcount;
				size_t scount;
				//	heap will be separated (kernel and stack will be together in paging (unlike heap kernel and stack cannot grow))
				page_ring0.size = (kcount = ((kent->len / PAGE_SIZE) + (kent->len % PAGE_SIZE != 0)));
				page_ring0.size += (scount = ((sent->len / PAGE_SIZE) + (sent->len % PAGE_SIZE != 0)));
				page_ring0.base = align_alloco(sizeof(page_entry) * page_ring0.size, &page_ring0.offset);

				//	fill kernel entries
				for (size_t i = 0; i < kcount; i++) {
					page_set_address(&page_ring0.base[i], (void*)(kent->base + (i * PAGE_SIZE)));
					page_set_flags(&page_ring0.base[i], kflags);
				}
				scount += kcount;
				for (size_t i = kcount; i < scount; i++) {
					page_set_address(&page_ring0.base[i], (void*)(sent->base + (i * PAGE_SIZE)));
					page_set_flags(&page_ring0.base[i], sflags);
				}
			}

			//	assign pd entries
			{
				size_t rsize = (page_ring0.size / PAGE_COUNT) + (page_ring0.size % PAGE_COUNT != 0);	//	number of pd entries needed to assign ring0 PTs
				for (;pd_i < rsize; pd_i++) {
					page_set_address(&kpd.base[pd_i], (void*)((size_t)page_ring0.base + (pd_i * PAGE_COUNT)));
				}
			}

			//	heap
			page_heap.size = (hent->len / PAGE_SIZE) + (hent->len % PAGE_SIZE != 0);		//	minimum entries
			page_heap.size = ((page_heap.size / 512) + 1) * 512;		//	round up (to 512)
			page_heap.base = align_alloco(sizeof(page_entry) * page_heap.size, &page_heap.offset);

			{
				size_t addr = (size_t)heap_start;
				size_t lasta = (size_t)heap_start + hent->len;
				size_t i = 0;
				for (; addr < lasta; i++) {		//	fill valid entries
					addr = (size_t)heap_start + (PAGE_SIZE * i);
					page_set_address(&page_heap.base[i], (void*)addr);
					page_set_flags(&page_heap.base[i], hflags);
				}
				for (; i < page_heap.size; i++) {
					page_heap.base[i] = dflags;
				}
			}

			//	assign pd entries to point to heap pt
			{
				const size_t pd_i_start = pd_i;
				const size_t rsize = page_heap.size / 512;		//	page_heap.size is rounded to 512
				for (; pd_i - pd_i_start < rsize; pd_i++) {
					page_set_address(&kpd.base[pd_i], (void*)((size_t)page_heap.base + ((pd_i - pd_i_start) * PAGE_COUNT)));
				}
			}

			if (vocality >= report_everything) {
				report("paging re-initialization prepared successfully\n", note);
			}

			//	use paging
			size_t current_stack_physical = 0;

			//	NOTE: external kernel_stack_base variable is set in init.asm

			print("stack base virtual:\t"); printp(kernel_stack_base); endl();

			//	manually translate stack pointer
			{
				print("current paging setup:\t"); printp(current_paging_setup); endl();

				size_t index_pml4 = va_index(kernel_stack_base, 3);
				size_t index_pdpt = va_index(kernel_stack_base, 2);
				size_t index_pd = va_index(kernel_stack_base, 1);
				size_t index_pt = va_index(kernel_stack_base, 0);
				size_t offset = va_offset(kernel_stack_base);

				page_entry pml4 = current_paging_setup[index_pml4];
				if (!(pml4 & present)) {
					report("pml4: page not present\n", critical);
					panic(cannot_locate_kernel_stack);
				}

				page_entry pdpt = ((page_entry*)page_address(pml4))[index_pdpt];
				if (!(pdpt & present)) {
					report("pdpt: page not present\n", critical);
					panic(cannot_locate_kernel_stack);
				}

				page_entry pd = ((page_entry*)page_address(pdpt))[index_pd];
				if (!(pd & present)) {
					report("pd: page not present\n", critical);
					panic(cannot_locate_kernel_stack);
				}





				/*ssize_t lvl = 2;
				page_entry* i = page_address(current_paging_setup[va_index(kernel_stack_base, 3)]);
				for (; lvl >= 0; lvl--) {
					i = page_address(i[va_index(kernel_stack_base, lvl)]);
				}

				void* stack_physical = (void*)((size_t)i + va_offset(kernel_stack_base));*/

				//print("stack base physical: "); printp(stack_physical); endl();
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

	#else
		#warning memory/paging.c already included
	#endif
#else
	#error memory/paging.c: memory/paging.h not included
#endif