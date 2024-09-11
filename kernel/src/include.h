//
//  include.h
//      part of the CORE kernel belonging to the H-OS project
//

//	some inline functions are implemented in headers

#pragma once

#ifndef H_OS_LIB_INCLUDE_H
	#define H_OS_LIB_INCLUDE_H

	//	these macros are declare just to satisfy IDE code inspector
		//	are defined in compiler script (scripts/kernel/compile)
	#ifndef KERNEL_VERSION
		#define KERNEL_VERSION "0.0.0.0"
	#endif
	#ifndef KERNEL_BUILD_TIME
		#define KERNEL_BUILD_TIME "0-0-0000"
	#endif
	#ifndef KERNEL_NAME
		#define KERNEL_NAME "H-OS CORE"
	#endif
	#ifndef KERNEL_BUILD_GUID
		#define KERNEL_BUILD_GUID "0000"
			//	KERNEL_BUILD_GUID will be most likely used for random number generation
	#endif

	#define likely(x)   __builtin_expect(!!(x), 1)
	#define unlikely(x) __builtin_expect(!!(x), 0)
	//	used in if statements for compiler optimizations


	//	extern variables
	extern void* kernel_stack_address;
	extern void* heap_virtual_base;
	extern void* kernel_stack_base;
		//	these variables are initialized in page_init() (memory/paging.c)

	//  bootloader library
	#include <limine.h>

	//  other headers depends on          //  provides
	#include "./lib/integers.h"                 //  types (like <stdint.h>)
	#include "./lib/utils.h"                    //  basic functions and structures (strlen(), ...)
	#include "./lib/convert.h"
	#include "./lib/cpu.h"
	#include "./lib/k_management.h"


	//  depends on headers above
	#include "./lib/memory.h"
	#include "./lib/memory/heap.h"
	#include "./lib/memory/paging.h"

	#include "./lib/vector.h"
	#include "./lib/tss.h"
	#include "./lib/gdt.h"				//	needs vector.h
	#include "./lib/segmentation.h"		//	needs gdt.h

	#include "./lib/string.h"
	#include "./lib/output.h"

	//  sources
	#include "./lib/src/utils.c"
	#include "./lib/src/memory.c"
	#include "./lib/src/segmentation.c"
	#include "./lib/src/memory/heap.c"
	#include "./lib/src/memory/paging.c"
	#include "./lib/src/output.c"

	#include "./lib/src/gdt.c"
	#include "./lib/src/tss.c"

	#include "./lib/src/bitmap.c"
	#include "./lib/src/k_management.c"
	#include "./lib/src/convert.c"
	#include "./lib/src/cpu.c"
	#include "./lib/src/string.c"
	#include "./lib/src/vector.c"

#endif
//	#error include.h already included
//#endif

