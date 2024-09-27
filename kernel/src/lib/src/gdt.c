//
//	gdt.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../gdt.h"

void gdt_init() {
	//	flat memory model:	base must be 0, limit is ignored

	memset(&gdt, sizeof(gdt), 0);

	gdt.kernel_code.flags = (gdt.user_code.flags = 0xA0);
	gdt.kernel_data.flags = (gdt.user_data.flags = 0xA0);

	//	code segments
	gdt.kernel_code.access = (gdt.user_code.access = 0x9A);
	gdt.kernel_data.access = (gdt.user_code.access = 0x92);


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
	gdt_pointer.entries = (u64) &gdt;
	gdt_pointer.size = sizeof(gdt) - 1;

	//	load gdt pointer to the cpu
	asm volatile("lgdt %0" : : "m"(gdt_pointer));
	{
		//	update segment registers
		segment_t ds, ss;
		ds.privilege = 0;
		ds.TI = 0;
		ds.index = 2;

		ss.privilege = 0;
		ss.TI = 0;
		ss.index = 2;

		asm volatile("mov ds, %0" :: "r"(ds));
		asm volatile("mov es, %0" :: "r"(ds));
		asm volatile("mov fs, %0" :: "r"(ds));
		asm volatile("mov gs, %0" :: "r"(ds));
		asm volatile("mov ss, %0" :: "r"(ss));
		//update_cs(long_jump);
	}

	segment_t tsss;
	tsss.index = 5;
	tsss.privilege = 0;
	tsss.TI = 0;
	//	load tss segment into CPU
	tss_update(tsss);

	if (vocality >= vocality_report_everything) {
		report("legacy memory protection intialized\n", report_note);
	}
}
