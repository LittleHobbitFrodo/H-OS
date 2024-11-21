//
//	keyboard.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../integers.h"
#include "./kb-keys.h"

//	PS/2 keyboard driver

//	only scan codeset 1 is supported yet

//	keyboard commands
#define KEYBOARD_CMD_SCAN_CODE_SET 0xF0		//	set/get scan set
#define KEYBOARD_CMD_ECHO 0xEE		//	does nothing (keyboard just responds)
#define KEYBOARD_CMD_SET_LEDS 0xED		//	sets keyboard LEDS
#define KEYBOARD_CMD_SCANNING_ENABLE 0xF4
#define KEYBOARD_CMD_SCANNING_DISABLE 0xF5
#define KEYBOARD_CMD_RESEND 0xFE
#define KEYBOARD_CMD_SELF_TEST 0xFF		//	reset and self test (responds with 0xAA on success)
#define KEYBOARD_CMD_SET_DEFAULT_PARAMS 0xF6
#define KEYBOARD_CMD_READ_CONFIG 0x20
#define KEYBOARD_CMD_WRITE_CONFIG 0x60
#define KEYBOARD_CMD_IDENTIFY 0xF2
#define KEYBOARD_CMD_RESET 0xF6

//	command responses
#define KEYBOARD_CMD_RESPONSE_ACK 0xFA
#define KEYBOARD_CMD_RESPONSE_RESEND 0xFE
#define KEYBOARD_CMD_RESPONSE_SELF_TEST_SUCCESS 0xAA
	//	ACK before that info
#define KEYBOARD_CMD_RESPONSE_KEY_DETECTION_ERROR 0x00
#define KEYBOARD_CMD_RESPONSE_ECHO 0xEE

//	keyboard communication ports
#define KEYBOARD_PORT_CMD 0x64
#define KEYBOARD_PORT_DATA 0x60

//	scancode prefixes
#define KEYBOARD_SCANCODE_RELEASED 0xF0
#define KEYBOARD_CODE_EXTENDED 0xE0

#define KEYBOARD_LED_SCROLL_LOCK 0b1
#define KEYBOARD_LED_NUM_LOCK 0b10
#define KEYBOARD_LED_CAPS_LOCK 0b100

#define KEYBOARD_CONFIG_INTERRUPT 0b01





//	scancode to keycode resolution structures

#define KEYCODE_COUNT 160
#define SCANCODE_COUNT 0x61

typedef struct keycode {
	volatile u8 state;	//	pressed?
	u8 code;	//	keycode ('a', ctrl, ..)
	u8 shift;	//	keycode when shift is down
} keycode;

typedef keycode** scancode_t;

typedef struct keyboard_t {
	keycode keycodes[KEYCODE_COUNT];
	keycode* scancodes[SCANCODE_COUNT];
	volatile u8 last;
	volatile bool hit;
} keyboard_t;

static keyboard_t keyboard = {0};

__attribute__((always_inline)) static inline bool keyboard_shift() {
	return keyboard.keycodes[keycode_lshift].state || keyboard.keycodes[keycode_rshift].state;
}

__attribute__((always_inline)) static inline bool keyboard_ctrl() {
	return keyboard.keycodes[keycode_lctrl].state || keyboard.keycodes[keycode_rctrl].state;
}

__attribute__((always_inline)) static inline bool keyboard_alt() {
	return keyboard.keycodes[keycode_lalt].state || keyboard.keycodes[keycode_ralt].state;
}



__attribute__((always_inline)) static inline char keyboard_char() {
	if (keyboard.hit) {
		keyboard.hit = false;
		return keyboard.last;
	}
	return '\0';
}

string keyboard_getline(bool draw);

__attribute__((always_inline)) static inline bool keyboard_hit() {
	return keyboard.hit;
}



//	driver functions
[[maybe_unused]] static void keyboard_init();
[[maybe_unused]] static bool keyboard_test();

__attribute__((always_inline)) static inline bool keyboard_readready() {
	return inb(KEYBOARD_PORT_CMD) & 0x1;
}

__attribute__((always_inline)) static inline bool keyboard_writeready() {
	return (~(inb(KEYBOARD_PORT_CMD) >> 1)) & 0x1;
}

__attribute__((always_inline)) static inline u8 keyboard_receive_byte() {
	//	receives byte from keyboard
	while (!keyboard_readready()) {
		iowait();	//	wait few nanoseconds
	}
	return inb(KEYBOARD_PORT_DATA);
}

__attribute__((always_inline)) static inline void keyboard_send_byte(u8 data) {
	//	sends byte to keyboard
	while (!keyboard_writeready()) {
		iowait();	//	wait few nanoseconds
	}
	outb(KEYBOARD_PORT_DATA, data);
}

__attribute__((always_inline)) static inline u8 keyboard_send(u8 data) {
	//	sends byte to keyboard and returns answer
	keyboard_send_byte(data);
	iowait();
	return keyboard_receive_byte();
}

static bool keyboard_send_cmd(u8 cmd);

static bool keyboard_send_cmd_data(u8 cmd, u8 data);

[[maybe_unused]] static bool keyboard_set_scan_code(u8 set);
[[maybe_unused]] static u8 keyboard_get_scan_code();