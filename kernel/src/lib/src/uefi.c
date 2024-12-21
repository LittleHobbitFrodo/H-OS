//
//	uefi.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../k_management.h"

void uefi_init() {

	size_t line = 0;

	if (vocality >= vocality_report_everything) {
		line = report("checking UEFI runtime services\n", report_note);
	}

	if (req_firmware_type.response->firmware_type != LIMINE_FIRMWARE_TYPE_UEFI64) {

		#ifndef IGNORE_FIRMWARE_TYPE
		if (vocality >= vocality_report_everything) {
			report_status("CRITICAL FAILURE", line, col.critical);
		}

		report("unsupported firmware platform (", report_critical);

		switch (req_firmware_type.response->firmware_type) {
			case LIMINE_FIRMWARE_TYPE_UEFI32: {
				printl("x86 UEFI)");
				break;
			}
			case LIMINE_FIRMWARE_TYPE_X86BIOS: {
				printl("x86 BIOS)");
				break;
			}
			default: {
				printl("UNKNOWN");
				break;
			}
		}
		panic(panic_code_unsupported_firmware);
		__builtin_unreachable();
		#else
		if (vocality >= vocality_report_everything) {
			report_status("PARTIAL FAILURE", line, col.yellow);
		}
		report("unsupported firmware platform (", report_warning);
		switch (req_firmware_type.response->firmware_type) {
			case LIMINE_FIRMWARE_TYPE_UEFI32: {
				printl("x86 UEFI)");
				break;
			}
			case LIMINE_FIRMWARE_TYPE_X86BIOS: {
				printl("x86 BIOS)");
				break;
			}
			default: {
				printl("UNKNOWN");
				break;
			}
		}
		#endif
	}

	//	prepare uefi structure
	memnull(&uefi, sizeof(uefi_t));

	uefi.supported = (req_efi_system_table.response != null) && (req_efi_system_table.response->address != null);

	if (!uefi.supported) {
		if (vocality >= vocality_report_everything) {
			report_status("FAILURE", line, col.red);
		}
		report("UEFI runtime services may not be supported on this platform\n", report_warning);
		return;
	}

	uefi.system_table = (EFI_SYSTEM_TABLE*)req_efi_system_table.response->address;
	uefi.runtime = uefi.system_table->RuntimeServices;

	if (vocality >= vocality_report_everything) {
		report_status("SUCCESS", line, col.green);
	}
}

void uefi_reset(enum uefi_reset_mode mode) {
	if (uefi.supported) {
		uefi.runtime->ResetSystem((EFI_RESET_TYPE)mode, EFI_SUCCESS, 0, NULL);
		__builtin_unreachable();
	} else {
		report("UEFI firmware communication not supported on this platform\n", report_critical);
		printl("hanging system now");
		hang();
		__builtin_unreachable();
	}
}