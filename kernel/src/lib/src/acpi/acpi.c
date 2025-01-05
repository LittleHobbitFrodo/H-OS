//
//	acpi.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../../integers.h"
#include "../../k_management.h"
#include "../../acpi/acpi.h"
#include "../../utils.h"

//	acpi is being deprecated (version 0.1.3.6 | this goofy implementation is no use anyway)

/*void __acpi_report(const char* msg, enum report_seriousness seriousness) {
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

	acpi_hypervisor_detect();

	if (acpi.version != 1) {
		//	gather more info about acpi version
		acpi.version = acpi.fadt->header.revision;
	}

	if (vocality >= vocality_report_everything) {
		report("Power management initialization completed\n", report_note);
	}
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
		}
	}

	if (acpi.tables.data == null) {
		return;
	}

	acpi_table_t* table = null;
	for (size_t i = 0; i < acpi.tables.len; i++) {
		table = ((acpi_table_t*)vec_at(&acpi.tables, i));
		switch (table->type) {
			case acpi_table_fadt: {
				acpi.fadt = (acpi_fadt_t*)table->ptr;
				if (acpi.version == 1) {
					acpi.facs = (acpi_sdt_header*)((size_t)acpi.fadt->firmware_ctrl_ptr);
					acpi.dsdt = (acpi_sdt_header*)((size_t)acpi.fadt->dsdt_ptr);
				} else {
					acpi.facs = (acpi_sdt_header*)acpi.fadt->x_firmware_ctrl_ptr;
					acpi.dsdt = (acpi_sdt_header*)acpi.fadt->x_dsdt_ptr;
				}
				break;
			}
			case acpi_table_dsdt: {
				acpi.dsdt = table->ptr;
				break;
			}
			case acpi_table_facs: {
				acpi.facs = table->ptr;
				break;
			}
			case acpi_table_bgrt: {
				acpi.bgrt = (acpi_bgrt_t*)table->ptr;
			}
			default: break;
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

void acpi_hypervisor_detect() {
	//	detects hypervisor and stores it in cpu structure (cpu.h)

	if (strncmpb((const char*)&acpi.fadt->header.oemid, "QEMU", 4)) {
		cpu.hypervisor = hypervisor_qemu;
		if (vocality >= vocality_vocal) {
			report("Late hypervisor detection: QEMU\n", report_note);
		}
	} else if (strncmpb((const char*)&acpi.fadt->header.oemid, "BOCHS", 5)) {
		cpu.hypervisor = hypervisor_bochs;
		if (vocality >= vocality_vocal) {
			report("Late hypervisor detection: BOCHS\n", report_note);
		}
	} else {
		cpu.hypervisor = hypervisor_none;
	}

}*/