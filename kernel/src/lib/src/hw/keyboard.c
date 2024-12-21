//
//	keyboard.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../../hw/keyboard.h"
#include "../../k_management.h"

void keyboard_init() {
	//	US (QWERTY) layout is used by default

	size_t line = 0;
	if (vocality >= vocality_report_everything) {
		line = report("keyboard driver initialization\n", report_note);
	}

	if (!keyboard_send_cmd_data(KEYBOARD_CMD_SET_LEDS, 0)) {
		if (vocality >= vocality_report_everything) {
			report_status("PARTIAL FAILURE", line, col.yellow);
		}
		if (vocality >= vocality_vocal) {
			report("failed to set leds\n", report_error);
		}
	}

	//	init scancode set structures
	keyboard_scancode_set_init(&keyboard);

	idt_set_address(&idt[33], (void*)&isr_int_keyboard_input);
	idt[33].flags = idt_gate_type_hardware;
	idt[33].cs = 0x8; //	index 1
	idt[33].flags &= ~idt_bits_privilege;

	pic_remap();

	if (vocality >= vocality_report_everything) {
		report_status("SUCCESS", line, col.green);
	}

}

bool keyboard_send_cmd(u8 cmd) {
	//	sends command and returns true if the keyboard answers with ACK
	u8 status;
	for (u8 i = 0; i < 8; i++) {
		status = keyboard_send(cmd);
		if (status == KEYBOARD_CMD_RESPONSE_ACK) {
			return true;
		}
	}
	return false;
}

bool keyboard_send_cmd_data(u8 cmd, u8 data) {
	//	sends command with data and returns true if the keyboard answers with ACK
	u8 status;
	for (u8 i = 0; i < 8; i++) {
		status = keyboard_send(cmd);
		if (status == KEYBOARD_CMD_RESPONSE_RESEND) {
			continue;
		}

		status = keyboard_send(data);
		if (status == KEYBOARD_CMD_RESPONSE_ACK) {
			return true;
		}
	}
	return false;
}

bool keyboard_test() {
	//	initializes keyboard self test
	for (u8 i = 0; i < 8; i++) {
		if (!keyboard_send_cmd(KEYBOARD_CMD_SELF_TEST)) {
			continue;
		}
		iowait();
		return keyboard_receive_byte() == KEYBOARD_CMD_RESPONSE_SELF_TEST_SUCCESS;
	}
	return false;
}

bool keyboard_set_scan_code(u8 set) {
	for (u8 i = 0; i < 8; i++) {
		if (keyboard_send_cmd_data(KEYBOARD_CMD_SCAN_CODE_SET, set)) {
			return true;
		}
	}
	return false;
}

u8 keyboard_get_scan_code() {
	u8 status;
	for (u8 i = 0; i < 8; i++) {
		keyboard_send_byte(KEYBOARD_CMD_SCAN_CODE_SET);
		(void)keyboard_send(0);
		status = keyboard_receive_byte();
		if (status != KEYBOARD_CMD_RESPONSE_RESEND) {
			switch (status) {
				case 0x43: {
					return 1;
				}
				case 0x41: {
					return 2;
				}
				case 0x3F: {
					return 3;
				}
				default: {
					print("gsc:\t"); printu(status); endl();
					break;
				}
			}
		}
	}
	return 0;
}

string keyboard_getline(bool draw) {
	string input;
	str(&input, &heap.global);
	char c;

	size_t start = output.column;

	while (true) {
		asm volatile("hlt");
		if (!keyboard_hit()) {
			continue;
		}
		c = keyboard_char();

		switch (c) {
			case '\n': {
				if (draw) {
					endl();
				}
				return input;
			}
			case '\b': {
				if (output.column > start) {
					if ((draw) && (output.column > 0)) {
						screen_flush_at(output.line, --output.column);
					}
					str_pop(&input, 1);
				}
				break;
			}
			default: {
				if ((c >= ' ') && (c <= '~')) {
					str_pushc(&input, c);
					if ((output.column + 1) * font.size >= screen.w) {
						return input;
					}
					if (draw) {
						printc(c);
					}
				}
				break;
			}
		}
	}
	return input;
}