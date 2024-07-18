//
//  math.h
//      part of the H-OS project
//

#ifndef H_OS_LIB_MATH_H
	#define H_OS_LIB_MATH_H

	namespace math {
		namespace comptime {
			template<typename T> constexpr size_t logn(T x, size_t n);

			namespace help {
				template<typename T> constexpr size_t logn(T x, size_t exp, size_t n);
			}

		}

		template<typename T> size_t logn(T x, size_t n);

	}

#else
	#warning math.h already included
#endif