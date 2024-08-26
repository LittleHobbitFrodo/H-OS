//
//  convert.cpp
//      part of the H-OS project
//

#ifdef H_OS_LIB_CONVERT_H
	#ifndef H_OS_LIB_CONVERT_CPP
		#define H_OS_LIB_CONVERT_CPP
		template<typename T> constexpr size_t conv::str::hexlen() {
			if constexpr (type::is_ptr<T>::value) {
				return (sizeof(void*) * 2) + 3;
			} else {
				return (sizeof(T) * 2) + 3;
			}
		}

		template<typename T> constexpr size_t conv::str::intlen() {
			if constexpr (type::is_int<T>::value) {
				if constexpr (type::is_same<T, i8>::value) {
					return math::comptime::logn<i8>(MAX_I8, 10) + 1;
				} else if constexpr (type::is_same<T, u8>::value) {
					return math::comptime::logn<u8>(MAX_U8, 10);
				} else if constexpr (type::is_same<T, i16>::value) {
					return math::comptime::logn<i16>(MAX_I16, 10) + 1;
				} else if constexpr (type::is_same<T, u16>::value) {
					return math::comptime::logn<u16>(MAX_U16, 10);
				} else if constexpr (type::is_same<T, i32>::value) {
					return math::comptime::logn<i32>(MAX_I32, 10) + 1;
				} else if constexpr (type::is_same<T, u32>::value) {
					return math::comptime::logn<u32>(MAX_U32, 10);
				} else if constexpr (type::is_same<T, i64>::value) {
					return math::comptime::logn<i64>(MAX_I64, 10) + 1;
				} else if constexpr (type::is_same<T, u64>::value) {
					return math::comptime::logn<u64>(MAX_U64, 10);
				} else {
					return 0;
				}
			} else {
				return 0;
			}
		}

		template<typename T> void conv::to_string(T data, char* str, size_t len) {
			if constexpr (type::is_int<T>::value) {
				if (data == 0) {
					str[0] = '0';
					str[1] = '\0';
					return;
				}
				if constexpr (type::is_unsigned<T>::value) {
					//  unsigned types
					size_t i = 0;
					while ((data > 0) && (i + 1 < len)) {
						str[i++] = '0' + (data % 10);
						data /= 10;
					}
					str[i] = '\0';
					utilstr::reverse(str, i);
				} else {
					//  signed types
					size_t i = 0;
					bool negative = data < 0;

					if (negative) {
						data = -data;
					}

					while ((data != 0) && (i + 1 < len)) {
						str[i++] = '0' + (data % 10);
						data /= 10;
					}

					if (negative) {
						str[i++] = '-';
					}
					str[i] = '\0';
					utilstr::reverse(str, i);
				}
			}
		}

		template<typename T> void conv::to_hex(T& data, char *str) {
			str[conv::str::hexlen<T>() - 1] = '\0';
			str[0] = '0';
			str[1] = 'x';

			u8* ptr = (u8*)&data;

			for (size_t i = conv::str::hexlen<T>()-2; i > 1;) {
				str[i--] = conv::hex[(*ptr & 0xf)];
				str[i--] = conv::hex[((*ptr >> 4) & 0xf)];
				ptr++;
			}
		}

		template<typename T> void conv::to_hex(T* data, char *str) {
			str[conv::str::hexlen<void*>() - 1] = '\0';
			str[0] = '0';
			str[1] = 'x';

			u8* ptr = (u8*)&data;

			for (size_t i = conv::str::hexlen<void*>()-2; i > 1;) {
				str[i--] = conv::hex[(*ptr & 0xf)];
				str[i--] = conv::hex[((*ptr >> 4) & 0xf)];
				ptr++;
			}
		}

	#else
		#warning convert.cpp already inluded
	#endif
#else
	#error convert.cpp: convert.h not inluded
#endif