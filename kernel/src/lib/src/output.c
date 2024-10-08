//
//	output.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once


//	initialization functions
void screen_init() {
	//	initialize screen structure
	screen.count = 0;
	screen.address = null;
	screen.bpp = 0;
	screen.h = 0;
	screen.w = 0;

	//	initialize colors
	col.white = 0xffffff;
	col.green = 0x88ff88;
	col.red = 0xff8888;
	col.blue = 0x8888ff;
	col.critical = 0xff0000;

	col.yellow = 0xffff00;
	col.orange = 0xffa500;
	col.cyan = 0x00ffff;
	col.grey = 0xaaaaaa;

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
}

void screen_flush_at(size_t column, size_t line) {
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
	output.line = (output.column = 0);
	output.space_between_lines = SPACE_BETWEEN_LINES_DEFAULT;
	output.fb = 0;
	output.color = col.white;
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

void prints(const string *s) {
	for (size_t i = 0; i < s->size; i++) {
		printc(s->data[i]);
	}
}

void printsl(const string *s) {
	for (size_t i = 0; i < s->size; i++) {
		printc(s->data[i]);
	}
	endl();
}


void printi(const i64 i) {
	char num[INTLEN_I64];
	to_stringi((char *) &num, i);
	print((const char *) &num);
}

void printu(const u64 u) {
	char num[INTLEN_U64];
	to_string((char *) &num, u);
	print((const char *) &num);
}

void printp(void *p) {
	char num[HEXLEN_PTR];
	to_hex((char *) num, p);
	print((const char *) &num);
}

void printb(size_t bin) {
	for (ssize_t i = (sizeof(size_t) * 8) - 1; i >= 0; i--) {
		printc('0' + ((bin >> i) & 1));
	}
}
