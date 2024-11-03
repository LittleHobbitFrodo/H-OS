//
//	smbios.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../hw/smbios.h"

__attribute__((always_inline)) inline
void __smbios_report(const char* msg, enum report_seriousness seriousness) {
	report("System management BIOS:\t", seriousness);
	print(msg);
}

void smbios_init() {

	report("proceeding to initialize smbios\n", report_warning);


	memnull(&smbios, sizeof(smbios_t));
	vecs(&smbios.tables, sizeof(smbios_tableptr_t));

	smbios.supported = (req_smbios.response != null) && (req_smbios.response->entry_64 != null);

	if (!smbios.supported) {
		__smbios_report("structure cannot be found\n", report_error);
		return;
	}

	smbios.entry_point = (smbios_entry_point_t*)((size_t)req_smbios.response->entry_64 + (size_t)pages.hhdm);

	if (!strncmpb((const char*)&smbios.entry_point->anchor, "_SM3_", 5)) {
		__smbios_report("invalid entry point\n", report_error);
		smbios.supported = false;
		return;
	}

	if ((void*)smbios.entry_point->table_base == null) {
		__smbios_report("cannot find table address\n", report_error);
		smbios.supported = false;
		return;
	}


	/*memmap_display();
	print("hhdm:\t"); printp((void*)req_page_hhdm.response->offset); endl();

	print("tabe_base:\t"); printp((void*)smbios.entry_point->table_base); endl();
	smbios.structure_table = (smbios_header_t*) smbios.entry_point->table_base;
	printl("structure_table:");
	print("type:\t"); printu(smbios.structure_table->type); endl();
	print("len:\t"); printu(smbios.structure_table->len); endl();
	print("handle:\t"); printu(smbios.structure_table->handle); endl();*/

	//	enumerate tables
	/*smbios_header_t* i = smbios.structure_table;
	size_t len;
	for (;;) {
		len = smbios_table_length(i);

		if (len < sizeof(smbios_header_t)) {
			print("table len:\t"); printu(i->len); print(" : "); printu(sizeof(smbios_header_t)); endl();
			print("len:\t"); printu(len); endl();
			printl("Error: Invalid table length. Exiting.");
			break;
		}

		smbios_tableptr_t* new = vec_push(&smbios.tables, 1);
		new->type = i->type;
		new->header = i;
		i = (smbios_header_t*)((size_t)i + len);

		if (i->type == smbios_header_end) {
			printl("end");
			break;
		}

	}*/


}

size_t smbios_table_length(smbios_header_t* header) {
	size_t i = 1;
	const char* str = (const char*)header + header->len;
	for (; (str[i-1] != '\0') && (str[i] != '\0'); i++);
	return header->len + i + 1;
}