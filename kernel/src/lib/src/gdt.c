//
//	gdt.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifndef H_OS_LIB_GDT_C
	#define H_OS_LIB_GDT_C

	void gdt_init() {


		for (size_t i = 0; i < 4; i++) {
			gdt_null(&((gdt_entry*)&gdt)[i]);
		}
		{	//	kernel code
			memmap_entry* ent = null;
			for (size_t i = 0; i < memmap.len; i++) {
				if ((ent = ((memmap_entry*)vec_at(&memmap, i)))->type == memmap_kernel) {
					break;
				}
			}
			if (ent == null) {
				report("could not find kernel memory map entry\n", rs_critical);
				panic(pc_gdt_initialization_failure);
			}
			gdt_set_base(&gdt.code, (void*)ent->base);
			gdt_set_limit(&gdt.code, (u32)ent->len);
			gdt_set_flags(&gdt.code, 0xA);
			gdt.code.access = 0x9A;
		}

		//	heap
		gdt_set_base(&gdt.heap, heap_start);
		gdt_set_limit(&gdt.heap, (size_t)heap_end_physical - (size_t)heap_start);
		gdt_set_flags(&gdt.heap, 0x92);
		gdt.heap.access = 0xC;

		//	tss
		gdt_tss_set_base(&gdt.tss, &tss.tss);
		gdt_tss_set_limit(&gdt.tss, sizeof(tss_base_t) - 1);
		gdt_tss_set_flags(&gdt.tss, 0);
		gdt.tss.access = 0x89;

		gdt_update();

	}

	void gdt_update() {
		//	interrupts are disabled in init.asm

		//	prepare gdt pointer
		gdt_pointer.entries = &gdt;
		gdt_pointer.size = sizeof(gdt) - 1;

		//	load gdt pointer to the cpu
		asm volatile("lgdt %0" : : "m"(gdt_pointer));
		{	//	update segment registers
			segment_t ds, ss;
			*((u16*)&ds) = 0;
			*((u16*)&ss) = 0;
			ds.index = 2;
			ss.index = 1;

			asm volatile("mov %0, ds" :: "r"(ds));
			asm volatile("mov %0, es" :: "r"(ds));
			asm volatile("mov %0, fs" :: "r"(ds));
			asm volatile("mov %0, gs" :: "r"(ds));
			asm volatile("mov %0, ss" :: "r"(ss));

		}

		segment_t tsss;
		*((u16*)&tsss) = 0;
		tsss.index = 3;
		//	load tss segment into CPU
		tss_update(tsss);

		if (vocality >= vocality_report_everything) {
			report("GDT loaded successfully\n", rs_note);
		}
	}

#endif