//
//  include.h
//      part of the CORE kernel belonging to the H-OS project
//

//	some inline functions are implemented in headers

#pragma once


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

//	please don't ask me how are headers separated...

//  bootloader library
//#include <limine.h>
#include "./limine.h"
#include "./lib/bootutils.h"

#include "./lib/k_management.h"

//  other headers depends on				//  provides
#include "./lib/integers.h"                 //  types (like <stdint.h>)
#include "./lib/utils.h"                    //  basic functions and structures (strlen(), ...)
#include "./lib/convert.h"					//	to-string conversions
#include "./lib/cpu.h"						//	information about the CPU
#include "./lib/k_management.h"				//	important functions (init, panic, ...)
#include "./lib/pic.h"						//	programmable interrupt controller
#include "./lib/time.h"						//	information about time
#include "./lib/timer.h"					//	hardware timer configuration


//  depends on headers above
#include "./lib/memory.h"					//	basic memory management
#include "./lib/memory/heap/heap.h"			//	kernel heap
#include "./lib/memory/paging.h"			//	memory protection
//#include "./lib/memory/heap/page-heap.h"	//	kernel heap for page table allocations

#include "./lib/uefi.h"						//	includes uefi runtime services

#include "./lib/memory/aligned_ptr.h"		//	structure for aligned allocations
#include "./lib/vector/vector.h"
#include "./lib/vector/volatile-vector.h"	//	multithreaded vector (untested)

#include "./lib/hw/pci/pci.h"


#include "./lib/segmentation.h"
#include "./lib/tss.h"
#include "./lib/gdt.h"

#include "./lib/string.h"
#include "./lib/font.h"
#include "./lib/output.h"

#include "./lib/interrupts.h"
#include "./lib/exceptions.h"
#include "./lib/int-handler.h"

#include "./lib/hardware.h"
#include "./lib/hw/keyboard.h"
#include "./lib/acpi/acpi.h"
#include "./lib/hw/nvm/nvm.h"








//  sources
#include "./lib/src/utils.c"
#include "./lib/src/memory.c"
#include "./lib/src/memory/heap/heap.c"
#include "./lib/src/memory/paging.c"
#include "./lib/src/output.c"

#include "./lib/src/gdt.c"
#include "./lib/src/tss.c"

#include "./lib/src/time.c"
#include "./lib/src/font.c"
#include "./lib/src/k_management.c"
#include "./lib/src/timer.c"
#include "./lib/src/convert.c"
#include "./lib/src/cpu.c"
#include "./lib/src/string.c"
#include "./lib/src/memory/aligned_ptr.c"
#include "./lib/src/vector/vector.c"
#include "./lib/src/vector/volatile-vector.c"

#include "./lib/src/int-handler.c"
#include "./lib/src/exceptions.c"
#include "./lib/src/interrupts.c"
#include "./lib/src/pic.c"

#include "./lib/src/hardware.c"
#include "./lib/src/hw/keyboard.c"
#include "./lib/src/hw/kb-keys.c"
#include "./lib/src/acpi/acpi.c"
#include "./lib/src/uefi.c"
#include "./lib/src/memory/heap/page-heap.c"

#include "./lib/src/hw/pci/pci.c"
#include "./lib/src/hw/ahci.c"
#include "./lib/src/hw/devices/device.c"
#include "./lib/src/hw/nvm/nvm.c"