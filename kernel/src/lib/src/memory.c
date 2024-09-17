//
//	memory.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifdef H_OS_LIB_MEMORY_H
	#ifndef H_OS_LIB_MEMORY_C
		#define H_OS_LIB_MEMORY_C

		void* KERNEL_STACK_END = &KERNEL_STACK[65536];

		void* kernel_stack_address = null;
		//void* heap_virtual_base = null;
		//void* interrupt_stack_address = null;
		//void* pml4_address = null;
		void* kernel_stack_base = null;

		enum memmap_types memmap_entry_type(u64 constant)  {
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
				case LIMINE_MEMMAP_ACPI_NVS: case LIMINE_MEMMAP_ACPI_RECLAIMABLE: {
					return memmap_acpi;
				}
				case LIMINE_MEMMAP_KERNEL_AND_MODULES: {
					return memmap_kernel;
				}
				case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE: {
					return memmap_reclaimable;
				}
				default: {
					return memmap_undefined;
				}
			}
		}

		void memmap_display(bool original) {
			if ((memmap.data == null) || (original)) {
				struct limine_memmap_entry** ents = req_memmap.response->entries;
				size_t size = req_memmap.response->entry_count;
				printl("original memory map:");
				struct limine_memmap_entry* ent;
				for (size_t i = 0; i < size; i++) {
					output.color = col.white;
					ent = ents[i];
					printu(i + 1); print(":\t");
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
						case LIMINE_MEMMAP_ACPI_NVS: case LIMINE_MEMMAP_ACPI_RECLAIMABLE: {
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
					printp((void*)ent->base);
					print("\tto\t");
					printp((void*)(ent->base + ent->length));
					endl();
				}
			} else {
				memmap_entry* ents = (memmap_entry*)memmap.data;
				printl("costom memory map");
				for (size_t i = 0; i < memmap.len; i++) {
					output.color = col.white;
					printu(i + 1); print(":\t");
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
							output.color = col.green;
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
					printp((void*)ents[i].base);
					print("\tto\t");
					printp((void*)(ents[i].base + ents[i].len));
					endl();
				}
			}
			output.color = col.white;
		}

		enum panic_codes memory_init() {

			//	reserves memory for heap
			if (!heap_reserve_memory(false)) {
					//	initialize (maybe) temporary heap
					//	once the kernel will deal with all reclaimable entries heap will be (maybe) reinitialized
					//	meybe = if any reclaimable entry is in place where we want the kernel heap
				return pc_cannot_allocate_memoey_for_kernel_heap;
			}

			//	initialize heap
			heap_init();

			//	parse command line arguments
			parse_cmd();
				//	better parse cmd earlier

			//	parse memory map
			memmap_parse();

			//	initialize paging
			//page_init();

			//	initialize task state segment (needed for GDT initialization)
			tss_init();

			//	initialize global descriptor table
			gdt_init();

			tss_init();

			return pc_ok;
		}


		void memmap_parse() {

			vecs(&memmap, sizeof(memmap_entry));
			size_t msize = req_memmap.response->entry_count;
			struct limine_memmap_entry** ents = req_memmap.response->entries;
			enum memmap_types tmp = memmap_undefined;
			ssize_t stack_index = -1;
			size_t paging_address = (size_t)page_find();

			//	localize future stacks
			/*{
				size_t stack_start_entry = 0;
				ssize_t ssize = 0;
				for (ssize_t i = msize - 1; i > 0; i--) {
					if (ents[i]->type == LIMINE_MEMMAP_USABLE) {
						if (ssize == 0) {
							stack_start_entry = i;
						}
						ssize += (ssize_t)ents[i]->length;
					} else {
						ssize = 0;
					}
					if (ssize >= STACK_MINIMUM_INITIAL_SIZE * KB) {
						stack_index = stack_start_entry;
						break;
					}
				}
				if (stack_index < 0) {
					panic(cannot_allocate_memory_for_stacks);
				}
			}*/

			struct limine_memmap_entry* ent;
			size_t i = 0;

			{
				//	prepare first entry
				memmap_entry* first = (memmap_entry*)vec_push(&memmap, 1);
				first->base = ents[0]->base;
				first->type = memmap_entry_type(ents[i]->type);
				for (++i; memmap_entry_type(ents[i]->type) == tmp; i++);
				--i;
				first->len = ents[i]->base + ents[i]->length - first->base;
				++i;
			}

			bool stack_defined = false;

			size_t heap_index = 0;

			for (; i < msize; i++) {
				ent = ents[i];

				if (ent->base == (size_t)heap_start) {
					memmap_entry* h = (memmap_entry*)vec_push(&memmap, 1);
					h->base = ent->base;
					h->len = HEAP_MINIMAL_ENTRY_SIZE * KB;
					h->type = memmap_heap;
					heap_index = memmap.len - 1;
				} else if (ent->type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE) {
					/*print("memmap_parse:\tphysical:\t");
					{
						void* stack;
						asm volatile("mov %0, rsp" : "=r"(stack));
						printp(physical(stack));
						endl();
					}*/
					if ((paging_address > ent->base) && (paging_address < ent->base + ent->length)) {
						memmap_entry* p = (memmap_entry*)vec_push(&memmap, 1);
						p->base = ent->base;
						p->len = ent->length;
						p->type = memmap_paging;
						continue;
					}
				}

				tmp = memmap_entry_type(ent->type);
				for (++i; (i < msize) && (memmap_entry_type(ents[i]->type) == tmp); i++);
				--i;

				memmap_entry* new = (memmap_entry*)vec_push(&memmap, 1);
				new->base = ent->base;
				new->type = tmp;
				if (i+1 < msize) {
					new->len = ents[i+1]->base - new->base;
				} else {
					new->len = ents[i]->base + ents[i]->length - new->base;
				}

				if (((ssize_t)i >= stack_index) && (!stack_defined)) {
					memmap_entry* stck = (memmap_entry*)vec_push(&memmap, 1);
					stck->len = (KERNEL_STACK_SIZE + INTERRUPT_STACK_SIZE) * KB;
					stck->base = new->base + new->len - stck->len;
					stck->type = memmap_stack;
					new->len -= stck->len;
					stack_defined = true;
				}
			}

			//	set heap entry
			memmap_entry* es = memmap.data;
			es[heap_index+1].base += es[heap_index].len;
			es[heap_index+1].len -= es[heap_index].len;


			//	initialize meminfo structure (memory.h)
			meminfo.total = 0;
			meminfo.usable = 0;
			meminfo.used = 0;
			meminfo.reserved = 0;
			meminfo.ring0 = 0;
			meminfo.unmapped = 0;

			for (i = 0; i < msize; i++) {
				switch (es[i].type) {
					case memmap_usable: {
						//	usable memory
						meminfo.usable += es[i].len;
						break;
					}
					case memmap_heap: case memmap_stack: {
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
					case memmap_kernel: case memmap_acpi: case memmap_other: {
						//	ring 0
						meminfo.ring0 += es[i].len;
						break;
					}
					case memmap_bad: {
						meminfo.unmapped += es[i].len;
						break;
					}
					case memmap_undefined: {
						report("memory map entry index ", rs_error);
						printu(i); printl(" has undefined type");
						break;
					}
					default: break;
				}
			}

			meminfo.total = es[memmap.len - 1].base + es[memmap.len-1].len;

			if (vocality >= vocality_report_everything) {
				report("memory map parsed successfully\n", rs_note);
			}

		}


		void* aptr_alloc(aligned_ptr* this, size_t bytes) {
			if (this->ptr == null) {
				this->offset = this->align;
				this->ptr = palign_alloc(bytes, &this->offset);
				return this->ptr;
			}
			return null;
		}

		void* aptr_realloc(aligned_ptr* this, size_t bytes) {
			if (this->ptr == null) {
				return aptr_alloc(this, bytes);
			}
			return (this->ptr = palign_realloc(this->ptr, &this->offset, this->align, bytes));
		}

		void* aptr_reallocf(aligned_ptr* this, size_t bytes, void (*on_realloc)(void*)) {
			if (this->ptr == null) {
				return aptr_alloc(this, bytes);
			}
			return (this->ptr = palign_reallocf(this->ptr, &this->offset, this->align, bytes, on_realloc));
		}

		void* aptr_realloca(aligned_ptr* this, size_t bytes, size_t add) {
			if (this->ptr == null) {
				return aptr_alloc(this, bytes);
			}
			return (this->ptr = palign_realloca(this->ptr, &this->offset, this->align, bytes, add));
		}

		void* aptr_reallocaf(aligned_ptr* this, size_t bytes, size_t add, void (*on_realloc)(void*)) {
			if (this->ptr == null) {
				return aptr_alloc(this, bytes);
			}
			return (this->ptr = palign_reallocaf(this->ptr, &this->offset, this->align, bytes, add, on_realloc));
		}


		void va_info(void* addr) {
			printp(addr); printl(":");
			print("pml4\t["); printu(va_index(addr, 3)); printl("]");
			print("pdpt\t["); printu(va_index(addr, 2)); printl("]");
			print("pd\t\t["); printu(va_index(addr, 1)); printl("]");
			print("pt\t\t["); printu(va_index(addr, 0)); printl("]");
			print("offset:\t"); printu(va_offset(addr)); endl();
		}

	#endif
	//	#warning memory.c already included
	//#endif
#else
	#error memory.c: memory.h not included
#endif