//
//  math.cpp
//      part of the H-OS project
//

#ifdef H_OS_LIB_MATH_H
	#ifndef H_OS_LIB_MATH_CPP
		#define H_OS_LIB_MATH_CPP

		//  constexpr

		template<typename T> constexpr size_t math::comptime::logn(T x, size_t n) {
			if constexpr (type::is_int<T>::value) {
				return (x <= 0)? 0 : math::comptime::help::logn<T>(x/n, 1, n);
			} else {
				return 0;
			}
		}

		template<typename T> constexpr size_t math::comptime::help::logn(T x, size_t exp, size_t n) {
			return (x <= 0)? exp : math::comptime::help::logn<T>(x/n, exp+1, n);
		}


		template<typename T> size_t math::logn(T x, size_t n) {
			if constexpr (type::is_int<T>::value) {
				size_t exp = 0;
				for (; x > 0; x /= n) {
					exp++;
				}
				return exp;
			} else {
				return 0;
			}
		}

	#else
		#warning math.cpp already included
	#endif
#else
	#error math.cpp: math.h not included
#endif