//
//	tss.h
//		part of the H-OS project
//

#ifndef H_OS_LIB_TSS_H
	#define H_OS_LIB_TSS_H

	#ifndef KERNEL_STACK_SIZE
		#define KERNEL_STACK_SIZE 32
	#else
		#if KERNEL_STACK_SIZE < 32
			#warning kernel_stack_size macro must be bigger than 64 (defaulting to 64)
		#endif
	#endif


	namespace stack {
		namespace size {
			const static u64 kernel {KERNEL_STACK_SIZE * mem::byte_t::per_kb};
			const static u64 interrupt {INTERRUPT_HANDLER_STACK_SIZE_KB * mem::byte_t::per_kb};
		}

		extern "C" void* kernel_ptr {nullptr};
		extern "C" void* interrupt_ptr {nullptr};
		extern "C" u16 stack_segment {0};

	}

	namespace tss {

		typedef struct entry {
			u32 reserved0;

			void* stack[3];

			u64 reserved1;

			void* int_stack[7];

			u64 reserved2;
			u16 reserved3;

			u16 iomap;

			entry();

		} __attribute__((packed));
	}


#else
	#warning tss.h already included
#endif


//	tss (Tast State Segment)
//		tss manages stacks (ex.: on different rings)

//		used in interrupt handling for exmaple
//			switches to a specific stack

