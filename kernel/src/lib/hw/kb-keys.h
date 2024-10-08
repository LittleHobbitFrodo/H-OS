//
//	kb-keys.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

//	defines key and scancodes (only scan set 2 is supported)
struct keyboard_t;

static void keyboard_scancode_set_init(struct keyboard_t* kb);


enum scancodes {

	//	release scan codes have 7th bit set

	scancode_F1			= 0x3B,
	scancode_F2			= 0x3C,
	scancode_F3			= 0x3D,
	scancode_F4			= 0x3E,
	scancode_F5			= 0x3F,
	scancode_F6			= 0x40,
	scancode_F7			= 0x41,
	scancode_F8			= 0x42,
	scancode_F9			= 0x43,
	scancode_F10		= 0x44,
	scancode_F11		= 0x57,
	scancode_F12		= 0x58,

	scancode_escape		= 0x01,
	scancode_lctrl		= 0x1D,
	scancode_rctrl		= 0x1D,		//	extended
	scancode_lshift		= 0x2A,
	scancode_rshift		= 0x36,
	scancode_lalt		= 0x38,
	scancode_ralt		= 0x38,		//	right alt apparently not supported lol
	scancode_tab		= 0x0F,
	scancode_capslock	= 0x3A,

	scancode_delete		= 0x53,		//	extended
	scancode_backspace	= 0x0E,
	scancode_enter		= 0x1C,

	scancode_arrow_up		= 0x48,		//	extended
	scancode_arrow_down	= 0x50,		//	extended
	scancode_arrow_left	= 0x4B,		//	extended
	scancode_arrow_right	= 0x4D,		//	extended
	scancode_page_up		= 0x49,		//	extended
	scancode_page_down	= 0x51,		//	extended

	//	qwerty line 1
	scancode_back_tick	= 0x29,
	scancode_1			= 0x02,
	scancode_2			= 0x03,
	scancode_3			= 0x04,
	scancode_4			= 0x05,
	scancode_5			= 0x06,
	scancode_6			= 0x07,
	scancode_7			= 0x08,
	scancode_8			= 0x09,
	scancode_9			= 0x0A,
	scancode_0			= 0x0B,
	scancode_minus		= 0x0C,		//	-
	scancode_equal		= 0x0D,		//	=

	//	qwerty line 2
	scancode_Q			= 0x10,
	scancode_W			= 0x11,
	scancode_E			= 0x12,
	scancode_R			= 0x13,
	scancode_T			= 0x14,
	scancode_Y			= 0x15,
	scancode_U			= 0x16,
	scancode_I			= 0x17,
	scancode_O			= 0x18,
	scancode_P			= 0x19,
	scancode_sq_br		= 0x1A,		//	[
	scancode_sq_br_end	= 0x1B,		//	]

	//	qwerty line 3
	scancode_A			= 0x1E,
	scancode_S			= 0x1F,
	scancode_D			= 0x20,
	scancode_F			= 0x21,
	scancode_G			= 0x22,
	scancode_H			= 0x23,
	scancode_J			= 0x24,
	scancode_K			= 0x25,
	scancode_L			= 0x26,
	scancode_semicolon	= 0x27,
	scancode_quote		= 0x28,
	scancode_backslash	= 0x2B,

	//	qwerty line 4
	scancode_Z			= 0x2C,
	scancode_X			= 0x2D,
	scancode_C			= 0x2E,
	scancode_V			= 0x2F,
	scancode_B			= 0x30,
	scancode_N			= 0x31,
	scancode_M			= 0x32,
	scancode_comma		= 0x33,		//	,
	scancode_period		= 0x34,		//	.
	scancode_slash		= 0x35,

	scancode_space		= 0x39,

	scancode_np_enter		= 0x1C,		//	extended
	scancode_np_plus		= 0x4E,		//	extended
	scancode_np_minus		= 0x4A,
	scancode_np_asterisk	= 0x37,
	scancode_np_slash		= 0x35,		//	extended
	scancode_np_1			= 0x4F,
	scancode_np_2			= 0x50,
	scancode_np_3			= 0x51,
	scancode_np_4			= 0x4B,
	scancode_np_5			= 0x4C,
	scancode_np_6			= 0x4D,
	scancode_np_7			= 0x47,
	scancode_np_8			= 0x48,
	scancode_np_9			= 0x49,
	scancode_np_0			= 0x52,
	scancode_np_period		= 0x53,

	scancode_super = 0x60,
	scancode_super_ = 91


} scancodes;

enum keycodes {		//	these keycodes are used only in H-OS
	keycode_F1 = 128,
	keycode_F2 = 129,
	keycode_F3 = 130,
	keycode_F4 = 131,
	keycode_F5 = 132,
	keycode_F6 = 133,
	keycode_F7 = 134,
	keycode_F8 = 135,
	keycode_F9 = 136,
	keycode_F10 = 137,
	keycode_F11 = 138,
	keycode_F12 = 139,

	keycode_backspace = '\b',		//	8
	keycode_delete = 27,
	keycode_tab = '\t',				//	9
	keycode_space = ' ',
	keycode_lshift = 28,
	keycode_rshift = 29,
	keycode_enter = '\n',			//	10
	keycode_lctrl = 33,		//	'!'
	keycode_rctrl = 34,		//	'\'"'
	keycode_lalt = 35,		//	'#'
	keycode_ralt = 36,		//	'$'
	keycode_escape = 30,

	keycode_page_up = 20,
	keycode_page_down = 21,
	keycode_arrow_up = 22,
	keycode_arrow_down = 23,
	keycode_arrow_left = 24,
	keycode_arrow_right = 25,

	keycode_capslock = 26,
	keycode_np_1 = 140,
	keycode_np_2 = 141,
	keycode_np_3 = 142,
	keycode_np_4 = 143,
	keycode_np_5 = 144,
	keycode_np_6 = 145,
	keycode_np_7 = 146,
	keycode_np_8 = 147,
	keycode_np_9 = 148,
	keycode_np_0 = 149,		//	'Z'
	keycode_np_slash = 14,
	keycode_np_asterisk = 15,
	keycode_np_minus = 16,
	keycode_np_plus = 17,
	keycode_np_enter = 18,
	keycode_np_period = 19,

	keycode_super = 127

	//	other keys are identified by its character (on EN-US qwerty keyboard)

} keycodes;