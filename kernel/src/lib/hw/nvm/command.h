//
//	hw/nvm/command.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

typedef struct nvm_command_t {
	u32 opcode:		8;		//	OPC
	u32 fused_op:	2;
	u32 reserved:	4;
	u32 sgl:		2;		//	0 = prp, 1 = sgl
	u32 cmdid:		16;
} __attribute__((packed)) nvm_command_t;


typedef struct nvm_submission_entry {
	//	16 dwords (64 bytes)
	nvm_command_t command;
	u32 namespace;
	u32 cmd_specific[2];
	u64 metadata_ptr;
	u64 data_ptr[2];
	u32 cmd_specific2[6];

} __attribute__((packed)) nvm_submission_entry;
#define NVM_SUBMISSION_ENTRY_SIZE 64

typedef struct nvm_completion_entry {
	//	2 dwords (16 bytes)
	u32 specific;
	u32 reserved;
	u16 head_pointer;
	u16 queue_id;
	u16 cmd_id;
	u16 phase:		1;
	u16 status:		15;
} __attribute__((packed)) nvm_completion_entry;
#define NVM_COMPLETION_ENTRY_SIZE 16



//	admin commands
#define NVM_CMD_ADMIN_DELETE_IO_SUBMISSION 0x00
#define NVM_CMD_ADMIN_CREATE_IO_SUBMISSION 0x01
#define NVM_CMD_ADMIN_DELETE_IO_COMPLETION 0x04
#define NVM_CMD_ADMIN_CREATE_IO_COMPLETION 0x05
#define NVM_CMD_ADMIN_IDENTIFY_NAMESPACE 0x06
#define NVM_CMD_ADMIN_ABORT 0x08

#define NVM_CMD_FABRICS 0x7f

//	IO commands
#define NVM_CMD_IO_READ 0x02
#define NVM_CMD_IO_WRITE 0x01