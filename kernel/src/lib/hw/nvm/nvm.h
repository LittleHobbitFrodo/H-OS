//
//	hw/nvm/nvm.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../../integers.h"
#include "./structures.h"

//	simple NVM controller driver

typedef struct nvm_t {
	u8 used:		1;
	u8 initialzed:	1;

	pci_address pci_address;
	nvm_base_register_t* base;

	disk_vector disks;

	struct {
		nvm_submission_entry* submission;
		nvm_completion_entry* completion;
		size_t max_size;
	} queue;

	page_table_t* table;


} nvm_t;

static nvm_t nvm = {0};

static void nvm_init();


//	reading dwords is neccesary
__attribute__((always_inline))
inline u32 nvm_read(const void* const address) {
	return *((u32*)address);
}

__attribute__((always_inline))
inline u64 nvm_read64(const void* const address) {
	return (u64)(*((u32*)address)) | (((u64)((u32*)((size_t)address + sizeof(u32)))) << 32);
}

static inline void nvm_fill(void* structure, void* address, size_t size) {
	size /= sizeof(u32);
	for (size_t i = 0; i < size; i++) {
		*((u32*)structure + i) = *((u32*)address + i);
	}
}

__attribute__((always_inline))
inline void nvm_write(void* const address, const u32 val) {
	*((u32*)address) = val;
}

__attribute__((always_inline))
inline void nvm_write64(void* const address, const u64 val) {
	*((u32*)address) = (u32)val;
	*((u32*)address+1) = (u32)(val >> 32);
}

static inline void nvm_flush(void* structure, void* address, size_t size) {
	size /= sizeof(u32);
	for (size_t i = 0; i < size; i++) {
		*((u32*)address + i) = *((u32*)structure + i);
	}
}