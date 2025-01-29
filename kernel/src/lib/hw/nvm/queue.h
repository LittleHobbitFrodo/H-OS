//
//	hw/nvm/queue.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

typedef struct nvm_queue_attributes_t {
	u32 submission_size:		12;
	u32 completion_size:		12;
	u32 _reserved:	4;
} __attribute__((packed)) nvm_queue_attributes_t;

typedef struct nvm_queue_ptr {
	u64 _reserved:		12;
	u64 ptr:			52;		//	higher 52 bits (physical address)
} __attribute__((packed)) nvm_queue_ptr;



typedef struct nvm_queue {
	struct {
		nvm_submission_entry* queue;
		nvm_doorbell_registers* doorbell;
	} submission;
	struct {
		nvm_completion_entry* queue;
		nvm_doorbell_registers* doorbell;
	} completion;
} nvm_queue;

