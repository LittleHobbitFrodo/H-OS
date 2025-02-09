//
//	utilities/lib/mdc/section.hpp
//		part of the build system belonging to the H-OS project
//

#pragma once

#include "./mdc.hpp"
#include <unordered_map>

//TODO:	check if name exists (::add())

namespace mdc {

	class section {

	public:

		class pointer {

			union {
				mdc::section* sec;
				mdc::value* val;
				size_t i;
			} ptr;

			bool is_val;
				//	true => mdc::value is allocated in .ptr.val
				//	false => mdc::section is allocated in .ptr.sec

		public:

			pointer();
			inline explicit pointer(mdc::section* sec);
			inline explicit pointer(mdc::value* val);
			template<typename T>
			inline explicit pointer(mdc::string name, const T val);

			inline bool is_value() const noexcept {return this->is_val;};

			template<typename T = mdc::value>
			inline T& get() const;

			~pointer();

		};


		explicit section(mdc::string name);

		inline void add(mdc::string name, mdc::value& value);
		inline mdc::section& add(mdc::string name);		//	adds section

		inline void add(mdc::value* val);
		inline void add(mdc::section& sec);

		template<typename T>
		inline void add(mdc::string name, const T val);

		inline const mdc::section::pointer& at(mdc::string name) const;
		inline const mdc::section::pointer& operator[](mdc::string name) const;

		template<typename T>
		inline const T& at(mdc::string name) const;
			//	returns object or throws mdc::exception

		inline size_t size() const noexcept {return this->map.size();}
		inline bool empty() const noexcept {return this->map.empty();};

		[[nodiscard]] const mdc::string& name() const noexcept {return this->nm;}


	private:

		mdc::string nm;
		std::unordered_map<mdc::string, mdc::section::pointer> map;

	};

	/*class section {
	private:

		mdc::string& nm;
		std::unordered_map<mdc::string, mdc::value> values;
		std::unordered_map<mdc::string, mdc::section> sections;

	public:
		explicit section(std::string& name);

		inline void add(mdc::string name, const mdc::value& val);
		template<typename T>
		inline void add(mdc::string name, const T val);

		inline void add(mdc::string& name);
			//	adds section

		template<typename T = mdc::value>
		inline size_t size() const noexcept;


		template<typename T = mdc::value>
		const T& find(mdc::string name) const;
			//	finds value or section
			//	if it is not present returns nullptr
		template<typename T = mdc::value>
		inline bool empty() const noexcept;

	};*/
}

#include "./section.cpp"