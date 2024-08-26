//
//  output.cpp
//      part of the H-OS project
//

#ifdef H_OS_LIB_OUTPUT_H

	#ifndef H_OS_LIB_OUTPUT_CPP
		#define H_OS_LIB_OUTPUT_CPP

		//  screen
		inline u64 screen::width() {
			return mem::framebuffer.response->framebuffers[(out::use_fb < mem::framebuffer.response->framebuffer_count) * out::use_fb]->width;
		}

		inline u64 screen::height() {
			return mem::framebuffer.response->framebuffers[(out::use_fb < mem::framebuffer.response->framebuffer_count) * out::use_fb]->height;
		}

		inline u16 screen::bpp() {
			return mem::framebuffer.response->framebuffers[(out::use_fb < mem::framebuffer.response->framebuffer_count) * out::use_fb]->bpp;
		}

		inline void* screen::address() {
			return mem::framebuffer.response->framebuffers[(out::use_fb < mem::framebuffer.response->framebuffer_count) * out::use_fb]->address;
		}

		inline void screen::flush() {
			u32* ptr = (u32*)screen::address();
			u64 len = screen::width() * screen::height();
			for (size_t i = 0; i < len; i++) {
				ptr[i] = 0x0;
			}
			out::line = 0;
			out::column = 0;
		}

		inline void out::actions::endl() {
			u64 w = screen::width();
			out::line += (out::column / w) + (1 * (out::column < w));
			out::column = 0;
		}

		inline void out::actions::tab() {
			//out::index += OUT_TAB_SPACE_COUNT - (out::index % OUT_TAB_SPACE_COUNT);
			out::column += OUT_TAB_SPACE_COUNT - (out::column % OUT_TAB_SPACE_COUNT);
			if (out::column >= screen::width()) {
				out::actions::endl();
			}
		}

		inline void out::printc(char c) {
			if (c <= 0) {
				return;
			}
			switch (c) {
				case '\n': {
					out::actions::endl();
					return;
				}
				case ' ': {
					if (++out::column >= screen::width()) {
						out::actions::endl();
					}
					return;
				}
				case '\t': {
					out::actions::tab();
					break;
				}
				default: {
					u8 actual = c * (c > font::place_sub) - (font::place_sub * (c > font::place_sub));
					u64 w = screen::width();
					u32* ptr = (u32*)screen::address() + ((out::line * w * (font::h + out::space_between_lines))) + (out::column * font::w);

					for (u16 i = 0; i < font::h; i++) {
						for (u16 ii = 0; ii < font::h; ii++) {
							*(ptr + (i * w) + (font::h - ii)) = out::color * ((font::table[actual][i] >> ii) & 1);
						}
					}
					if (++out::column >= w) {
						out::actions::endl();
					}
					return;
				}
			}
		}

		const out::null_t& out::change(u32 color) {
			out::color = color;
			return out::null;
		}

		template<typename T> inline T& out::hex(T& data, bool yes) {
			output.hex = yes;
			return data;
		}

		inline const out::null_t& out::hex(bool yes) {
			output.hex = yes;
			return out::null;
		}

		inline void out::print(const char* str) {
			for (; *str != '\0'; str++) {
				out::printc(*str);
			}
		}

		inline void out::printl(const char* str) {
			for (; *str != '\0'; str++) {
				out::printc(*str);
			}
			out::actions::endl();
		}

		inline struct output& output::operator<<(const char* str) {
			out::print(str);
			return *this;
		}

		inline struct output& output::operator<<(const char c) {
			out::printc(c);
			return *this;
		}

		template<typename T> inline struct output& output::operator<<(T* ptr) {
			char str[conv::str::hexlen<void*>()];
			conv::to_hex(ptr, (char*)&str);
			out::print((const char*)str);
			return *this;
		}

		inline struct output& output::operator<<(const bool& boolean) {
			out::print((boolean)? "true" : "false");
			return *this;
		}

		inline struct output& output::operator<<(const string& str) {
			if (str.data() != nullptr) {
				for (u32 i = 0; i < str.size(); i++) {
					out::printc(str[i]);
				}
			} else {
				out::print("(null)");
			}
			return *this;
		}

		inline struct output& output::operator<<(const out::endl_t&) {
			out::actions::endl();
			return *this;
		}

		inline struct output& output::operator<<(const out::null_t&) {
			return *this;
		}

		inline struct output& output::operator<<(const mem::byte_t& bt) {
			char str[conv::str::intlen<size_t>()];
			conv::to_string<size_t>(bt.count, (char*)&str, conv::str::intlen<size_t>());
			out::print((const char*)&str);
			out::print("B");
			return *this;
		}

		inline struct output& output::operator<<(const mem::kb_t& kb) {
			char str[conv::str::intlen<size_t>()];
			conv::to_string<size_t>(kb.count, (char*)&str, conv::str::intlen<size_t>());
			out::print((const char*)&str);
			out::print("KB");
			return *this;
		}

		inline struct output& output::operator<<(const mem::mb_t& mb) {
			char str[conv::str::intlen<size_t>()];
			conv::to_string<size_t>(mb.count, (char*)&str, conv::str::intlen<size_t>());
			out::print((const char*)&str);
			out::print("MB");
			return *this;
		}

		inline struct output& output::operator<<(const mem::gb_t& gb) {
			char str[conv::str::intlen<size_t>()];
			conv::to_string<size_t>(gb.count, (char*)&str, conv::str::intlen<size_t>());
			out::print((const char*)&str);
			out::print("GB");
			return *this;
		}

		template<typename T> inline typename type::enable<type::is_int<T>::value, struct output&>::valid output::operator<<(const out::binary<T>& bin) {
			for (ssize_t i = (sizeof(T) * 8) - 1; i >= 0; --i) {
				out::printc(((bin.ref & (1 << i)) != 0)? '1' : '0');
			}
			return *this;
		}

		template<typename T> typename type::enable<type::is_int<T>::value && !type::is_same<T, char>::value, struct output&>::valid inline operator<<(struct output& o, T num) {
			if (o.hex) {
				char str[conv::str::hexlen<T>()];
				conv::to_hex<T>(num, (char*)&str);
				out::print((const char*)&str);
			} else {
				char str[conv::str::intlen<T>()];
				conv::to_string<T>(num, (char*)&str, conv::str::intlen<T>());
				out::print((const char*)&str);
			}
			return o;
		}

	#else
		#warning output.cpp already inluded
	#endif

#else
	#error output.cpp: output.h not included
#endif