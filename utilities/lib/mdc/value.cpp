//
//	utilities/lib/mdc.hpp
//		part of the build system belonging to the H-OS project
//

#pragma once

#include "./mdc.hpp"


mdc::value::value(mdc::string name, enum mdc::value::type type) {
	this->nm = name;
	switch (type) {
		case string_: {
			this->data.str = new mdc::string();
			this->tp = string_;
			break;
		}
		case boolean_: {
			this->data.b = false;
			this->tp = boolean_;
			break;
		}
		case integer_: {
			this->data.i = 0;
			this->tp = integer_;
			break;
		}
		case decimal_: {
			this->data.d = 0.0;
			this->tp = decimal_;
			break;
		}
		case list_: {
			this->data.list = new mdc::value::list();
			this->tp = list_;
			break;
		}
		case undefined_: default: {
			this->data.i = 0;
			this->tp = undefined_;
			break;
		}
	}
}

mdc::value::value(const mdc::value& val) {
	switch (val.type()) {
		case string_: {
			this->data.str = new mdc::string(*val.data.str);
			this->tp = string_;
			break;
		}
		case boolean_: {
			this->data.b = val.data.b;
			this->tp = boolean_;
			break;
		}
		case integer_: {
			this->data.i = val.data.i;
			this->tp = integer_;
			break;
		}
		case decimal_: {
			this->data.d = val.data.d;
			this->tp = decimal_;
			break;
		}
		case list_: {
			throw mdc::exception();
		}
		case undefined_: {
			this->data.i = 0;
			this->tp = undefined_;
			break;
		}
	}
}


template<typename T>
mdc::value::value(mdc::string name, const T data) {
	this->nm = name;
	if constexpr (std::is_same<T, mdc::string>()) {
		this->data.str = new mdc::string(data);
		this->tp = string_;
	} else if constexpr (std::is_same<T, mdc::boolean>()) {
		this->data.b = (mdc::boolean)data;
		this->tp = boolean_;
	} else if constexpr (std::is_integral<T>::value) {
		this->data.i = (mdc::integer)data;
		this->tp = integer_;
	} else if constexpr (std::is_floating_point<T>::value) {
		this->data.d = (mdc::decimal)data;
		this->tp = decimal_;
	} else if constexpr (std::is_same<T, mdc::value::list>()) {
		this->data.list = new mdc::value::list();
		this->data.list->lists = data.lists;
		this->data.list->values = data.values;
	} else {
		static_assert(0 == 1, "incompatible type for mdc::value::value<T>(name, data)");
	}
}

enum mdc::value::type mdc::value::type() const noexcept {
	return this->tp;
}

mdc::value::~value() {
	switch (this->type()) {
		case string_: {
			delete this->data.str;
			break;
		}
		case list_: {
			delete this->data.list;
			break;
		}
		default: break;
	}
	this->data.i = 0;
	this->tp = undefined_;
}

template<bool full_name>
const char* mdc::value::type_string(enum mdc::value::type type) {
	if constexpr (full_name) {
		switch (type) {
			case mdc::value::type::undefined_: {
				return "UNDEFINED";
			}
			case mdc::value::type::string_: {
				return "string";
			}
			case mdc::value::type::boolean_: {
				return "boolean";
			}
			case mdc::value::type::integer_: {
				return "integer";
			}
			case mdc::value::type::decimal_: {
				return "decimal";
			}
			case mdc::value::type::list_: {
				return "list";
			}
			default: {
				return "ERROR type";
			}
		}
	} else {
		switch (type) {
			case mdc::value::type::undefined_: {
				return "UNDEF";
			}
			case mdc::value::type::string_: {
				return "str";
			}
			case mdc::value::type::boolean_: {
				return "bool";
			}
			case mdc::value::type::integer_: {
				return "int";
			}
			case mdc::value::type::decimal_: {
				return "dec";
			}
			case mdc::value::type::list_: {
				return "list";
			}
			default: {
				return "ERR";
			}
		}
	}
}

template<typename T>
const T& mdc::value::read() const {
	if constexpr (std::is_same<T, mdc::string>()) {
		if (this->data.str == nullptr) {
			throw mdc::exception(exception::no_value);
		}
		return *this->data.str;
	} else if constexpr (std::is_same<T, mdc::boolean>()) {
		return this->data.b;
	} else if constexpr (std::is_same<T, mdc::integer>()) {
		return this->data.i;
	} else if constexpr (std::is_same<T, mdc::decimal>()) {
		return this->data.d;
	} else if constexpr (std::is_same<T, mdc::value::list>()) {
		if (this->data.list == nullptr) {
			throw mdc::exception(exception::no_value);
		}
		return *this->data.list;
	} else {
		static_assert(0 == 1, "incompatible type for mdc::value::read<T>()");
	}
}

const mdc::string& mdc::value::name() const noexcept {
	return this->nm;
}


mdc::value::list::list() {
	this->lists = std::unordered_map<mdc::string, mdc::value::list>();
	this->values = std::unordered_map<mdc::string, mdc::value>();
}



/*mdc::value::list::list() {
	this->lists = std::vector<mdc::value::list>();
	this->values = std::vector<mdc::value>();
}*/



