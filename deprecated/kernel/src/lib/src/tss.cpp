//
//	tss.cpp
//		part of the H-OS project
//

#ifdef H_OS_LIB_TSS_H
	#ifndef H_OS_LIB_TSS_CPP
		#define H_OS_LIB_TSS_CPP

		tss::entry::entry() {
			this->reserved0 = 0;
			for (u8 i = 0; i < 3; i++) {
				this->stack[i] = nullptr;
			}
			this->reserved1 = 0;
			for (u8 i = 0; i < 7; i++) {
				this->int_stack[i] = nullptr;
			}
			this->reserved2 = 0;
			this->reserved3 = 0;
			this->iomap = 0;
		}


	#else
		#warning tss.cpp already included
	#endif
#else
	#error tss.cpp: tss.h not inlcuded
#endif