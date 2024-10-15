//
//	uefi.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../k_management.h"

void uefi_init() {

	//	prepare uefi structure
	memnull(&uefi, sizeof(uefi_t));

	uefi.supported = (req_efi_system_table.response != null) && (req_efi_system_table.response->address != null);

	if (!uefi.supported) {
		report("UEFI runtime services may not be supported on this platform\n", report_warning);
		return;
	}
	#ifdef KERNEL_DEBUG
	else {
		report("UEFI seems to be supported on this platform\n", report_debug);
	}
	#endif

	uefi.system_table = (EFI_SYSTEM_TABLE*)req_efi_system_table.response->address;
	uefi.runtime = uefi.system_table->RuntimeServices;

	if (vocality >= vocality_report_everything) {
		report("UEFI firmware communication has been successfully initialized\n", report_note);
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