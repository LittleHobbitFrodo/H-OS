//
//	utilities/lib/mdc/mdc.hpp
//		part of the build system belonging to the H-OS project
//

//	MDC stands for MarkDown Configuration
	//	simply mark down format used for configuration

//	datatypes in mdc namespace can be defined in preprocessor with #define HCF_<type>_T <type>


#pragma once

#include <iostream>
#include <fstream>
#include <vector>

#ifndef MDC_STRING_T
	#define MDC_STRING_T std::string
#endif
#ifndef MDC_BOOLEAN_T
	#define MDC_BOOLEAN_T bool
#endif
#ifndef MDC_INTEGER_T
	#define MDC_INTEGER_T ssize_t
#endif
#ifndef MDC_DECIMAL_T
	#define MDC_DECIMAL_T double
#endif


namespace mdc {

	class exception : std::exception {
	public:
		enum code : unsigned char {
			unknown_exception = 0,
			incompatible_value_type = 0b1,
			no_value = 0b10,
		};
		enum code code;

		inline exception() {
			this->code = mdc::exception::code::unknown_exception;
		}

		inline exception(enum mdc::exception::code code) {
			this->code = code;
		}
	};

	typedef MDC_STRING_T string;
	typedef MDC_BOOLEAN_T boolean;
	typedef MDC_INTEGER_T integer;
	typedef MDC_DECIMAL_T decimal;

}

#include "./value.hpp"
#include "./section.hpp"