//
//	tss.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../tss.h"

	void tss_init() {
		//	set all to null

		//NOTE: INTERRUPT_STACKS is defined in memory.h

		memset(&tss, sizeof(tss_t), 0);

		const size_t add = (INTERRUPT_STACK_SIZE * KB) - 1;

		for (size_t i = 0; i < 7; i++) {
			tss.tss.ist[i] = (size_t)&INTERRUPT_STACKS[i] + add;
		}
		tss.tss.perms_offset = sizeof(tss_base_t);

	}
