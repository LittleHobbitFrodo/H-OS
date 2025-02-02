//
//	output.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once


//	initialization functions
void screen_init() {

	if (req_fb.response == null) {
		return;
	}

	if (req_fb.response->framebuffer_count < 1) {
		return;
	}

	struct limine_framebuffer *f = req_fb.response->framebuffers[0];

	if (f->memory_model != LIMINE_FRAMEBUFFER_RGB) {
		return;
	}

	//	set screen data
	screen.count = req_fb.response->framebuffer_count;
	screen.bpp = f->bpp;
	screen.address = (u32 *) f->address;
	screen.h = f->height;
	screen.w = f->width;
}

void screen_flush() {
	size_t size = screen.w * screen.h;
	for (size_t i = 0; i < size; i++) {
		screen.address[i] = 0;
	}
	output.column = 0;
	output.line = 0;
}

void screen_flush_at(size_t line, size_t column) {
	if ((column * font.size >= screen.w) || (line * font.size >= screen.h)) {
		return;
	}

	u32* ptr = screen.address + ((line * screen.w * (font.size + output.space_between_lines))) + (column * font.size);
	for (u16 i = 0; i < font.size; i++) {
		for (u16 ii = 0; ii < font.size; ii++) {
			*(ptr + (i * screen.w) + (font.size - ii)) = 0;
		}
	}
}


void output_init() {

	screen_init();

	screen_flush();

	font_init();

	output.line = (output.column = 0);
	output.space_between_lines = SPACE_BETWEEN_LINES_DEFAULT;
	output.fb = 0;
	output.color = col.white;

	switch (font.size) {
		case 8: default: {
			printc = _printc_8;
			break;
		}
		case 16: {
			printc = _printc_16;
			break;
		}
		case 32: {
			printc = _printc_32;
			break;
		}
		case 64: {
			printc = _printc_64;
			break;
		}
	}
}


//	output functions
void print(const char *s) {
	for (size_t i = 0; s[i] != '\0'; i++) {
		printc(s[i]);
	}
}

void printl(const char *s) {
	for (size_t i = 0; s[i] != '\0'; i++) {
		printc(s[i]);
	}
	endl();
}

void printi(const i64 i) {
	char num[INTLEN_I64];
	to_stringi((char*)&num, i);
	print((const char*) &num);
}

void printu(const u64 u) {
	char num[INTLEN_U64];
	to_string((char *) &num, u);
	print((const char *) &num);
}

void printp(void *p) {
	if (p == null) {
		print("NULL");
		return;
	}
	char num[HEXLEN_PTR];
	to_hex((char *) num, p);
	print((const char *) &num);
}

void printh(size_t h) {
	char num[HEXLEN];
	to_hexs((char*)&num, h);
	print((const char*)&num);
}

void printb(size_t bin) {
	for (ssize_t i = (sizeof(size_t) * 8) - 1; i >= 0; i--) {
		printc('0' + ((bin >> i) & 1));
	}
}

void printn(const char* str, size_t n) {
	for (size_t i = 0; i < n; i++) {
		printc(str[i]);
	}
}


void _printc_8(const char c) {
	if ((c < ' ') && (c != '\t') && (c != '\n')) {
		return;
	}
	switch (c) {
		case '\n': {
			endl();
			break;
		}
		case ' ': {
			output.column++;
			if (output.column >= screen.w) {
				endl();
			}
			break;
		}
		case '\t': {
			tab();
			break;
		}
		default: {
			u8 actual = c - FONT_PLACE_SUB;
			u32 *ptr = screen.address + ((output.line * screen.w * (8 + output.space_between_lines))) + (output.column * 8);
			u8 *fnt;
			for (u16 i = 0; i < 8; i++) {
				fnt = font.table[actual];
				for (u16 ii = 0; ii < 8; ii++) {
					*(ptr + (i * screen.w) + (8 - ii)) = output.color * ((fnt[i] >> ii) & 1);
				}
			}

			output.column++;
			if ((output.column * 8) >= screen.w) {
				endl();
			}
		}
	}
}


void _printc_16(const char c) {
	if ((c < ' ') && (c != '\t') && (c != '\n')) {
		return;
	}
	switch (c) {
		case '\n': {
			endl();
			break;
		}
		case ' ': {
			output.column++;
			if (output.column >= screen.w) {
				endl();
			}
			break;
		}
		case '\t': {
			tab();
			break;
		}
		default: {
			u8 actual = c - FONT_PLACE_SUB;
			u32 *ptr = screen.address + ((output.line * screen.w * (16 + output.space_between_lines))) + (output.column * 16);
			u16 *fnt;
			for (u16 i = 0; i < 16; i++) {
				fnt = font.table[actual];
				for (u16 ii = 0; ii < 16; ii++) {
					*(ptr + (i * screen.w) + (16 - ii)) = output.color * ((fnt[i] >> ii) & 1);
				}
			}

			output.column++;
			if ((output.column * 16) >= screen.w) {
				endl();
			}
		}
	}
}

void _printc_32(const char c) {
	if ((c < ' ') && (c != '\t') && (c != '\n')) {
		return;
	}
	switch (c) {
		case '\n': {
			endl();
			break;
		}
		case ' ': {
			output.column++;
			if (output.column >= screen.w) {
				endl();
			}
			break;
		}
		case '\t': {
			tab();
			break;
		}
		default: {
			u8 actual = c - FONT_PLACE_SUB;
			u32 *ptr = screen.address + ((output.line * screen.w * (32 + output.space_between_lines))) + (output.column * 32);

			u32 *fnt;
			for (u16 i = 0; i < 32; i++) {
				fnt = font.table[actual];
				for (u16 ii = 0; ii < 32; ii++) {
					*(ptr + (i * screen.w) + (32 - ii)) = output.color * ((fnt[i] >> ii) & 1);
				}
			}

			output.column++;
			if ((output.column * 32) >= screen.w) {
				endl();
			}
		}
	}
}

void _printc_64(const char c) {
	if ((c < ' ') && (c != '\t') && (c != '\n')) {
		return;
	}
	switch (c) {
		case '\n': {
			endl();
			break;
		}
		case ' ': {
			output.column++;
			if (output.column >= screen.w) {
				endl();
			}
			break;
		}
		case '\t': {
			tab();
			break;
		}
		default: {
			u8 actual = c - FONT_PLACE_SUB;
			u32 *ptr = screen.address + ((output.line * screen.w * (64 + output.space_between_lines))) + (output.column * 64);
			u64 *fnt;
			for (u16 i = 0; i < 64; i++) {
				fnt = font.table[actual];
				for (u16 ii = 0; ii < 64; ii++) {
					*(ptr + (i * screen.w) + (64 - ii)) = output.color * ((fnt[i] >> ii) & 1);
				}
			}

			output.column++;
			if ((output.column * 64) >= screen.w) {
				endl();
			}
		}
	}
}