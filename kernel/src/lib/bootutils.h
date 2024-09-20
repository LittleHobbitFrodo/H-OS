//
//	bootutils.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifndef H_OS_LIB_BOOTUTILS_H
	#define H_OS_LIB_BOOTUTILS_H

	#ifndef KERNEL_STACK_SIZE
		#define KERNEL_STACK_SIZE 64
	#endif

	#ifndef INTERRUPT_STACK_SIZE
		#define INTERRUPT_STACK_SIZE 8
	#endif

	#define STACK_MINIMUM_INITIAL_SIZE KERNEL_STACK_SIZE + INTERRUPT_STACK_SIZE

	#define INTERRUPT_STACK_COUNT 7
		//	defined by tss

	static volatile struct limine_stack_size_request req_stack_size = {
		.id = LIMINE_STACK_SIZE_REQUEST,
		.revision = 0,
		.stack_size = KERNEL_STACK_SIZE * 1024	//	kb
	};

	static volatile struct limine_kernel_address_request req_k_address = {
		.id = LIMINE_KERNEL_ADDRESS_REQUEST,
		.revision = 0
	};

	static volatile struct limine_memmap_request req_memmap = {
		.id = LIMINE_MEMMAP_REQUEST,
		.revision = 0
	};

	static volatile struct limine_paging_mode_request req_page_mode = {
		.id = LIMINE_PAGING_MODE_REQUEST,
		.revision = 2,
		.mode = LIMINE_PAGING_MODE_X86_64_4LVL
	};

	static volatile struct limine_hhdm_request req_page_hhdm = {
		.id = LIMINE_HHDM_REQUEST,
		.revision = 2,
	};

	static volatile struct limine_kernel_file_request req_kernel_file = {
		.id = LIMINE_KERNEL_FILE_REQUEST,
		.revision = 0
	};

#endif