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

	//smbios.entry_point = (smbios_entry_point_t*)((size_t)req_smbios.response->entry_64 + (size_t)pages.hhdm);
	smbios.entry_point = req_smbios.response->entry_64;
	req_smbios.response = null;

	if (!strncmpb(smbios.entry_point->anchor, "_SM3_", 5)) {
		__smbios_report("invalid entry point\n", report_error);
		smbios.supported = false;
		return;
	}

	if ((void*)smbios.entry_point->table_base == null) {
		__smbios_report("cannot find table address\n", report_error);
		smbios.supported = false;
		return;
	}

	smbios.structure_table = (smbios_header_t*)(smbios.entry_point->table_base + (size_t)pages.hhdm);

	print("smbios entry point:\t"); printp(smbios.entry_point); endl();
	print("smbios entry point size:\t"); printu(smbios.entry_point->length); endl();
	print("smbios structure table:\t"); printp(smbios.structure_table); endl();
	wait(500);
	print("smbios table type:\t"); printu(smbios.structure_table->type); endl();

	smbios_header_t* h = smbios.structure_table;
	size_t i = 0;
	for (; (h->type != smbios_header_end) && ((size_t)h < (size_t)smbios.entry_point + smbios.entry_point->length); i++) {
		//print("header["); printu(i); print("]:\t"); printp(h); print(":\t"); printu(h->type); endl();
		h = smbios_next_header(h);
		if ((h->type != 0) || (h->handle != 0) || (h->len != 0)) {

		}

	}
	print("there are "); printu(i+1); printl(" smbios tables");


}

size_t smbios_table_length(smbios_header_t* header) {
	size_t i = 0;
	const char* str = (const char*)header + header->len;
	for (; !(str[i] == '\0' && str[i+1] == '\0'); i++);
	return header->len + i + 2;
}

smbios_header_t* smbios_next_header(smbios_header_t* header) {
	size_t i = 1;
	const char* str = (const char*)header + header->len;
	for (; !(str[i-1] == '\0' && str[i] == '\0'); i++);
	return (smbios_header_t*)((size_t)header + header->len + i + 1);
}