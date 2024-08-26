#ifndef H_OS_LIB_TRAITS_H
	#define H_OS_LIB_TRAITS_H
#include <utility>

namespace type {
	template<typename T, typename U> struct is_same {
		static const bool value = false;
	};
	template<typename T> struct is_same<T, T> {
		static const bool value = true;
	};
	template<typename T> struct is_same<T, T&> {
		static const bool value = true;
	};

	//  integral type check
	template<typename T> struct is_int {
		static const bool value = false;
	};
	template<typename T> struct is_int<T&> {
		static const bool value = false;
	};
	template<> struct is_int<i8> {
		static const bool value = true;
	};
	template<> struct is_int<i8&> {
		static const bool value = true;
	};
	template<> struct is_int<u8> {
		static const bool value = true;
	};
	template<> struct is_int<u8&> {
		static const bool value = true;
	};
	template<> struct is_int<i16> {
		static const bool value = true;
	};
	template<> struct is_int<i16&> {
		static const bool value = true;
	};
	template<> struct is_int<u16> {
		static const bool value = true;
	};
	template<> struct is_int<i32> {
		static const bool value = true;
	};
	template<> struct is_int<i32&> {
		static const bool value = true;
	};
	template<> struct is_int<u32> {
		static const bool value = true;
	};
	template<> struct is_int<u32&> {
		static const bool value = true;
	};
	template<> struct is_int<i64> {
		static const bool value = true;
	};
	template<> struct is_int<i64&> {
		static const bool value = true;
	};
	template<> struct is_int<u64> {
		static const bool value = true;
	};
	template<> struct is_int<u64&> {
		static const bool value = true;
	};
	template<> struct is_int<i128> {
		static const bool value = true;
	};
	template<> struct is_int<i128&> {
		static const bool value = true;
	};
	template<> struct is_int<u128> {
		static const bool value = true;
	};
	template<> struct is_int<u128&> {
		static const bool value = true;
	};

	//  floating point check
	template<typename T> struct is_float {
		static const bool value = false;
	};
	template<> struct is_float<float> {
		static const bool value = true;
	};
	template<> struct is_float<float&> {
		static const bool value = true;
	};
	template<> struct is_float<double> {
		static const bool value = true;
	};
	template<> struct is_float<double&> {
		static const bool value = true;
	};
	template<> struct is_float<long double> {
		static const bool value = true;
	};
	template<> struct is_float<long double&> {
		static const bool value = true;
	};

	//  unsigned integer check
	template<typename T> struct is_unsigned {
		static const bool value = false;
	};
	template<> struct is_unsigned<u8> {
		static const bool value = true;
	};
	template<> struct is_unsigned<u8&> {
		static const bool value = true;
	};
	template<> struct is_unsigned<u16> {
		static const bool value = true;
	};
	template<> struct is_unsigned<u16&> {
		static const bool value = true;
	};
	template<> struct is_unsigned<u32> {
		static const bool value = true;
	};
	template<> struct is_unsigned<u32&> {
		static const bool value = true;
	};
	template<> struct is_unsigned<u64> {
		static const bool value = true;
	};
	template<> struct is_unsigned<u64&> {
		static const bool value = true;
	};
	template<> struct is_unsigned<u128> {
		static const bool value = true;
	};
	template<> struct is_unsigned<u128&> {
		static const bool value = true;
	};

	//  pointer check
	template<typename T> struct is_ptr {
		static const bool value = false;
	};
	template<typename T> struct is_ptr<T*> {
		static const bool value = true;
	};
	template<typename T> struct is_ptr<T*&> {
		static const bool value = true;
	};


	//  enable
	template<bool B, typename T> struct enable {
		typedef void valid;
	};
	template<typename T> struct enable<true, T> {
		typedef T valid;
	};

	//	if
	template<bool B, typename T, typename F> struct if_ {
		typedef F type;
	};

	template<typename T, typename F> struct if_<true, T, F> {
		typedef T type;
	};
	/*template<bool B, typename T, typename F> class if_<B, T, F> {
		typedef F type;
	};
	template<bool B, typename T, typename F> class if_<true, T, F> {
		typedef T type;
	};
	template<bool B, typename T, typename F> class if_<false, T, F> {
		typedef F type;
	};*/

		//  reference check
		template<typename T> struct is_ref {
			static const bool value = false;
		};
		template<typename T> struct is_ref<T&> {
			static const bool value = true;
		};

		template<typename T> struct is_const {
			static const bool value = false;
		};

		template<typename T> struct is_const<const T> {
			static const bool value = true;
		};

		#ifndef H_OS_TYPE_NAME_WITH_BASE_NAME
			#define H_OS_TYPE_NAME_WITH_BASE_NAME false
		#endif

		//  typename at comptime
		template<typename T> constexpr const char* name() {
			return "unknown";
		}
		template<> constexpr const char* name<i8>() {
			#if H_OS_TYPE_NAME_WITH_BASE_NAME == true
				return "i8 (char)";
			#else
				return "i8";
			#endif
		}
		template<> constexpr const char* name<u8>() {
			#if H_OS_TYPE_NAME_WITH_BASE_NAME == true
				return "u8 (unsigned char)";
			#else
				return "u8";
			#endif
		}
		template<> constexpr const char* name<i16>() {
			#if H_OS_TYPE_NAME_WITH_BASE_NAME == true
				return "i16 (short)";
			#else
				return "i16";
			#endif
		}
		template<> constexpr const char* name<u16>() {
			#if H_OS_TYPE_NAME_WITH_BASE_NAME == true
				return "u16 (unsigned short)";
			#else
				return "u16";
			#endif
		}
		template<> constexpr const char* name<i32>() {
			#if H_OS_TYPE_NAME_WITH_BASE_NAME == true
				return "i32 (int)";
			#else
				return "i32";
			#endif
		}
		template<> constexpr const char* name<u32>() {
			#if H_OS_TYPE_NAME_WITH_BASE_NAME == true
				return "u32 (unsigned int)";
			#else
				return "u32";
			#endif
		}
		template<> constexpr const char* name<i64>() {
			#if H_OS_TYPE_NAME_WITH_BASE_NAME == true
				return "i64 (long)";
			#else
				return "i64";
			#endif
		}
		template<> constexpr const char* name<u64>() {
			#if H_OS_TYPE_NAME_WITH_BASE_NAME == true
				return "u64 (unsigned long)";
			#else
				return "u64";
			#endif
		}
		template<> constexpr const char* name<i128>() {
			#if H_OS_TYPE_NAME_WITH_BASE_NAME == true
				return "i128 (long long)";
			#else
				return "i128";
			#endif
		}
		template<> constexpr const char* name<u128>() {
			#if H_OS_TYPE_NAME_WITH_BASE_NAME == true
				return "u128 (unsigned long long)";
			#else
				return "u128";
			#endif
		}

	}


#endif