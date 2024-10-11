//
//	acpi.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../../k_management.h"
#include "../../acpi/acpi.h"

#include "../../k_management.h"

void acpi_init() {

	memnull(&acpi, sizeof(acpi_t));

	vecs(&acpi.tables, sizeof(void*));

	if (vocality >= vocality_vocal) {
		report("proceeding to initialize ACPI\n", report_warning);
	}

	if (req_rsdp.response == null) {
		report("unable to locate ACPI RSDP (pointer)\n", report_critical);
		panic(panic_code_unable_to_locate_RSDP);
	}

	acpi.rsdp = (acpi_rsdp_extended_t*)req_rsdp.response->address;
	acpi.rsdt = (acpi_rsdt_t*)((size_t)acpi.rsdp->rsdt_address);

	//acpi_connect();

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
						report("ACPI memory map entry resolved:", report_note);
						printu(i); endl();
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

}

void __acpi_report(const char* msg, enum report_seriousness seriousness) {
	report("ACPI:\t", seriousness);
	print(msg);
}

bool acpi_compare(const acpi_sdt_header* h, const char* str) {
	if (strlen(str) != 4) {
		return false;
	}
	return (h->signature[0] == str[0]) && (h->signature[1] == str[1]) && (h->signature[2] == str[2]) && (h->signature[3] == str[3]);
}