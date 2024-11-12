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

typedef struct pci_func_t {
	//	data structure to store non-volatile device information

	pci_address address;
	u8 type;
	bool multifunc;

	struct {
		u16 vendor;
		u16 id;		//	device ID
	} device;
	struct {
		u8 class;
		u8 subclass;
		u8 programming;
		u8 latency;
		pci_bist_t selftest;
		u8 cache_size;
	} info;

	void* specific;
		//	specific for each device type

} pci_func_t;
void pci_function_construct(pci_func_t* func);
__attribute__((always_inline))
inline void pci_function_destruct([[maybe_unused]] pci_func_t* func) {}
vector_type_cd(pci_funcs, pci_func_t, pci_function_vector, pci_function_construct, pci_function_destruct);



typedef struct pci_slot_t {
	pci_function_vector functions;
	u8 bus;
} pci_slot_t;
void pci_slot_construct(pci_slot_t* slot);
void pci_slot_destruct(pci_slot_t* slot);
vector_type_cd(pci_slots, pci_slot_t, pci_slot_vector_t, pci_slot_construct, pci_slot_destruct);



typedef struct pci_t {
	pci_slot_vector_t slots;
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




//	so SSD should be:
	//	class code:	0x1 -> mass storage
	//	subclass:	0x6 -> serial ATA
