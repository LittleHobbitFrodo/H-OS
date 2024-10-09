//
//	hardware.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

void hardware_init() {

	//	initialize cpu (cpuid)
	cpu_init();

	timer_init();

	keyboard_init();

	acpi_init();

	if (vocality >= vocality_report_everything) {
		report("hardware initialization success\n", report_note);
	}
}
