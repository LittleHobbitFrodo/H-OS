//
//	tss.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifndef H_OS_LIB_TSS_C
	#define H_OS_LIB_TSS_C

	void tss_init() {
		//	set all to null

		memset(&tss, sizeof(tss_t), 0);

		tss.tss.rsp[0] = (u64)KERNEL_STACK_END;
		for (u16 i = 0; i < INTERRUPT_STACK_SIZE; i++) {
			tss.tss.ist[i] = (size_t)INTERRUPT_STACKS[i] + (INTERRUPT_STACK_SIZE * KB);
		}
	}

#endif