//
//	hw/nvm/base-register.h
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

	u64 maximum_queue_entries:		16;		//	maximum individual queue size
	u64 continuous_queues_required:	1;		//	1 = all queues must be physically continuous
	u64 arbitration_supported:		2;		//	0 = weighted round-robin, 1 = vendor specific
	u64 _reserved:					5;
	u64 timeout:					8;		//	worst-case timeout (normally much shorter time), 500 ms units
	u64 stride:						4;		//	indicates stride between doorbell properties (2 ^ (2 + stride))
	u64 subsystem_reset_supported:	1;

	u64 cmd_sets_supported_cmd_set:	1;
	u64 cmd_sets_supported_reserved:5;
	u64 cmd_sets_supported_idk:		1;
	u64 cmd_sets_supported_no_set_supported:	1;

	u64 boot_partition_supported:	1;
	u64 controller_power_scope:		2;		//	enum nvm_controller_power_scopes
	u64 page_size_minimum:			4;		//	minimum page size supported by controller (2 ^(12 + mpsmin))
	u64 page_size_maximum:			4;		//	maximum page size supported by controller (2 ^ (12 + mpsmax)
	u64 persistent_memory_region:	1;		//	1 = persistent memory region is supported
	u64 controller_memory_buffer_supported:		1;	//	1 = controller supports controller memory buffer
	u64 subsystem_shutdown_supported:	1;	//	1 = controller supports NVM subsystem shutdown feature
	//	rms = ready modes supported
	u64 rms_media_mode:	1;			//	supports Controller ready with Media feature
	u64 rms_independent_of_media_mode:		1;
	u64 _reserved2:	3;

} __attribute__((packed)) nvm_capabilities_t;

typedef struct nvm_controller_config_t {
	u32 enable:		1;		//	1 = controller can process commands
	u32 _reserved:	3;
	u32 command_set_selected:	3;
	u32 memory_page_size:	4;		//	(2 ^(12 + MPS))
	u32 arbit_mechanism:	3;
	u32 shutdown_notification:		2;
	u32 queue_entry_size_submission:		4;		//	2^n
	u32 queue_entry_size_completion:		4;		//	2^n
	u32 contoller_ready_mode:			1;		//	CRIME (lol)
	u32 _reserved1:			7;
} __attribute__((packed)) nvm_controller_config_t;

typedef struct nvm_controller_status_t {
	u32 ready:		1;		//	ready to process submission_ptr queue entries
	u32 fatal_error:	1;
	u32 shutdown_status:	2;
	u32 subsystem_reset_occured:	1;
	u32 processing_paused:		1;
	u32 shutdown_type:		1;
	u32 reserved:	25;
} __attribute__((packed)) nvm_controller_status_t;

typedef struct nvm_base_register_t {

	nvm_capabilities_t capabilities;		//	CAP		bitfield
	nvm_version_t version;					//	VS
	u32 interrupt_mask_set;
	u32 interrupt_mask_clear;
	nvm_controller_config_t config;			//	CC
	u32 _reserved;
	nvm_controller_status_t status;
	u32 subsystem_reset;		//	write 0x4E564D65 to initialize reset
	nvm_queue_attributes_t admin_queue_attributes;
	nvm_queue_ptr admin_submission_queue;
	nvm_queue_ptr admin_completion_queue;
	u32 _memory_buffer_location;
	u32 _memory_buffer_size;
	u32 _boot_part_info;
	u32 _boot_part_read_select;
	u64 _boot_part_memory_location;
	u64 _controller_memory_buffer_memory_space_control;
	u32 _controller_memory_buffer_status;
	u32 _controller_memory_buffer_elasticity_size;
	u32 _controller_memory_buffer_sustained_write_throughput;
	u32 system_shutdown;
	u32 _controller_ready_timeouts;

	u8 _reserved_2[0xE00-0x6C];

	u32 _persistent_memory_capabilities;

} __attribute__((packed)) nvm_base_register_t;

//	Y is specified in the capabilities register