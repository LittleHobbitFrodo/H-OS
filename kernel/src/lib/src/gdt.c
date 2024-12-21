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

	gdt.kernel_code.access = (gdt.user_code.access = 0x9A);
	gdt.kernel_data.access = (gdt.user_code.access = 0x92);

	gdt.code.access = (gdt.user_code.access = 0x9A);
	gdt.code.flags = (gdt.user_data.flags = 0xA0);


	//	tss
	gdt_tss_set_base(&gdt.tss, &tss.tss);
	gdt_tss_set_limit(&gdt.tss, sizeof(tss_base_t) - 1);
	gdt_tss_set_flags(&gdt.tss, 0);
	gdt.tss.access = 0x89;

	gdt_update();
}

void gdt_update() {

	size_t line = 0;

	if (vocality >= vocality_report_everything) {
		line = report("configuring legacy memory protection\n", report_note);
	}

	//	interrupts are disabled in init.asm

	//	prepare gdt pointer
	gdt_pointer.entries = (u64) &gdt;
	gdt_pointer.size = sizeof(gdt) - 1;

	//	load gdt pointer to the cpu
	asm volatile("lgdt %0" : : "m"(gdt_pointer));

	update_segment_registers();

	segment_t tsss;
	tsss.index = 6;
	tsss.privilege = 0;
	tsss.TI = 0;
	//	load tss segment into CPU
	tss_update(tsss);

	if (vocality >= vocality_report_everything) {
		report_status("SUCCESS", line, col.green);
	}

}
