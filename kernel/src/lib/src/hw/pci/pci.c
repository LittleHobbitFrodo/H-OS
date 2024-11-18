//
//	hw/pci/pci.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../../hw/pci/pci.h"

void pci_init() {
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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

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

			device_t* device = devices_push(1);

			switch (class) {
				case pci_device_class_mass_storage_controller: {
					print("\tmass storage controller:\t");
					device_init(device, device_type_disk);
					disk_t* disk = device->type.data;
					pci_connection_data* connect = alloc(sizeof(pci_connection_data));
					connect->address = pci_address_construct(bus, i, ii, 0, true);

					pci_device_header header;
					u32* ptr = (u32*)&header;
					for (size_t iii = 0; iii < sizeof(pci_device_header)/sizeof(u32); iii++) {
						*ptr = pci_read(bus, i ,ii, iii);
					}

					connect->vendor = header.vendor;
					connect->device_id = header.device;
					connect->class = header.class;
					connect->subclass = header.subclass;
					connect->programming = header.programming;
					connect->cache = header.cache_size;
					connect->latency = header.latency;
					connect->header_type = header.header_type;
					connect->test = header.bist;

					disk->connect.data = connect;
					switch (subclass) {
						case pci_mass_storage_serial_ata: {
							print("SATA:\t");
							switch (programming) {
								case pci_serial_ata_vendor_specific: {
									printl("\tvendor specific");
									disk->connect.type = disk_connect_unsupported | disk_connect_pci;
									break;
								}
								case pci_serial_ata_ahci_1_0: {
									printl("AHCI");
									disk->connect.type = disk_connect_ahci | disk_connect_pci;
									break;
								}
								case pci_serial_ata_serial_storage_bus: {
									printl("\tserial storage");
									disk->connect.type = disk_connect_unsupported | disk_connect_pci;
									break;
								}
								default: {
									print("unknown (0x"); printh(programming); printl(")");
									disk->connect.type = disk_connect_undefined | disk_connect_pci;
									break;
								}
							}
							break;
						}
						case pci_mass_storage_nvm_controller: {
							printl("NVM");
							2break;
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

#pragma GCC diagnostic pop

u8 pci_enumerate() {
	for (u16 i = 0; i < 256; i++) {
		pci_scan_bus((u8)i);
	}

	return ok;
}