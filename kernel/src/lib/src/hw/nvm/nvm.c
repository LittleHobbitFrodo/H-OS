//
//	hw/nvm/nvm.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../../../hw/nvm/nvm.h"

void nvm_init() {

	if (nvm.initialzed) {
		return;
	}

	size_t line = 0;

	if (vocality >= vocality_report_everything) {
		line = report("proceeding to initialize NVM controller\n", report_note);
	}

	if (nvm.address.pci.enable == 0) {
		if (vocality >= vocality_report_everything) {
			report_status("FAILURE", line, col.red);
		}
		report("cannot find PCI address\n", report_error);
		return;
	}

	if (!nvm_memory_init()) {
		if (vocality >= vocality_report_everything) {
			report_status("FAILURE", line, col.red);
		}
		report("failed to initialize virtual address space for NVM configuration\n", report_error);
		return;
	}

	if (!nvm_version_supported()) {
		if (vocality >= vocality_report_everything) {
			report_status("FAILURE", line, col.red);
		}
		report("NVM version ", report_error);
		const nvm_version_t version = nvm.controller->version;
		printu(version.major); printc('.'); printu(version.minor); printc('.'); printu(version.tertiary);
		printl(" is not supported");
		return;
	}

	//	check capabilities for command setW
	if (!nvm_check_capabilities()) {
		if (vocality >= vocality_report_everything) {
			report_status("FAILURE", line, col.red);
		}
		report("NVM controller does not support command set\n", report_error);
		return;
	}

	//	reset controller
	nvm_reset(nvm.controller);

	//	set controller config, admin queues
	nvm_create_admin_submission_queue();
	nvm_create_admin_completion_queue();


	//	start da controller

	//	interrupts + register handler

	//	send identify command
		//	check for IO controller

	//	reset software progress marker

	//	create IO completion_ptr and submission_ptr queue

	//	identify namespace IDs
		//	then individual namespaces
		//	check block size, and if its read only


	nvm.initialzed = true;
	nvm.used = true;

	if (vocality >= vocality_report_everything) {
		report_status("SUCCESS", line, col.green);
	}
}


bool nvm_version_supported() {
	nvm_version_t version = nvm.controller->version;
	return ((version.major == 1) || (version.minor == 4));
}

bool nvm_memory_init() {

	bool is_io;
	u64 bar = pci_read_bar(nvm.address.pci, 0, &is_io);
	if (is_io) {
		return false;
	}
	print("NVM BASE:\t"); printp((void*)bar); endl();
	union virtual_address address = {.voidptr = pages.system.pdpt.virtual};

	for (size_t i = 0; i < PAGE_COUNT; i++) {
		if (pages.system.pdpt.table[i].address == 0) {
			address.virtual_address.pdpt = i;
			goto pdpt_found;
		}
	}

	return false;

	pdpt_found:

	nvm.table = (sized_page_table*)table_alloc(&pages.heap.global, 1);
	if (nvm.table == null) {
		return false;
	}

	nvm.address.physical = align(bar, 2*MB);

	for (size_t i = 0; i < PAGE_COUNT; i++) {
		(*nvm.table)[i] = (sized_page_entry){.present = true, .write = true, .exec_disable = true, .page_size = true};
	}
	unsized_page_set_address(pages.system.pdpt.table[address.virtual_address.pdpt], table_physical(&pages.heap.global, (void*)nvm.table));

	sized_page_set_address((*nvm.table)[0], nvm.address.physical);
	sized_page_set_address((*nvm.table)[1], nvm.address.physical + (2*MB));
	nvm.controller = (nvm.address.virtual = address.voidptr);



	nvm.queue.io.doorbell = (void*)((size_t)nvm.controller + NVM_DOORBELL_OFFSET);

	return true;
}

bool nvm_check_capabilities() {
	const nvm_capabilities_t cap = nvm.controller->capabilities;

	nvm.meta.max_entries = cap.maximum_queue_entries;
	nvm.meta.stride = cap.stride;
	nvm.meta.page_size_min = 1u << (12 + cap.page_size_minimum);
	nvm.meta.page_size_max = 1u << (12 + cap.maximum_queue_entries);
	nvm.meta.continuous_queues = cap.continuous_queues_required;
	nvm.meta.boot_partition_supported = cap.boot_partition_supported;
	nvm.meta.shutdown_supported = cap.subsystem_shutdown_supported;
	nvm.meta.timeout = cap.timeout;

	return cap.cmd_sets_supported_cmd_set;
}


void nvm_create_admin_submission_queue() {
	nvm.controller->admin_submission_queue.ptr = ((size_t)&nvm.queue.admin.submission - pages.kernel.virtual + pages.kernel.physical) >> 12;
	if (nvm.meta.max_entries < 255) {
		nvm.controller->admin_queue_attributes.completion_size = nvm.meta.max_entries;
	} else {
		nvm.controller->admin_queue_attributes.completion_size = 255;
	}
}

void nvm_create_admin_completion_queue() {
	nvm.controller->admin_completion_queue.ptr = ((size_t)&nvm.queue.admin.completion - pages.kernel.virtual + pages.kernel.physical) >> 12;
	if (nvm.meta.max_entries < 63) {
		nvm.controller->admin_queue_attributes.submission_size = nvm.meta.max_entries;
	} else {
		nvm.controller->admin_queue_attributes.submission_size = 63;
	}
}

/*void nvm_send(nvm_completion_entry* queue, u8 opcode, u32 namespace, void* data, size_t datalen) {

}


void nvm_send_admin(u8 opcode, u32 namespace, void* data) {
	//	sends command
}

bool nvm_send_admin_wait(u8 opcode, u32 namespace, void* data) {
	//	sends command and waits for completion
}*/