//
//	hardware.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

void hardware_init() {

	size_t line = 0;
	init_phase_status_line = &line;

	if (vocality >= vocality_report_everything) {
		endl();
		line = report("nproceeding with hardware initialization\n", report_note);
	}

	kernel_status = k_state_init_hardware;

	//	initialize cpu (cpuid)
	cpu_init();

	timer_init();

	keyboard_init();

	uefi_init();
		//	uefi runtime services

	pci_init();

	init_phase_status_line = null;

	if (vocality >= vocality_report_everything) {
		report_status("SUCCESS", line, col.green);
	}
}
