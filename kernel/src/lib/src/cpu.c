//
//	cpu.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

//	TODO: add limine multiprocessor feature

#include "../cpu.h"

void cpu_info() {
	printl("CPU info:");
	print("\tvendor:\t");
	printl(cpu.vendor->name);
	print("\tmodel:\t");
	printl((const char *) &cpu.model);
	switch (cpu.vendor->environment) {
		case environment_hardware: {
			printl("running on real hardware");
			break;
		}
		case environment_emulator: {
			print("running under "); print(cpu.vendor->name); printl(" emulator");
			break;
		}
		case environment_vm: {
			print("running in "); print(cpu.vendor->name); printl(" vm");
			break;
		}
		default: break;
	}
}

void cpu_init() {

	size_t line = 0;

	if (vocality >= vocality_report_everything) {
		line = report("gathering information about CPU\n", report_note);
	}

	memnull(&cpu, sizeof(cpu_t));

	cpu.vendor = null;

	//	detect vendor
	u32 data[3];
	asm volatile("cpuid" : "=b"(data[0]), "=d"(data[1]), "=c"(data[2]) : "a"(0));
	char str[13];
	const char *s = (const char *) &str;
	memcpy((void *) &data, &str, 12);
	str[12] = '\0';

	for (size_t i = 0; i < cpu_vendor_count; i++) {
		if (strcmpb(cpu_vendors[i].id, s)) {
			cpu.vendor = (cpu_vendor_t *)&(cpu_vendors[i]);
			break;
		}
	}

	if (cpu.vendor == null) {
		if (vocality >= vocality_report_everything) {
			report_status("PARTIAL FAILURE", line, col.yellow);
		}
		report("could not find CPU vendor", report_warning);
	}

	//	detect cpu model name

	u32 txt[4];

	//	first 16 characters-smbios type=1,manufacturer=QEMU
	asm volatile("cpuid" : "=a"(txt[0]), "=b"(txt[1]), "=c"(txt[2]), "=d"(txt[3]) : "a"(0x80000002));
	memcpy((void *) &txt, (void *) &cpu.model, 4 * sizeof(u32));

	asm volatile("cpuid" : "=a"(txt[0]), "=b"(txt[1]), "=c"(txt[2]), "=d"(txt[3]) : "a"(0x80000003));
	memcpy((void *) &txt, (void *) ((size_t) &cpu.model + sizeof(u32) * 4), 4 * sizeof(u32));

	cpu.model[sizeof(u32) * 8] = '\0';

	asm volatile("cpuid" : "=a"(txt[0]), "=b"(txt[1]), "=c"(txt[2]), "=d"(txt[3]) : "a"(0x80000004));
	memcpy((void *) &txt, (void *) ((size_t) &cpu.model + sizeof(u32) * 8), 4 * sizeof(u32));
	cpu.model[48] = '\0';

	if ((vocality >= vocality_normal) && (cpu.vendor->environment != environment_hardware)) {
		report("The OS is running under ", report_note);
		print(cpu.vendor->name);
		switch (cpu.vendor->environment) {
			case environment_vm: {
				printl(" hypervisor");
				break;
			}
			case environment_emulator: {
				printl(" emulator");
				break;
			}
			default: break;
		}
	}

	if (vocality >= vocality_report_everything) {
		report_status("SUCCESS", line, col.green);
	}
}
