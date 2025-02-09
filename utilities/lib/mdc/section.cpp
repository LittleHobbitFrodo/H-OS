//
//	utilities/lib/mdc/section.cpp
//		part of the build system belonging to the H-OS project
//

#pragma once

mdc::section::pointer::pointer() {
	this->ptr.i = 0;
}

mdc::section::pointer::pointer(mdc::section *sec) {
	this->ptr.sec = sec;
	this->is_val = false;
}

mdc::section::pointer::pointer(mdc::value *val) {
	this->ptr.val = val;
	this->is_val = true;
}

template<typename T>
mdc::section::pointer::pointer(mdc::string name, const T val) {
	if constexpr (mdc::is_mdc_type<T>() || std::is_integral_v<T> || std::is_floating_point_v<T>) {
		this->ptr.val = new mdc::value(name, val);
		this->is_val = true;
	} else {
		static_assert(false, "incompatible type for mdc::section::pointer<T>()");
	}
}

template<typename T>
T& mdc::section::pointer::get() const {
	if constexpr (std::is_same<T, mdc::value>()) {
		if (this->is_val) {
			return *this->ptr.val;
		}
		throw mdc::exception(exception::no_value);
	} else if constexpr (std::is_same<T, mdc::section>()) {
		if (!this->is_val) {
			return *this->ptr.sec;
		}
		throw mdc::exception(exception::no_value);
	} else {
		static_assert(false, "incompatible type for mdc::section::pointer<T>() (expected mdc::value or mdc::section)");
	}
}

mdc::section::pointer::~pointer() {
	if (this->is_val) {
		delete this->ptr.val;
	} else {
		delete this->ptr.sec;
	}
	this->ptr.i = 0;
}


mdc::section::section(mdc::string name) {
	this->nm = mdc::string(name);
	this->map = std::unordered_map<mdc::string, mdc::section::pointer>();
}

void mdc::section::add(mdc::string name, mdc::value& value) {
	this->map.insert_or_assign(name, pointer(new mdc::value(value)));
}

mdc::section& mdc::section::add(mdc::string name) {
	this->map.insert_or_assign(name, pointer(new mdc::section(name)));
	return (mdc::section&)map.at(name);
}

template<typename T>
void mdc::section::add(mdc::string name, const T val) {
	if constexpr (mdc::is_mdc_type<T>() || std::is_integral_v<T> || std::is_floating_point_v<T>) {
		this->map.insert_or_assign(name, pointer(new mdc::value(name, val)));
	} else {
		static_assert(false, "incompatible type for mdc::section::add<T>(string, T)");
	}
}

const mdc::section::pointer& mdc::section::at(mdc::string name) const {
	return this->map.at(name);
}

const mdc::section::pointer& mdc::section::operator[](mdc::string name) const {
	return this->map.at(name);
}

template<typename T>
const T& mdc::section::at(mdc::string name) const {
	mdc::section::pointer& ptr = (mdc::section::pointer&)this->map.at(name);
	if constexpr (std::is_same<T, mdc::value>()) {
		if (ptr.is_value()) {
			return ptr.get<mdc::value>();
		}
		throw mdc::exception(exception::incompatible_value_type);
	} else if constexpr (std::is_same<T, mdc::section>()) {
		if (ptr.is_value()) {
			throw mdc::exception(exception::incompatible_value_type);
		}
		return ptr.get<mdc::section>();
	} else {
		static_assert(false, "incompatible type for mdc::section::at<T>() (expected mdc::value or mdc::section)");
	}
}

/*mdc::section::section(std::string& name) : nm(name) {
	this->values = std::unordered_map<mdc::string, mdc::value>();
	this->sections = std::unordered_map<mdc::string, mdc::section>();
}

void mdc::section::add(mdc::string name, const mdc::value& val) {
	//TODO:	check if name exists
	std::pair<mdc::string, mdc::value> pair = std::pair<mdc::string, mdc::value>(name, val);
	this->values.insert(pair);
}

template<typename T>
void mdc::section::add(mdc::string name, const T val) {
	//TODO:	check if name exists
	if constexpr (mdc::is_mdc_type<T>()) {
		std::pair<mdc::string, mdc::value> pair = std::pair<mdc::string, mdc::value>(name, mdc::value(val));
		this->values.insert(pair);
	} else {
		static_assert(false, "incompatible type for mdc::section::add(T value)");
	}
}

void mdc::section::add(mdc::string& name) {
	//TODO:	check if name exists
	std::pair<mdc::string, mdc::section> pair = std::pair<mdc::string, mdc::section>(name, mdc::section(name));
	this->sections.insert(pair);
}

template<typename T>
size_t mdc::section::size() const noexcept {
	if constexpr (std::is_same<T, mdc::value>()) {
		return this->values.size();
	} else if constexpr (std::is_same<T, mdc::section>()) {
		return this->sections.size();
	} else {
		static_assert(false, "incompatible type for mdc::section::size<T>() (expected mdc::value or mdc::section)");
	}
}

template<typename T>
const T& mdc::section::find(mdc::string name) const {
	if constexpr (std::is_same<T, mdc::value>()) {
		return this->values.at(name);
	} else if constexpr (std::is_same<T, mdc::section>()) {
		return this->sections.at(name);
	} else {
		static_assert(false, "incompatible type for mdc::section::find<T>() (expected mdc::value or mdc::section)");
	}
}

template<typename T>
bool mdc::section::empty() const noexcept {
	if constexpr (std::is_same<T, mdc::value>()) {
		return this->values.empty();
	} else if constexpr (std::is_same<T, mdc::section>()) {
		return this->sections.empty();
	} else {
		static_assert(false, "incompatible type for mdc::section::empty<T>() (expected mdc::value or mdc::section)");
	}
}*/