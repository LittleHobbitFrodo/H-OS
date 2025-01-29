//
//	hw/nvm/nvm.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

typedef struct nvm_doorbell_registers {
	u32 submission_tail;
	u32 completion_head;
} __attribute__((packed)) nvm_doorbell_registers;
#define NVM_DOORBELL_OFFSET 0x1000


#include "../../integers.h"
#include "./command.h"
#include "./queue.h"
#include "./base-register.h"
#include "./enums.h"

//	simple NVM controller driver

//	physical region page is aligned to 4 => can be allocated on regular heap

typedef struct nvm_t {

	u8 used:		1;
	u8 initialzed:	1;

	size_t max_queue_size;

	struct {
		pci_address pci;

		size_t physical;		//	start of the physical address space
		void* virtual;			//	start of virtual address space

		size_t size;	//	address space size

	} address;

	nvm_base_register_t* controller;
	u16 cmdid;	//	cmdID (unique for each command)
		//	volatile?

	sized_page_table* table;

	disk_vector disks;

	struct {
		//	obtained from nvm_check_capabilities()
		u16 max_entries;

		u16 stride:				4;
		u32 page_size_max:	4;
		u32 page_size_min:	4;
		u16 continuous_queues:	1;
		u16 boot_partition_supported: 1;
		u16 shutdown_supported:		1;

		u8 timeout;

	} meta;

	struct {
		//	space for admin queues is statically allocated
		struct {
			__attribute__((aligned(4096))) u8 completion[4096];
			__attribute__((aligned(4096))) u8 submission[4096];

			u64* completion_tail;
			u64* submission_tail;
		} admin;
		struct {
			u8* completion;
			u8* submission;
			nvm_doorbell_registers* doorbell;
		} io;
	} queue;

} nvm_t;





__attribute__((always_inline))
inline size_t nvm_get_page_size(u8 size) {
	return (2 << (12+size));
}

inline void nvm_reset(nvm_base_register_t* controller) {
	controller->subsystem_reset = 0x4E564D65;
	iowait();
}




nvm_t nvm = {0};

void nvm_init();

u16 nvm_get_cmdid(nvm_t* controller) {
	if (controller->cmdid == MAX_U16) {
		controller->cmdid = 0;
		return MAX_U16;
	}
	return controller->cmdid++;
}

static bool nvm_version_supported();

static bool nvm_memory_init();

static bool nvm_check_capabilities();
	//	returns false if

static void nvm_create_admin_submission_queue();
static void nvm_create_admin_completion_queue();
	//	both allocates one page for queue

/*void nvm_send(nvm_completion_entry* queue, u8 opcode, u32 namespace, void* data, size_t datalen);
bool nvm_send_wait(nvm_completion_entry* queue, u8 opcode, u32 namespace, void* data, size_t datalen);

void nvm_send_cmd(nvm_completion_entry* queue, nvm_command_t* cmd);
bool nvm_send_cmd_wait(nvm_completion_entry* queue, nvm_command_t* cmd);*/


