//
//	kb-keys.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../../hw/kb-keys.h"

void keyboard_scancode_set_init(keyboard_t* kb) {
	//	init keycodes

		//	special keys (not in ascii)
	kb->keycodes[keycode_F1] = (keycode){.state = false, .code = keycode_F1, .shift = keycode_F1};
	kb->keycodes[keycode_F2] = (keycode){.state = false, .code = keycode_F2, .shift = keycode_F2};
	kb->keycodes[keycode_F3] = (keycode){.state = false, .code = keycode_F3, .shift = keycode_F3};
	kb->keycodes[keycode_F4] = (keycode){.state = false, .code = keycode_F4, .shift = keycode_F4};
	kb->keycodes[keycode_F5] = (keycode){.state = false, .code = keycode_F5, .shift = keycode_F5};
	kb->keycodes[keycode_F6] = (keycode){.state = false, .code = keycode_F6, .shift = keycode_F6};
	kb->keycodes[keycode_F7] = (keycode){.state = false, .code = keycode_F7, .shift = keycode_F7};
	kb->keycodes[keycode_F8] = (keycode){.state = false, .code = keycode_F8, .shift = keycode_F8};
	kb->keycodes[keycode_F9] = (keycode){.state = false, .code = keycode_F9, .shift = keycode_F9};
	kb->keycodes[keycode_F10] = (keycode){.state = false, .code = keycode_F10, .shift = keycode_F10};
	kb->keycodes[keycode_F11] = (keycode){.state = false, .code = keycode_F11, .shift = keycode_F11};
	kb->keycodes[keycode_F12] = (keycode){.state = false, .code = keycode_F12, .shift = keycode_F12};

	kb->keycodes[keycode_backspace] = (keycode){.state = false, .code = keycode_backspace, .shift = keycode_backspace};
	kb->keycodes[keycode_delete] = (keycode){.state = false, .code = keycode_delete, .shift = keycode_delete};
	kb->keycodes[keycode_tab] = (keycode){.state = false, .code = '\t', .shift = '\t'};
	kb->keycodes[keycode_space] = (keycode){.state = false, .code = ' ', .shift = ' '};
	kb->keycodes[keycode_lshift] = (keycode){.state = false, .code = keycode_lshift, .shift = keycode_lshift};
	kb->keycodes[keycode_rshift] = (keycode){.state = false, .code = keycode_rshift, .shift = keycode_rshift};
	kb->keycodes[keycode_enter] = (keycode){.state = false, .code = '\n', .shift = '\n'};
	kb->keycodes[keycode_lctrl] = (keycode){.state = false, .code = keycode_lctrl, .shift = keycode_lctrl};
	kb->keycodes[keycode_rctrl] = (keycode){.state = false, .code = keycode_rctrl, .shift = keycode_rctrl};
	kb->keycodes[keycode_lalt] = (keycode){.state = false, .code = keycode_lalt, .shift = keycode_lalt};
	kb->keycodes[keycode_ralt] = (keycode){.state = false, .code = keycode_ralt, .shift = keycode_ralt};
	kb->keycodes[keycode_escape] = (keycode){.state = false, .code = keycode_escape, .shift = keycode_escape};
	kb->keycodes[keycode_page_up] = (keycode){.state = false, .code = keycode_page_up, .shift = keycode_page_up};
	kb->keycodes[keycode_page_down] = (keycode){.state = false, .code = keycode_page_down, .shift = keycode_page_down};
	kb->keycodes[keycode_arrow_up] = (keycode){.state = false, .code = keycode_arrow_up, .shift = keycode_arrow_up};
	kb->keycodes[keycode_arrow_down] = (keycode){.state = false, .code = keycode_arrow_down, .shift = keycode_arrow_down};
	kb->keycodes[keycode_arrow_left] = (keycode){.state = false, .code = keycode_arrow_left, .shift = keycode_arrow_left};
	kb->keycodes[keycode_arrow_right] = (keycode){.state = false, .code = keycode_arrow_right, .shift = keycode_arrow_right};

	kb->keycodes[keycode_capslock] = (keycode){.state = false, .code = keycode_capslock, .shift = keycode_capslock};
	kb->keycodes[keycode_np_1] = (keycode){.state = false, .code = '1', .shift = keycode_np_1};
	kb->keycodes[keycode_np_2] = (keycode){.state = false, .code = '2', .shift = keycode_np_2};
	kb->keycodes[keycode_np_3] = (keycode){.state = false, .code = '3', .shift = keycode_np_3};
	kb->keycodes[keycode_np_4] = (keycode){.state = false, .code = '4', .shift = keycode_np_4};
	kb->keycodes[keycode_np_5] = (keycode){.state = false, .code = '5', .shift = keycode_np_5};
	kb->keycodes[keycode_np_6] = (keycode){.state = false, .code = '6', .shift = keycode_np_6};
	kb->keycodes[keycode_np_7] = (keycode){.state = false, .code = '7', .shift = keycode_np_7};
	kb->keycodes[keycode_np_8] = (keycode){.state = false, .code = '8', .shift = keycode_np_8};
	kb->keycodes[keycode_np_9] = (keycode){.state = false, .code = '9', .shift = keycode_np_9};
	kb->keycodes[keycode_np_0] = (keycode){.state = false, .code = '0', .shift = keycode_np_0};
	kb->keycodes[keycode_np_slash] = (keycode){.state = false, .code = '/', .shift = keycode_np_slash};
	kb->keycodes[keycode_np_asterisk] = (keycode){.state = false, .code = '*', .shift = keycode_np_asterisk};
	kb->keycodes[keycode_np_minus] = (keycode){.state = false, .code = '-', .shift = keycode_np_minus};
	kb->keycodes[keycode_np_plus] = (keycode){.state = false, .code = '+', .shift = keycode_np_plus};
	kb->keycodes[keycode_np_enter] = (keycode){.state = false, .code = '\n', .shift = keycode_np_enter};
	kb->keycodes[keycode_np_period] = (keycode){.state = false, .code = '.', .shift = keycode_np_period};

		//	qwerty line 1
	kb->keycodes['`'] = (keycode){.state = false, .code = '`', .shift = '~'};
	kb->keycodes['1'] = (keycode){.state = false, .code = '1', .shift = '!'};
	kb->keycodes['2'] = (keycode){.state = false, .code = '2', .shift = '@'};
	kb->keycodes['3'] = (keycode){.state = false, .code = '3', .shift = '#'};
	kb->keycodes['4'] = (keycode){.state = false, .code = '4', .shift = '$'};
	kb->keycodes['5'] = (keycode){.state = false, .code = '5', .shift = '%'};
	kb->keycodes['6'] = (keycode){.state = false, .code = '6', .shift = '^'};
	kb->keycodes['7'] = (keycode){.state = false, .code = '7', .shift = '&'};
	kb->keycodes['8'] = (keycode){.state = false, .code = '8', .shift = '*'};
	kb->keycodes['9'] = (keycode){.state = false, .code = '9', .shift = '('};
	kb->keycodes['0'] = (keycode){.state = false, .code = '0', .shift = ')'};
	kb->keycodes['-'] = (keycode){.state = false, .code = '-', .shift = '_'};
	kb->keycodes['='] = (keycode){.state = false, .code = '=', .shift = '+'};

		//	qwerty line 2
	kb->keycodes['\t'] = (keycode){.state = false, .code = '\t', .shift = '\t'};
	kb->keycodes['q'] = (keycode){.state = false, .code = 'q', .shift = 'Q'};
	kb->keycodes['w'] = (keycode){.state = false, .code = 'w', .shift = 'W'};
	kb->keycodes['e'] = (keycode){.state = false, .code = 'e', .shift = 'E'};
	kb->keycodes['r'] = (keycode){.state = false, .code = 'r', .shift = 'R'};
	kb->keycodes['t'] = (keycode){.state = false, .code = 't', .shift = 'T'};
	kb->keycodes['y'] = (keycode){.state = false, .code = 'y', .shift = 'Y'};
	kb->keycodes['u'] = (keycode){.state = false, .code = 'u', .shift = 'U'};
	kb->keycodes['i'] = (keycode){.state = false, .code = 'i', .shift = 'I'};
	kb->keycodes['o'] = (keycode){.state = false, .code = 'o', .shift = 'O'};
	kb->keycodes['p'] = (keycode){.state = false, .code = 'p', .shift = 'P'};
	kb->keycodes['['] = (keycode){.state = false, .code = '[', .shift = '{'};
	kb->keycodes[']'] = (keycode){.state = false, .code = ']', .shift = '}'};

		//	qwerty line 3
	kb->keycodes['a'] = (keycode){.state = false, .code = 'a', .shift = 'A'};
	kb->keycodes['s'] = (keycode){.state = false, .code = 's', .shift = 'S'};
	kb->keycodes['d'] = (keycode){.state = false, .code = 'd', .shift = 'D'};
	kb->keycodes['f'] = (keycode){.state = false, .code = 'f', .shift = 'F'};
	kb->keycodes['g'] = (keycode){.state = false, .code = 'g', .shift = 'G'};
	kb->keycodes['h'] = (keycode){.state = false, .code = 'h', .shift = 'H'};
	kb->keycodes['j'] = (keycode){.state = false, .code = 'j', .shift = 'J'};
	kb->keycodes['k'] = (keycode){.state = false, .code = 'k', .shift = 'K'};
	kb->keycodes['l'] = (keycode){.state = false, .code = 'l', .shift = 'L'};
	kb->keycodes[';'] = (keycode){.state = false, .code = ';', .shift = ':'};
	kb->keycodes['\''] = (keycode){.state = false, .code = '\'', .shift = '\"'};
	kb->keycodes['\\'] = (keycode){.state = false, .code = '\\', .shift = '|'};

		//	qwerty line 4
	kb->keycodes['z'] = (keycode){.state = false, .code = 'z', .shift = 'Z'};
	kb->keycodes['x'] = (keycode){.state = false, .code = 'x', .shift = 'X'};
	kb->keycodes['c'] = (keycode){.state = false, .code = 'c', .shift = 'C'};
	kb->keycodes['v'] = (keycode){.state = false, .code = 'v', .shift = 'V'};
	kb->keycodes['b'] = (keycode){.state = false, .code = 'b', .shift = 'B'};
	kb->keycodes['n'] = (keycode){.state = false, .code = 'n', .shift = 'N'};
	kb->keycodes['m'] = (keycode){.state = false, .code = 'm', .shift = 'M'};
	kb->keycodes[','] = (keycode){.state = false, .code = ',', .shift = '<'};
	kb->keycodes['.'] = (keycode){.state = false, .code = '.', .shift = '>'};
	kb->keycodes['/'] = (keycode){.state = false, .code = '/', .shift = '?'};
	kb->keycodes[keycode_super] = (keycode){.state = false, .code = keycode_super, .shift = keycode_super};


	//	bind scancodes to keycodes
	//	Function keys
	kb->scancodes[scancode_F1] = &kb->keycodes[keycode_F1];
	kb->scancodes[scancode_F2] = &kb->keycodes[keycode_F2];
	kb->scancodes[scancode_F3] = &kb->keycodes[keycode_F3];
	kb->scancodes[scancode_F4] = &kb->keycodes[keycode_F4];
	kb->scancodes[scancode_F5] = &kb->keycodes[keycode_F5];
	kb->scancodes[scancode_F6] = &kb->keycodes[keycode_F6];
	kb->scancodes[scancode_F7] = &kb->keycodes[keycode_F7];
	kb->scancodes[scancode_F8] = &kb->keycodes[keycode_F8];
	kb->scancodes[scancode_F9] = &kb->keycodes[keycode_F9];
	kb->scancodes[scancode_F10] = &kb->keycodes[keycode_F10];
	kb->scancodes[scancode_F11] = &kb->keycodes[keycode_F11];
	kb->scancodes[scancode_F12] = &kb->keycodes[keycode_F12];

	kb->scancodes[scancode_escape] = &kb->keycodes[keycode_escape];
	kb->scancodes[scancode_lctrl] = &kb->keycodes[keycode_lctrl];
	//kb->scancodes[scancode_rctrl] = &kb->keycodes[keycode_rctrl];
	kb->scancodes[scancode_lshift] = &kb->keycodes[keycode_lshift];
	kb->scancodes[scancode_rshift] = &kb->keycodes[keycode_rshift];
	kb->scancodes[scancode_lalt] = &kb->keycodes[keycode_lalt];
	kb->scancodes[scancode_ralt] = &kb->keycodes[keycode_ralt];
	kb->scancodes[scancode_tab] = &kb->keycodes[keycode_tab];
	kb->scancodes[scancode_capslock] = &kb->keycodes[keycode_capslock];

	//kb->scancodes[scancode_delete] = &kb->keycodes[keycode_delete];
	kb->scancodes[scancode_backspace] = &kb->keycodes[keycode_backspace];
	kb->scancodes[scancode_enter] = &kb->keycodes[keycode_enter];

	//kb->scancodes[scancode_arrow_up] = &kb->keycodes[keycode_arrow_up];
	//kb->scancodes[scancode_arrow_down] = &kb->keycodes[keycode_arrow_down];
	//kb->scancodes[scancode_arrow_left] = &kb->keycodes[keycode_arrow_left];
	//kb->scancodes[scancode_arrow_right] = &kb->keycodes[keycode_arrow_right];
	//kb->scancodes[scancode_page_down] = &kb->keycodes[keycode_page_down];
	//kb->scancodes[scancode_page_up] = &kb->keycodes[keycode_page_up];

	kb->scancodes[scancode_back_tick] = &kb->keycodes['`'];
	kb->scancodes[scancode_1] = &kb->keycodes['1'];
	kb->scancodes[scancode_2] = &kb->keycodes['2'];
	kb->scancodes[scancode_3] = &kb->keycodes['3'];
	kb->scancodes[scancode_4] = &kb->keycodes['4'];
	kb->scancodes[scancode_5] = &kb->keycodes['5'];
	kb->scancodes[scancode_6] = &kb->keycodes['6'];
	kb->scancodes[scancode_7] = &kb->keycodes['7'];
	kb->scancodes[scancode_8] = &kb->keycodes['8'];
	kb->scancodes[scancode_9] = &kb->keycodes['9'];
	kb->scancodes[scancode_0] = &kb->keycodes['0'];
	kb->scancodes[scancode_minus] = &kb->keycodes['-'];
	kb->scancodes[scancode_equal] = &kb->keycodes['='];

	kb->scancodes[scancode_Q] = &kb->keycodes['q'];
	kb->scancodes[scancode_W] = &kb->keycodes['w'];
	kb->scancodes[scancode_E] = &kb->keycodes['e'];
	kb->scancodes[scancode_R] = &kb->keycodes['r'];
	kb->scancodes[scancode_T] = &kb->keycodes['t'];
	kb->scancodes[scancode_Y] = &kb->keycodes['y'];
	kb->scancodes[scancode_U] = &kb->keycodes['u'];
	kb->scancodes[scancode_I] = &kb->keycodes['i'];
	kb->scancodes[scancode_O] = &kb->keycodes['o'];
	kb->scancodes[scancode_P] = &kb->keycodes['p'];
	kb->scancodes[scancode_sq_br] = &kb->keycodes['['];
	kb->scancodes[scancode_sq_br_end] = &kb->keycodes[']'];

	kb->scancodes[scancode_A] = &kb->keycodes['a'];
	kb->scancodes[scancode_S] = &kb->keycodes['s'];
	kb->scancodes[scancode_D] = &kb->keycodes['d'];
	kb->scancodes[scancode_F] = &kb->keycodes['f'];
	kb->scancodes[scancode_G] = &kb->keycodes['g'];
	kb->scancodes[scancode_H] = &kb->keycodes['h'];
	kb->scancodes[scancode_J] = &kb->keycodes['j'];
	kb->scancodes[scancode_K] = &kb->keycodes['k'];
	kb->scancodes[scancode_L] = &kb->keycodes['l'];
	kb->scancodes[scancode_semicolon] = &kb->keycodes[';'];
	kb->scancodes[scancode_quote] = &kb->keycodes['\''];
	kb->scancodes[scancode_backslash] = &kb->keycodes['\\'];

	kb->scancodes[scancode_Z] = &kb->keycodes['z'];
	kb->scancodes[scancode_X] = &kb->keycodes['x'];
	kb->scancodes[scancode_C] = &kb->keycodes['c'];
	kb->scancodes[scancode_V] = &kb->keycodes['v'];
	kb->scancodes[scancode_B] = &kb->keycodes['b'];
	kb->scancodes[scancode_N] = &kb->keycodes['n'];
	kb->scancodes[scancode_M] = &kb->keycodes['m'];
	kb->scancodes[scancode_comma] = &kb->keycodes[','];
	kb->scancodes[scancode_period] = &kb->keycodes['.'];
	kb->scancodes[scancode_slash] = &kb->keycodes['/'];

	kb->scancodes[scancode_space] = &kb->keycodes[' '];

	//kb->scancodes[scancode_np_enter] = &kb->keycodes[keycode_np_enter];
	//kb->scancodes[scancode_np_plus] = &kb->keycodes[keycode_np_plus];
	kb->scancodes[scancode_np_minus] = &kb->keycodes[keycode_np_minus];
	kb->scancodes[scancode_np_asterisk] = &kb->keycodes[keycode_np_asterisk];
	//kb->scancodes[scancode_np_slash] = &kb->keycodes[keycode_np_slash];
	kb->scancodes[scancode_np_1] = &kb->keycodes[keycode_np_1];
	kb->scancodes[scancode_np_2] = &kb->keycodes[keycode_np_2];
	kb->scancodes[scancode_np_3] = &kb->keycodes[keycode_np_3];
	kb->scancodes[scancode_np_4] = &kb->keycodes[keycode_np_4];
	kb->scancodes[scancode_np_5] = &kb->keycodes[keycode_np_5];
	kb->scancodes[scancode_np_6] = &kb->keycodes[keycode_np_6];
	kb->scancodes[scancode_np_7] = &kb->keycodes[keycode_np_7];
	kb->scancodes[scancode_np_8] = &kb->keycodes[keycode_np_8];
	kb->scancodes[scancode_np_9] = &kb->keycodes[keycode_np_9];
	kb->scancodes[scancode_np_0] = &kb->keycodes[keycode_np_0];
	kb->scancodes[scancode_np_period] = &kb->keycodes[keycode_np_period];

	kb->scancodes[scancode_super] = (kb->scancodes[scancode_super_] = &kb->keycodes[keycode_super]);


}