//
//	hw/nvm/nvm.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../../../hw/nvm/nvm.h"

void nvm_init() {

	report("proceeding to initialize NVM controller\n", report_warning);
	wait(750);

	if (nvm.initialzed) {
		return;
	}

	if (nvm.pci_address.enable == 0) {
		report("NVM init: cannot find PCI address | initialization halted\n", report_error);
		return;
	}

	//	read PCI base memory register 0
	nvm.base = pci_read_bar(nvm.pci_address, 0);

	//	map 2mb page
	nvm.table = page_alloc(1);
	print("nvm.table:\t"); printp(nvm.table); endl();
	wait(1500);
	{
		u64* ptr = (u64*)nvm.table;
		for (size_t i = 0; i < 512; i++) {
			*ptr++ = 1;	//	present bit is on
		}
	}
	//	map it to itself
	(*nvm.table)[0].address = ((size_t)nvm.table - pages.hhdm) >> PAGE_SHIFT;

	//	map second entry to nvm config space
	(*nvm.table)[1].address = ((size_t)nvm.base - pages.hhdm) >> PAGE_SHIFT;

	//	find unused pml4 page
	for (size_t i = 511; i > 255; i--) {
		if ((*pages.pml4)[i].address == 0) {
			(*pages.pml4)[i].address = ((size_t)nvm.table - pages.hhdm) >> PAGE_SHIFT;
		}
	}

	if (nvm.base == null) {
		report("NVM controller initialization: cannot get memory-mapped IO (null)\n", report_error);
		return;
	}

	print("nvm base:\t\t"); printp((void*)((size_t)nvm.base - (size_t)pages.hhdm)); endl();
	memmap_display();
	memmap_display_original();

	print("NVM controller version:\t"); printu(nvm.base->version.major); printc('.'); printu(nvm.base->version.minor); endl();

	if (vocality >= vocality_report_everything) {
		report("NVM controller initialization succeeded\n", report_note);
	}
}
