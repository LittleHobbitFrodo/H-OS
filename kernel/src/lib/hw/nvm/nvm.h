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

	struct { ;
		pci_address pci;
		nvm_base_register_t *base;	//	base register virtual address

		size_t physical;		//	start of the physical address space
		void* virtual;			//	start of virtual address space

		size_t size;	//	address space size

	} address;

	sized_page_table* table;

	disk_vector disks;

	struct {
		nvm_submission_entry* submission;
		nvm_completion_entry* completion;
		size_t max_size;
	} queue;

} nvm_t;

nvm_t nvm = {0};

void nvm_init();

static bool nvm_version_supported();

static bool nvm_memory_init();




//	admin commands
#define NVM_CMD_ADMIN_CREATE_SUBMISSION_QUEUE 0x01
#define NVM_CMD_ADMIN_CREATE_COMPLETION_QUEUE 0x05
#define NVM_CMD_ADMIN_IDENTIFY 0x06

//	IO commands
#define NVM_CMD_IO_READ 0x02
#define NVM_CMD_IO_WRITE 0x01

