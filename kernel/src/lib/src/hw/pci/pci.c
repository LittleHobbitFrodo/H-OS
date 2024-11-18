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
	address.address.zero = 0;
	address.address.offset = offset;
	address.address.function = function;
	address.address.slot = slot;
	address.address.bus = bus;
	address.address.reserved = 0;
	address.address.enable = 1;

	outd(PCI_CONFIG_ADDRESS, address.u32);

	return ind(PCI_CONFIG_DATA);
}

/*void pci_scan_bus(u8 bus) {

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

			pci_device_header header __attribute__((uninitialized));
			u32* ptr = (u32*)&header;
			for (size_t iii = 0; iii < sizeof(pci_device_header) / sizeof(u32); iii++) {
				*ptr = pci_read(bus, i, ii, iii);
				ptr++;
			}
			func->type = header.header_type.header_type;
			func->multifunc = header.header_type.multifunc;
			func->device.id = header.device;
			func->device.vendor = header.vendor;
			func->info.class = header.class;
			func->info.subclass = header.subclass;
			func->info.latency = header.latency;
			func->info.programming = header.programming;
			func->info.selftest = header.bist;
			printc('\t'); printu(i); print(" : "); printu(ii); printc('\t');
			switch (func->info.class) {
				case pci_device_class_mass_storage_controller: {
					switch (func->info.subclass) {
						case pci_mass_storage_nvm_controller: {
							printl("non-volatile memory controller");
							break;
						}
						case pci_mass_storage_serial_ata: {
							printl("serial ATA");
							break;
						}
						default: {
							printl("unknown (mass storage)");
							break;
						}
					}
					break;
				}
				case pci_device_class_base_system_peripheral: {
					switch (func->info.subclass) {
						case pci_base_peripheral_timer: {
							printl("timer");
							break;
						}
						case pci_base_peripheral_pic: {
							printl("PIC");
							break;
						}
						case pci_base_peripheral_rtc_controller: {
							printl("RTC controller");
							break;
						}
						default: {
							printl("unknown (base peripheral)");
							break;
						}
					}
					break;
				}
				case pci_device_class_unclassified: {
					print("unclassified ("); printh(func->info.subclass); printl(")");
					break;
				}
				default: {
					printl("unknown");
					break;
				}
			}

		}
		slot = null;
	}
}*/

void pci_scan_bus(u8 bus) {

	for (size_t i = 0; i < 8; i++) {
		for (size_t ii = 0; ii < 8; ii++) {
			if (!pci_exists(bus, i , ii)) {
				continue;
			}

			printu(i); printc('\t'); printu(ii); printc('\t');
			u8 class = pci_read_class(bus, i, ii);
			u8 subclass = pci_read_subclass(bus, i, ii);
			u8 programming = pci_read_programming(bus, i, ii);
			output.color = col.blue;
			output.color = col.white;
			switch (class) {
				case pci_device_class_mass_storage_controller: {
					print("\tmass storage controller:\t");
					switch (subclass) {
						case pci_mass_storage_serial_ata: {
							print("SATA:\t");
							switch (programming) {
								case pci_serial_ata_vendor_specific: {
									printl("\tvendor specific");
									break;
								}
								case pci_serial_ata_ahci_1_0: {
									printl("AHCI");
									break;
								}
								case pci_serial_ata_serial_storage_bus: {
									printl("\tserial storage");
									break;
								}
								default: {
									print("unknown (0x"); printh(programming); printl(")");
									break;
								}
							}
							break;
						}
						case pci_mass_storage_nvm_controller: {
							printl("NVM");
							break;
						}
						default: {
							print("unknown (0x"); printh(subclass); printl(")");
							break;
						}
					}
					break;
				}
				case pci_device_class_processor: {
					printl("processor");
					break;
				}
				case pci_device_class_bridge: {
					printl("bridge");
					break;
				}
				default: {
					print("\tunknown:\t"); printh(subclass); endl();
					break;
				}
			}
		}
	}
}

u8 pci_enumerate() {

	for (u16 i = 0; i < 256; i++) {
		pci_scan_bus((u8)i);
	}

	return ok;
}

void pci_general_dev_construct(pci_general_device_t* device) {
	u32* ptr = (u32*)&device->address;
	*ptr = 0;
	device->type = (pci_header_type){.header_type = 0, .multifunc = false};
	device->class.class = 0;
	device->class.subclass = 0;
	device->class.programming = 0;
	device->selftest = (pci_bist_t){.capable = false, .code = 0, .reserved = 0, .start = 0};
	device->device.device = 0;
	device->device.vendor = 0;
	device->revision = 0;
	device->cache_size = 0;
	device->latency = 0;
}