//
//	utilities/lib/mdc/value.hpp
//		part of the build system belonging to the H-OS project
//

#pragma once

#include "./mdc.hpp"
#include <unordered_map>

namespace mdc {
	class value {
	public:

		enum type {
			//	other types may be added later (versions, lists, etc.)
			undefined_ = 0,
			string_,
			boolean_,
			integer_,
			decimal_,
			list_,
			//quote_
		};

		struct list {
			list();
			std::unordered_map<mdc::string, mdc::value::list> lists;
			std::unordered_map<mdc::string, mdc::value> values;
		};

		union pointer {
			//	primitive types (not strings) are allocated in this union
			mdc::string* str;
			mdc::boolean b;
			mdc::integer i;
			mdc::decimal d;
			struct mdc::value::list* list;
		};

		explicit value(mdc::string name, enum mdc::value::type type);
		value(const mdc::value& val);

		template<typename T>
		explicit value(mdc::string name, const T data);

		[[nodiscard]] inline enum mdc::value::type type() const noexcept;

		template<typename T>
		const T& read() const;

		template<bool full_name = true>
		static const char* type_string(enum mdc::value::type type);

		[[nodiscard]] inline const mdc::string& name() const noexcept;

		~value();

	private:

		mdc::string nm;

		union mdc::value::pointer data;
		//	only strings are allocated

		enum value::type tp;

	};

	std::ostream& operator<<(std::ostream& os, const mdc::value& val) {
		os << val.name() << " (" << mdc::value::type_string<false>(val.type()) << ") = ";
		switch (val.type()) {
			case mdc::value::string_: {
				os << val.read<mdc::string>();
				break;
			}
			case mdc::value::boolean_: {
				os << val.read<mdc::boolean>();
				break;
			}
			case mdc::value::integer_: {
				os << val.read<mdc::integer>();
				break;
			}
			case mdc::value::decimal_: {
				os << val.read<mdc::decimal>();
				break;
			}
			case mdc::value::list_: {
				os << "{list}";
				break;
			}
			case mdc::value::undefined_: default: break;
		}
		return os;
	}

	template<typename T>
	constexpr bool is_mdc_type() {
		return std::is_same<mdc::string, T>() || std::is_same<mdc::boolean, T>() || std::is_same<mdc::integer, T>() || std::is_same<mdc::decimal, T>() || std::is_same<mdc::value::list, T>();
	}

}


#include "./value.cpp"