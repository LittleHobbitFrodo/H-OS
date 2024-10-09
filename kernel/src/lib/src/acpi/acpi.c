//
//	acpi.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../../k_management.h"
#include "../../acpi/acpi.h"

void acpi_init() {

	memnull(&acpi, sizeof(acpi_t));

	if (vocality >= vocality_vocal) {
		report("proceeding to initialize ACPI\n", report_warning);
	}

	if (req_rsdp.response == null) {
		report("unable to locate ACPI RSDP (pointer)\n", report_critical);
		panic(panic_code_unable_to_locate_RSDP);
	}

	acpi.rsdp = (acpi_rsdp_extended_t*)req_rsdp.response->address;

	printl("acpi info:");
	print("signature:\t"); printl((const char*)&acpi.rsdp->signature);
	print("oem ID:\t"); printl((const char*)&acpi.rsdp->oemid);
	print("revision:\t"); printu(acpi.rsdp->revision); endl();
	print("rsdt address:\t"); printp((void*)((size_t)acpi.rsdp->rsdt_address)); endl();

	//	check if acpi memory map entry is present
		//	if not mark it as acpi
	{
		bool found = false;
		memmap_entry* ents = memmap.data;
		for (size_t i = 0; i < memmap.len; i++) {
			if ((ents[i].type == memmap_acpi)) {
				found = true;
				break;
			}
		}
		if (!found) {
			if (vocality >= vocality_vocal) {
				report("unable to locate ACPI memory map entry -> resolving manually\n", report_warning);
			}
			for (size_t i = 0; i < memmap.len; i++) {
				if (((acpi.rsdp->rsdt_address >= ents[i].base) && (acpi.rsdp->rsdt_address < ents[i].base + ents[i].len))) {
					ents[i].type = memmap_acpi;
					found = true;
					if (vocality >= vocality_vocal) {
						report("ACPI memory map entry resolved\n", report_note);
					}
					break;
				}
			}
		}
		if (!found) {
			report("unable to pick ACPI memory map entry\n", report_critical);
			panic(panic_code_unable_to_pick_acpi_memmap_entry);
		}
	}

	acpi.version = acpi_detect_version();

}


u32 acpi_detect_version() {
	//	detects acpi version
	if (acpi.rsdp->revision == 0) {
		return 1;
	}
}