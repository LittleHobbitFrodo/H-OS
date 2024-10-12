//
//	acpi.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../../integers.h"
#include "../../k_management.h"
#include "../../acpi/acpi.h"
#include "../../utils.h"

void __acpi_report(const char* msg, enum report_seriousness seriousness) {
	report("ACPI:\t", seriousness);
	print(msg);
}

void acpi_init() {

	memnull(&acpi, sizeof(acpi_t));

	vecs(&acpi.tables, sizeof(acpi_table_t));

	if (vocality >= vocality_vocal) {
		report("proceeding to initialize ACPI\n", report_warning);
	}

	if (req_rsdp.response == null) {
		report("unable to locate ACPI RSDP (pointer)\n", report_critical);
		panic(panic_code_unable_to_locate_RSDP);
	}

	acpi.rsdp = (acpi_rsdp_extended_t*)req_rsdp.response->address;
	acpi.rsdt = (acpi_rsdt_t*)((size_t)acpi.rsdp->rsdt_address);

	acpi_check();

	//	check if acpi memory map entry is present
		//	if not mark it as acpi
	{
		bool found = false;
		memmap_entry* ents = memmap.data;
		for (size_t i = 0; i < memmap.len; i++) {
			if ((ents[i].type == memmap_acpi)) {
				found = true;
				break;
			} else if (((acpi.rsdp->rsdt_address >= ents[i].base) && (acpi.rsdp->rsdt_address < ents[i].base + ents[i].len))) {
				ents[i].type = memmap_acpi;
				found = true;
			}
		}
		if (!found) {
			report("unable to pick ACPI memory map entry\n", report_critical);
			panic(panic_code_unable_to_pick_acpi_memmap_entry);
		}
	}

	acpi_query();
	//	fills acpi structure

	if (acpi.version != 1) {
		//	gather more info about acpi version
		acpi.version = acpi.fadt->header.revision;
	}

	if (vocality >= vocality_report_everything) {
		report("Power management initialization completed\n", report_note);
	}
}

bool acpi_compare(const acpi_sdt_header* h, const char* str) {
	if (strlen(str) != 4) {
		return false;
	}
	return (h->signature[0] == str[0]) && (h->signature[1] == str[1]) && (h->signature[2] == str[2]) && (h->signature[3] == str[3]);
}

void acpi_query() {
	//	find other tables

	if (acpi.rsdp->revision == 0) {
		acpi.version = 1;
	} else {
		acpi.version = 2;
	}

	size_t count = (acpi.rsdt->length - sizeof(acpi_sdt_header)) / (acpi.version == 1)? 4 : 8;

	acpi_sdt_header* header = null;
	if (acpi.version == 1) {
		u32* entries = (u32*)((size_t)acpi.rsdt + sizeof(acpi_rsdt_t));

		enum acpi_tables tp;

		for (size_t i = 0; i < count; i++) {
			header = (acpi_sdt_header*)((size_t)entries[i]);
			tp = acpi_resolve_table((const char*)&header->signature);

			if ((tp != acpi_table_unknown) && (tp != acpi_table_unsupported)) {
				acpi_table_t* table = vec_push(&acpi.tables, 1);
				table->ptr = header;
				table->type = tp;
			}
		}
	} else {
		//	acpi version 2.0+ may not be tested well
		acpi_sdt_header** entries = (acpi_sdt_header**)((size_t)acpi.rsdt + sizeof(acpi_rsdp_extended_t));
		enum acpi_tables tp;
		for (size_t i = 0; i < count; i++) {
			tp = acpi_resolve_table((const char*)&entries[i]->signature);

			if ((tp == acpi_table_unknown) || (tp == acpi_table_unsupported)) {
				continue;
			}
			acpi_table_t* table = vec_push(&acpi.tables, 1);
			table->ptr = entries[i];
			table->type = tp;

			switch (tp) {
				case acpi_table_fadt: {
					acpi.fadt = (acpi_fadt_t*)entries[i];
					break;
				}
				case acpi_table_dsdt: {
					acpi.dsdt = entries[i];
					break;
				}
				case acpi_table_facs: {
					acpi.facs = entries[i];
				}
				default: break;
			}
		}
	}
}

void acpi_check() {

	//	validate the RSDP checksum
	size_t size = (acpi.rsdp->revision == 0)? sizeof(acpi_rsdp_t) : sizeof(acpi_rsdp_extended_t);
		//	acpi version?
	u8 sum = 0;
	for (size_t i = 0; i < size; i++) {
		sum += ((u8*)acpi.rsdp)[i];
	}
	if (sum != 0) {
		__acpi_report("RSDP table validation error\n", report_critical);
		panic(panic_code_acpi_validation_failed);
	}

	//	validate the RSDT checksum
	sum = 0;
	size = acpi.rsdt->length;
	for (size_t i = 0; i < size; i++) {
		sum += ((u8*)acpi.rsdt)[i];
	}
	print("rsdt sum:\t"); printu(sum); endl();
	if (sum != 0) {
		__acpi_report("RSDT table validation error\n", report_critical);
		panic(panic_code_acpi_validation_failed);
	}

}


enum acpi_tables acpi_resolve_table(const char* signature) {
	if (strncmpb(signature, "FACP", 4)) {
		return acpi_table_fadt;
	} else if (strncmpb(signature, "DSDT", 4)) {
		return acpi_table_dsdt;
	} else if (strncmpb(signature, "FACS", 4)) {
		return acpi_table_facs;
	} else {
		return acpi_table_unknown;
	}
}