//
//	utilities/lib/hcf.hpp
//		part of the build system belonging to the H-OS project
//

//	HCF is configuration file format for H-OS kernel, utilities and build system

//	datatypes in hcf namespace can be defined in preprocessor with #define HCF_<type>_T <type>

#pragma once

#include <iostream>
#include <fstream>
#include <unordered_map>

#ifndef HCF_STRING_T
	#define HCF_STRING_T std::string
#endif
#ifndef HCF_BOOLEAN_T
	#define HCF_BOOLEAN_T bool
#endif
#ifndef HCF_INTEGER_T
	#define HCF_INTEGER_T ssize_t
#endif
#ifndef HCF_DECIMAL_T
	#define HCF_DECIMAL_T double
#endif


namespace hcf {

	class exception : std::exception {
		public:
		enum code : unsigned char {
			unknown_exception = 0,
			incompatible_value_type = 0b1,
			no_value = 0b10
		};
		enum code code;
		inline exception();
		inline exception(enum hcf::exception::code code);
	};

	typedef HCF_STRING_T string;
	typedef HCF_BOOLEAN_T boolean;
	typedef HCF_INTEGER_T integer;
	typedef HCF_DECIMAL_T decimal;

	template<typename T>
	constexpr bool is_hcf_type() {
		return std::is_same<hcf::string, T>() || std::is_same<hcf::boolean, T>()
		        || std::is_same<hcf::integer, T>() || std::is_same<hcf::decimal, T>();
	}


	class property {

	public:
		enum type : unsigned char {
			undefined_,
			string_,
			boolean_,
			integer_,
			decimal_,
		};

		class value {

		public:
			inline value();
			inline value(enum hcf::property::type type);
			template<typename T>
			inline value(std::enable_if<hcf::is_hcf_type<T>(), const T&> value);

			inline void operator()();

		private:

			enum hcf::property::type _type;

			void* ptr;

			template<typename T>
			std::enable_if<hcf::is_hcf_type<T>(), const T&> read();
			inline enum hcf::property::type type();

			inline ~value();

		};

		inline property();
		inline property(hcf::string name, enum hcf::property::type type);
		template<typename T>
		inline property(hcf::string name, std::enable_if<hcf::is_hcf_type<T>(), const T&>);


	private:
		hcf::string name;

		hcf::property::value value;

		~property();

	};



	class config {
		hcf::string path;

		std::unordered_map<hcf::string, hcf::property> properties;
	};
}

#include "./hcf.hpp"