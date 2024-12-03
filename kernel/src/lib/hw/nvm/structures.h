//
//	hw/nvm/structures.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

typedef struct nvm_version_t {
	//	VS
	u32 tertiary:	8;
	u32 minor:		8;
	u32 major:		16;
} __attribute__((packed)) nvm_version_t;

typedef struct nvm_capabilities_t {
	//	CAP

	u64 maximum_queue_entries:		16;
	u64 contignous_queues_required:	1;		//	1 = all queues must be physically continuous
	u64 arbitration_supported:		2;
	u64 _reserved:					5;
	u64 timeout:					8;		//	worst-case timeout (normally much shorted time), 500 ms units
	u64 stride:						4;		//	indicates stride between doorbell properties (2 ^ (2 + stride))
	u64 subsystem_reset_supported:	1;
	u64 command_sets_supported:		8;
	u64 boot_partition_supported:	1;
	u64 controller_power_scope:		2;		//	enum nvm_controller_power_scopes
	u64 page_size_minimum:			4;		//	minimum page size supported by controller (2 ^(12 + mpsmin))
	u64 page_size_maximum:			4;		//	maximum page size supported by controller (2 ^ (12 + mpsmax)
	u64 persistent_memory_region:	1;		//	1 = persistent memory region is supported
	u64 controller_memory_buffer_supported:		1;	//	1 = controller supports controller memory buffer
	u64 subsystem_shutdown_supported:	1;	//	1 = controller supports NVM subsystem shutdown feature
	//	rms = ready modes supported
	u64 rms_meadia_mode:	1;			//	supports Controller ready with Media feature
	u64 rms_independent_of_meadi_mode:		1;
	u64 _reserved2:	2;

} __attribute__((packed)) nvm_capabilities_t;

typedef struct nvm_controller_config_t {
	u32 enable:		1;		//	1 = controller can process commands
	u32 _reserved:	2;
	u32 command_set_selected:	3;
	u32 memory_page_size:	4;		//	(2 ^(12 + MPS))
	u32 arbit_mechanism:	3;
	u32 shutdown_notification:		2;
	u32 queue_entry_size_submission:		4;		//	2^n
	u32 queue_entry_size_completion:		4;		//	2^n
	u32 contoller_ready_mode:			1;		//	CRIME (lol)
	u32 _reserved1:			8;
} __attribute__((packed)) nvm_controller_config_t;

typedef struct nvm_controller_status_t {
	u32 ready:		1;		//	ready to process submission queue entries
	u32 fatal_error:	1;
	u32 shutdown_status:	2;
	u32 subsystem_reset_occured:	1;
	u32 processing_paused:		1;
	u32 shutdown_type:		1;
	u32 reserved:	25;
} __attribute__((packed)) nvm_controller_status_t;

typedef struct nvm_queue_attributes_t {
	u32 submission_size:		12;
	u32 completion_size:		12;
	u32 _reserved:	4;
} __attribute__((packed)) nvm_queue_attributes_t;

typedef struct nvm_queue_ptr {
	u64 _reserved:		12;
	u64 ptr:			52;		//	higher 52 bits (physical address)
} __attribute__((packed)) nvm_queue_ptr;

typedef struct nvm_base_register_t {

	nvm_capabilities_t capabilities;		//	bitfield
	nvm_version_t version;
	u32 interrupt_mask_set;
	u32 interrupt_mask_clear;
	nvm_controller_config_t config;
	nvm_controller_status_t status;
	nvm_queue_attributes_t queue_attributes;
	nvm_queue_ptr submission_queue;
	nvm_queue_ptr completion_queue;

} __attribute__((packed)) nvm_base_register_t;
#define NVM_SUBMISSION_QUEUE_TAIL 0x1000
#define NVM_COMPLETION_QUEUE_HEAD 0x1000
//	Y is specified in the capabilities register

typedef struct nvm_queue {
	void* ptr;
	size_t size;
} nvm_queue;

typedef struct nvm_command {
	u32 opcode:		8;
	u32 fused_op:	2;
	u32 reserved:	4;
	u32 sgl:		2;		//	else prp
	u32 index:		16;
} __attribute__((packed)) nvm_command;

typedef struct nvm_prp_ptr {
	u64 reserved:	2;
	u64 physical:	62;
} __attribute__((packed)) nvm_prp_ptr;

typedef struct nvm_submission_entry {
	//	16 dwords
	nvm_command command;
	u32 namespace;
	u32 reserved[2];
	u64 metadata;
	nvm_prp_ptr data_ptr[2];
	u32 specific[6];

} __attribute__((packed)) nvm_submission_entry;

typedef struct nvm_completion_entry {
	u32 specific;
	u32 reserved;
	u16 head_pointer;
	u16 queue_id;
	u16 cmd_id;
	u16 phase:		1;
	u16 status:		15;
} __attribute__((packed)) nvm_completion_entry;


enum nvm_controller_power_scopes {
	nvm_cps_not_reported = 0,
	nvm_cps_controlller_scope = 0b1,
	nvm_cps_domain_scope = 0b10,
	nvm_cps_nvm_subsystem_scope = 0b11
} nvm_controller_power_scopes;

enum nvm_controller_css {
	nvm_ccss_nvm_command_set = 0,
	nvm_ccss_reserved_command_set = 0b101,
	nvm_ccss_reserved2_command_set = 0b001,
	nvm_ccss_all_command_sets_supported = 0b110,
	nvm_ccss_admin_command_set = 0b111
} nvm_controller_css;

enum nvm_arbitration_mechanisms {
	nvm_arbit_round_robin = 0,
	nvm_arbit_weighted_round_robin = 0b001,
	nvm_arbit_reserved = 0b010,
	nvm_arbit_reserved2 = 0b110,
	nvm_arbit_vendor_specific = 0b111
} nvm_arbitration_mechanisms;

enum nvm_shutdown_notifications {
	nvm_shutdown_no_notification = 0,
	nvm_shutdown_notification_normal = 0b01,
	nvm_shutdown_notification_abrupt = 0b10
} nvm_shutdown_notifications;

enum nvm_sutdown_status {
	nvm_shutdown_normal_operation = 0,
	nvm_shutdown_processing = 0b01,
	nvm_shutdown_complete = 0b10
} nvm_shutdown_status;