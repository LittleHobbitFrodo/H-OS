//
//	hw/pci/pci.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../../hw/pci/pci.h"

void pci_init() {
	memnull(&pci, sizeof(pci_t));

	u8 tmp;
	if ((tmp = pci_enumerate()) != ok) {
		report("pci device enumeration failed (code: ", report_error);
		printu(tmp); printl(")");
		hang();
		__builtin_unreachable();
	}


}

u32 pci_read(u8 bus, u8 slot, u8 function, u8 offset) {

	union pci_address_u32 address;
	address.address.offset = offset & ~0b11;
	address.address.function = function;
	address.address.slot = slot;
	address.address.bus = bus;
	address.address.reserved = 0;
	address.address.enable = 1;

	outd(PCI_CONFIG_ADDRESS, address.u32);

	return ind(PCI_CONFIG_DATA);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

void pci_scan_bus(u8 bus) {
	pci_slot_t* slot = null;
	pci_func_t* func = null;
	for (u8 i = 0; i < 8; i++) {
		for (u8 ii = 0; ii < 8; ii++) {
			if (!pci_exists(bus, i, ii)) {
				continue;
			}
			if (slot == null) {
				slot = pci_slots_push(&pci.slots, 1);
				slot->bus = bus;
			}
			func = pci_funcs_push(&slot->functions, 1);

			pci_device_header header;
			u32* ptr = (u32*)&header;
			for (size_t iii = 0; iii < sizeof(pci_device_header) / sizeof(u32); iii++) {
				*ptr = pci_read(bus, i, ii, iii);
			}

			func->type = header.header_type.header_type;
			func->multifunc = header.header_type.multifunc;
			func->device.id = header.device;
			func->device.vendor = header.vendor;
			func->info.class = header.class;
			func->info.subclass = header.subclass;
			func->info.latency = header.latency_timer;
			func->info.programming = header.prog_interface;
			func->info.selftest = header.bist;

		}
		slot = null;
	}

}

#pragma GCC diagnostic pop

u8 pci_enumerate() {

	for (u8 i = 0; i < 8; i++) {
		pci_scan_bus(0);
	}

	return ok;
}

void pci_slot_construct(pci_slot_t* slot) {
	pci_funcs_construct(&slot->functions, 0);
	slot->bus = 0;
}

void pci_slot_destruct(pci_slot_t* slot) {
	pci_funcs_destruct(&slot->functions);
}

void pci_function_construct(pci_func_t* func) {
	u32* ptr = (u32*)((size_t)&func->address);
	*ptr = 0;
	func->type = 0;
	func->multifunc = false;
	func->device.vendor = 0;
	func->device.id = 0;
	func->info.class = 0;
	func->info.subclass = 0;
	func->info.programming = 0;
	func->info.latency = 0;
	u8* ptr2 = (u8*)&func->info.selftest;
	*ptr2 = 0;
	func->specific = null;
}



