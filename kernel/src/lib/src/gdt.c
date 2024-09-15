//
//	gdt.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../gdt.h"

#ifndef H_OS_LIB_GDT_C
	#define H_OS_LIB_GDT_C

	void gdt_entry_info(const gdt_entry* entry, bool simple) {
		u32 c = output.color;
		output.color = col.blue;

		if ((entry->access & gdt_bits_segment_type) != 0) {
			print("code/data segment:\t");
		} else {
			print("system segment:\t");
		}
		printp((void*)entry); endl();
		print("\tbase:\t"); printp(gdt_base(entry)); endl();
		print("\tlimit:\t"); printu(gdt_limit(entry)); endl();

		if (simple) {

			print("\taccess:\t"); printu(entry->access); endl();
			print("\tflags:\t"); printu(entry->flags & 0xf0); endl();

		} else {
			print("\taccess:\t");
			if ((entry->access & gdt_bits_present) != 0) {
				print("present ");
			}
			print("priv("); printu(((entry->access & gdt_bits_privilege) >> 5)); print(") ");
			if ((entry->access & gdt_bits_executable) != 0) {
				print("exec ");
			}
			if ((entry->access & gdt_bits_segment_type) != 0) {
				//	code/data
				print(((entry->access & gdt_bits_conform) != 0)? "conforming " : "nonconfirming ");

				print(((entry->access & gdt_bits_data_write) != 0)? "rw" : "r");

				print(((entry->access & gdt_bits_executable) != 0)? "x " : " ");

			} else {
				//	system segment
				print(((entry->access & gdt_bits_direction) != 0)? "grows(down) " : "grows(up)) ");

				print(((entry->access & gdt_bits_code_read) != 0)? "read " : "noread ");
			}

			if ((entry->access & gdt_bits_accessed) != 0) {
				print("accessed ");
			}
			endl();

			//	flags
			print("flags:\t");
			if ((entry->flags & granular) != 0) {
				print("granular ");
			}

			if ((entry->flags & long_mode) != 0) {
				print("64bit");
			} else {
				if ((entry->flags & protected_mode) != 0) {
					print("32bit");
				} else {
					print("16bit");
				}
			}
			endl();
		}

		output.color = c;
	}

	void gdt_init() {

		//	default gdt setup
		//	[0]		NULL
		//	[1]		kernel code segment
		//	[2]		kernel data segment (heap)
		//	[3]		kernel data segment (stack)
		//	[4]		TSS segment (later...)



		//	initialize gdt pointer
		gdt_pointer = (aligned_ptr){.ptr = null, .offset = 8, .align = 8};
		aptr_alloc(&gdt_pointer, sizeof(gdt_ptr));
		gdt_ptr* ptr = (gdt_ptr*)gdt_pointer.ptr;
		ptr->entries = null;		//	yet not initialzed
		ptr->size = 0;				//	...

		//	initialize gdt vector
		avecsa(&gdt, 5, sizeof(gdt_entry), 8);
		gdt_entry* ents = (gdt_entry*)gdt.data.ptr;

		//	[0]		null entry
		gdt_entry_null(&ents[0]);

		//	[1]		kernel code
		{	//	find kernel memory map entry
			gdt_entry_null(&ents[1]);		//	clear all bits
			memmap_entry* ent = null;
			for (size_t i = 0; i < memmap.len; i++) {
				if ((ent = ((memmap_entry*)vec_at(&memmap, i)))->type == kernel_) {
					break;
				}
			}
			if (ent == null) {
				report("could not find kernel memory map entry", critical);
				panic(gdt_initialization_failure);
			}
			//	apply data
			gdt_set_base(&ents[1], (void*)ent->base);
			gdt_set_limit(&ents[1], ent->len);
			gdt_set_flags(&ents[1], long_mode);
			gdt_set_access(&ents[1], gdt_code_readable | gdt_bits_executable | gdt_code_segment | gdt_bits_present);
		}

		//	kernel data (heap)
		gdt_entry_null(&ents[2]);		//	clear all bits
		gdt_set_base(&ents[2], heap_start);
		gdt_set_limit(&ents[2], (u32)((size_t)heap_end_physical - (size_t)heap_start));
		gdt_set_access(&ents[2], gdt_data_segment | gdt_data_segment | gdt_bits_present);
		gdt_set_flags(&ents[2], long_mode);

		//	kernel data (stack)
		gdt_entry_null(&ents[3]);
		/*{	//	find kernel stack entry

		}*/

		//	TSS segment (later)
		gdt_entry_null(&ents[4]);

		//gdt_load();

		if (vocality >= report_everything) {
			report("GDT initialization completed\n", note);
		}
	}

	//void gdt_load() {

	//}



#endif