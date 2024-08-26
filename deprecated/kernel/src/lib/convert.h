//
//  convert.h
//      part of the H-OS project
//

#ifndef H_OS_LIB_CONVERT_H
	#define H_OS_LIB_CONVERT_H

	namespace conv {

		const static char* hex = "0123456789abcdef";
		namespace str {
			template<typename T> constexpr size_t hexlen();
			//template<typename T> constexpr size_t hexlen<T*>();

			template<typename T> constexpr size_t intlen();
		}
		template<typename T> void to_string(T data, char* str, size_t len);
		template<typename T> void to_hex(T& data, char* str);
		template<typename T> void to_hex(T* data, char* str);
	}

#else
	#warning convert.h already included
#endif