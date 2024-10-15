//
//	hardware.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

void hardware_init() {

	kernel_status = k_state_init_hardware;

	//	initialize cpu (cpuid)
	cpu_init();

	timer_init();

	keyboard_init();

	uefi_init();
		//	uefi runtime services

	//acpi_init();

	if (vocality >= vocality_report_everything) {
		report("hardware initialization success\n", report_note);
	}
}
