//
//	hw/pci/pci.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../integers.h"
#include "./structures.h"
#include "../../vector/vector.h"
#include "../devices/disk.h"
#include "../devices/device.h"

#define PCI_CONFIG_ADDRESS 0xCF8
	//	32bit register
#define PCI_CONFIG_DATA 0xCFC


//	initializes PCI
static void pci_init();

void* pci_read_bar(pci_address address, u8 bar);

static bool pci_initialized = false;

u32 pci_read(u8 bus, u8 slot, u8 function, u8 offset);
__attribute__((always_inline))
inline u32 pci_reada(union pci_address_u32 address) {
	outd(PCI_CONFIG_ADDRESS, address.u32);
	return ind(PCI_CONFIG_DATA);
}
inline void pci_writea(union pci_address_u32 address, u32 data) {
	outd(PCI_CONFIG_ADDRESS, address.u32);
	outd(PCI_CONFIG_DATA, data);
}

u8 pci_enumerate();

void pci_scan_bus(u8 bus);

__attribute__((always_inline))
inline bool pci_exists(u8 bus, u8 slot, u8 function) {
	return (pci_read(bus, slot, function, 0) & 0xffff) != 0xffff;
}

__attribute__((always_inline))
inline u8 pci_read_class(u8 bus, u8 slot, u8 function) {
	return (pci_read(bus, slot, function, 2) >> 24) & 0xff;
}
__attribute__((always_inline))
inline u8 pci_read_subclass(u8 bus, u8 slot, u8 function) {
	return (pci_read(bus, slot, function, 2) >> 16) & 0xff;
}
__attribute__((always_inline))
inline u8 pci_read_programming(u8 bus, u8 slot, u8 function) {
	return (pci_read(bus, slot, function, 2) >> 8) & 0xff;
}

__attribute__((always_inline))
inline pci_address pci_address_construct(u8 bus, u8 slot, u8 func, u8 offset, bool enable) {
	return (pci_address){.zero = 0, .bus = bus, .slot = slot, .function = func, .offset = offset, .reserved = 0, .enable = enable};
}


typedef struct pci_connection_data {
	pci_address address;
	u16 vendor;
	u16 device_id;
	u8 class;
	u8 subclass;
	u8 programming;
	u8 cache;
	u8 latency;
	pci_header_type header_type;
	pci_bist_t test;
} pci_connection_data;

typedef struct pci_discovery_data {
	pci_address address;
	u8 class;
	u8 subclass;
	u8 programming;
	pci_bist_t test;
} pci_discovery_data;
