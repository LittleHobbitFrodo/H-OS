//
//	hw/pci/pci.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../integers.h"
#include "./structures.h"
#include "../../vector/vector.h"
#include "../../string.h"

#define PCI_CONFIG_ADDRESS 0xCF8
	//	32bit register
#define PCI_CONFIG_DATA 0xCFC

typedef struct pci_general_device_t {
	//	data structure to store non-volatile device information

	pci_address address;
	pci_header_type type;
	struct {
		u8 class;
		u8 subclass;
		u8 programming;
	} class;
	pci_bist_t selftest;

	struct {
		u16 vendor;
		u16 device;
	} device;

	u8 revision;
	u8 cache_size;
	u8 latency;

	void* specific;


} pci_general_device_t;
void pci_general_dev_construct(pci_general_device_t* func);
__attribute__((always_inline))
inline void pci_general_dev_destruct([[maybe_unused]] pci_general_device_t* func) {}
vector_type_cd(pci_devices, pci_general_device_t, pci_device_vector, pci_general_dev_construct, pci_general_dev_destruct);



typedef struct pci_t {
	pci_device_vector devices;
	bool supported;
} pci_t;

static pci_t pci;


static void pci_init();

u32 pci_read(u8 bus, u8 slot, u8 function, u8 offset);

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



//	so SSD should be:
	//	class code:	0x1 -> mass storage
	//	subclass:	0x6 -> serial ATA
