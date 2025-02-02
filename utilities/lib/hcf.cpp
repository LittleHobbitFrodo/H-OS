//
//	utilities/lib/hcf.cpp
//		part of the build system belonging to the H-OS project
//

#pragma once

#include "./hcf.hpp"

hcf::exception::exception() {
	this->code = hcf::exception::code::unknown_exception;
}

hcf::exception::exception(enum hcf::exception::code code) {
	this->code = code;
}


hcf::property::value::value() {
	this->_type = hcf::property::type::undefined_;
	this->ptr = nullptr;
}

hcf::property::value::value(enum hcf::property::type type) {
	this->_type = type;
	switch (type) {
		case hcf::property::type::string_: {
			this->ptr = new hcf::string;
			break;
		}
		case hcf::property::type::boolean_: {
			this->ptr = new hcf::boolean;
			break;
		}
		case hcf::property::type::integer_: {
			this->ptr = new hcf::integer;
			break;
		}
		case hcf::property::type::decimal_: {
			this->ptr = new hcf::decimal;
			break;
		}
		default: break;
	}
}

template<typename T>
hcf::property::value::value(std::enable_if<hcf::is_hcf_type<T>(), const T &> value) {

	if constexpr (std::is_same<T, hcf::string>()) {
		this->_type = hcf::property::type::string_;
		this->ptr = new hcf::string;
	} else if constexpr (std::is_same<T, hcf::boolean>()) {
		this->_type = hcf::property::type::boolean_;
		this->ptr = new hcf::boolean;
	} else if constexpr (std::is_same<T, hcf::integer>()) {
		this->_type = hcf::property::type::integer_;
		this->ptr = new hcf::integer;
	} else if constexpr (std::is_same<T, hcf::decimal>()) {
		this->_type = hcf::property::type::decimal_;
		this->ptr = new hcf::decimal;
	}
}


template<typename T>
std::enable_if<hcf::is_hcf_type<T>(), const T&> hcf::property::value::read() {
	unsigned char ex = (this->ptr == nullptr) << 1;
	if constexpr (std::is_same<T, hcf::string>()) {
		ex |= (this->type() != hcf::property::type::string_);
		if (ex != 0) {
			throw hcf::exception((enum hcf::exception::code)ex);
		}
		return (const hcf::string&)*((const hcf::string*)this->ptr);
	} else if constexpr (std::is_same<T, hcf::boolean>()) {
		ex |= (this->type() != hcf::property::type::boolean_);
		if (ex != 0) {
			throw hcf::exception((enum hcf::exception::code)ex);
		}
		return (const hcf::boolean&)*((const hcf::boolean*)this->ptr);
	} else if constexpr (std::is_same<T, hcf::integer>()) {
		ex |= (this->type() != hcf::property::type::integer_);
		if (ex != 0) {
			throw hcf::exception((enum hcf::exception::code)ex);
		}
		return (const hcf::integer&)*((const hcf::integer*)this->ptr);
	} else if constexpr (std::is_same<T, hcf::decimal>()) {
		ex |= (this->type() != hcf::property::type::decimal_);
		if (ex != 0) {
			throw hcf::exception((enum hcf::exception::code)ex);
		}
		return (const hcf::decimal &)*((const hcf::decimal*)this->ptr);
	}
}

enum hcf::property::type hcf::property::value::type() {
	return this->_type;
}


hcf::property::value::~value() {
	switch (this->type()) {
		case hcf::property::type::string_:{
			delete ((hcf::string*)this->ptr);
			break;
		}
		case hcf::property::type::boolean_: {
			delete ((hcf::boolean*)this->ptr);
			break;
		}
		case hcf::property::type::integer_: {
			delete ((hcf::integer*)this->ptr);
			break;
		}
		case hcf::property::type::decimal_: {
			delete ((hcf::decimal*)this->ptr);
			break;
		}
		default: break;
	}
}

hcf::property::property() {
	this->name = std::string();
	this->value = hcf::property::value();
}

