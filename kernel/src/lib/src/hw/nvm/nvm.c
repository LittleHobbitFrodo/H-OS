//
//	hw/nvm/nvm.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../../../hw/nvm/nvm.h"

void nvm_init() {

	if (nvm.initialzed) {
		return;
	}

	size_t line = 0;

	if (vocality >= vocality_report_everything) {
		line = report("proceeding to initialize NVM controller\n", report_note);
	}

	if (nvm.address.pci.enable == 0) {
		if (vocality >= vocality_report_everything) {
			report_status("FAILURE", line, col.red);
		}
		report("cannot find PCI address\n", report_error);
		return;
	}

	if (!nvm_memory_init()) {
		if (vocality >= vocality_report_everything) {
			report_status("FAILURE", line, col.red);
		}
		report("failed to initialize virtual address space for NVM configuration\n", report_error);
		return;
	}

	if (!nvm_version_supported()) {
		if (vocality >= vocality_report_everything) {
			report_status("FAILURE", line, col.red);
		}
		report("NVm controller version is not supported", report_error);
		return;
	}

	//	check capabilities for command setW

	//	check for support of host page size

	//	reset controller

	//	set controller config, admin queues

	//	start da controller

	//	interrupts + register handler

	//	send identify command
		//	check for IO controller

	//	reset software progress marker

	//	create IO completion and submission queue

	//	identify namespace IDs
		//	then individual namespaces
		//	check block size, and if its read only


	nvm.initialzed = true;
	nvm.used = true;

	if (vocality >= vocality_report_everything) {
		report_status("SUCCESS", line, col.green);
	}
}


bool nvm_version_supported() {
	nvm_base_register_t* reg = nvm.address.base;
	print("NVM version:\t"); printu(reg->version.major); printc('.'); printu(reg->version.minor); printc('.'); printu(reg->version.tertiary); endl();
	return true;
}

bool nvm_memory_init() {

	bool is_io;
	u64 bar = pci_read_bar(nvm.address.pci, 0, &is_io);
	if (is_io) {
		return false;
	}
	print("NVM BASE:\t"); printp((void*)bar); endl();
	union virtual_address address = {.voidptr = pages.system.pdpt.virtual};

	for (size_t i = 0; i < PAGE_COUNT; i++) {
		if (pages.system.pdpt.table[i].address == 0) {
			address.virtual_address.pdpt = i;
			goto pdpt_found;
		}
	}

	return false;

	pdpt_found:

	nvm.table = (sized_page_table*)table_alloc(&pages.heap.global, 1);
	if (nvm.table == null) {
		return false;
	}

	nvm.address.physical = align(bar, 2*MB);

	for (size_t i = 0; i < PAGE_COUNT; i++) {
		(*nvm.table)[i] = (sized_page_entry){.present = true, .write = true, .exec_disable = true, .page_size = true};
	}
	unsized_page_set_address(pages.system.pdpt.table[address.virtual_address.pdpt], table_physical(&pages.heap.global, (void*)nvm.table));

	sized_page_set_address((*nvm.table)[0], nvm.address.physical);
	sized_page_set_address((*nvm.table)[1], nvm.address.physical + (2*MB));
	nvm.address.base = (nvm.address.virtual = address.voidptr);

	return true;
}