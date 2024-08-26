//
//  include.h
//      part of the H-OS project
//


#ifndef H_OS_LIB_INCLUDE_H
	#define H_OS_LIB_INCLUDE_H


	//	basic memory info
	extern "C" void* KERNEL_START;
	extern "C" void* KERNEL_END;

	extern "C" void* STACK_START;
	extern "C" void* STACK_END;

	//  set up the environment and include other files

	//  bootloader library
	namespace limine {
		#include <limine.h>
	}

	//  other headers depends on          //  provides
	#include "./lib/integers.h"                 //  types (like <stdint.h>)
	#include "./lib/traits.h"                   //  type checks (like <type_traits.h>)
	#include "./lib/utils.h"                    //  basic functions and structures (strlen(), ...)
	#include "./lib/convert.h"
	#include "./lib/pair.h"
	#include "./lib/cpu.h"

	//  more of those but separated
	#include "./lib/math.h"
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

	#include "./lib/interrupts.h"
	#include "./lib/io/keyboard.h"


	//  sources
	#include "./lib/src/utils.cpp"
	#include "./lib/src/math.cpp"

	#include "./lib/src/memory.cpp"
	#include "./lib/src/segmentation.cpp"
	#include "./lib/src/memory/heap.cpp"
	#include "./lib/src/memory/paging.cpp"
	#include "./lib/src/output.cpp"

	#include "./lib/src/gdt.cpp"
	#include "./lib/src/tss.cpp"

	#include "./lib/src/interrupts/interrupts.cpp"
	#include "./lib/src/io/keyboard.cpp"

	#include "./lib/src/bitmap.cpp"
	#include "./lib/src/k_management.cpp"
	#include "./lib/src/convert.cpp"
	#include "./lib/src/cpu.cpp"
	#include "./lib/src/string.cpp"
	#include "./lib/src/vector.cpp"
	#include "./lib/src/pair.cpp"

#else
	#error include.h already included
#endif

