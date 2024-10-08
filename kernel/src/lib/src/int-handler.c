//
//	int-handler.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
void interrupt_keyboard_input(void) {
	u8 code = keyboard_receive_byte();

	keycode* key = keyboard.scancodes[code & ~(1 << 7)];
	//print("code:\t"); printu(code); print(" : "); printu(code & ~(1 << 7)); endl();

	if (key != null) {
		key->state = (code & (1 << 7)) == 0;
		if (keyboard_shift()) {
			keyboard.last = key->shift;
		} else {
			keyboard.last = key->code;
		}
	} else {
		report("unknown keyboard input (", report_error);
		printu(code); printl(")");
		return;
	}

	if ((code & (1 << 7)) == 0) {
		keyboard.hit = true;
	}
}
