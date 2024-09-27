//
//	int-handler.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

void interrupt_timer_pit([[maybe_unused]] int_stack_frame *frame) {
	print("Interrupt timer pit:\t");

	tick++;

	//outb(0x20, 0x20);
}
