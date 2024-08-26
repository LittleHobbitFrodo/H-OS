//
//  string.cpp
//      part of the H-OS project
//

#ifdef H_OS_LIB_STRING_H
	#ifndef H_OS_LIB_STRING_CPP
		#define H_OS_LIB_STRING_CPP

		inline size_t string::strlen(const char *str) {
			size_t i = 0;
			for (; str[i] != '\0'; i++);
			return i;
		}


		string::string() {
			this->dt = nullptr;
			this->len = 0;
			this->cto = false;
		}

		string::string(const char* str, bool can_take_over) {
			this->len = string::strlen(str);
			this->dt = (char*)mem::heap::alloc(this->len + string::alloc_add);
			for (u32 i = 0; i < this->len; i++) {
				this->dt[i] = str[i];
			}
			this->cto = can_take_over;
		}

		string::string(const char *s, size_t sz, bool can_take_over) {
			this->len = sz-1;
			this->dt = (char*)mem::heap::alloc(this->len + string::alloc_add);
			for (u32 i = 0; i < this->len; i++) {
				this->dt[i] = s[i];
			}
			this->cto = can_take_over;
		}

		string::string(const char c, bool can_take_over) {
			this->len = 1;
			this->dt = (char*)mem::heap::alloc(string::alloc_add + (1 * (string::alloc_add == 0)));
			this->dt[0] = c;
			this->cto = can_take_over;
		}

		string::string(const string& str, bool can_take_over) {
			if (str.dt == nullptr) {
				this->dt = nullptr;
				this->len = 0;
			} else {
				this->len = str.len;
				this->dt = (char*)mem::heap::alloc(this->len + string::alloc_add);
				for (u32 i = 0; i < this->len; i++) {
					this->dt[i] = str.dt[i];
				}
			}
			this->cto = can_take_over;
		}

		string::string(u32 l, bool can_take_over) {
			if (l > 0) {
				this->dt = (char*)mem::heap::alloc(l + string::alloc_add);
				this->len = l;
			} else {
				this->dt = nullptr;
				this->len = 0;
			}
			this->cto = can_take_over;
		}



		inline void string::push(const char *str) {
			if (this->dt == nullptr) {
				this->len = string::strlen(str);
				this->dt = (char*)mem::heap::alloc(this->len + string::alloc_add);
				for (u32 i = 0; i < this->len; i++) {
					this->dt[i] = str[i];
				}
			} else {
				u32 add = string::strlen(str);
				this->dt = (char*) mem::heap::realloc(this->dt, this->len + add, string::realloc_add);
				this->len += add;
				u32 ii = 0;
				for (u32 i = this->len - add; i < this->len; i++) {
					this->dt[i] = str[ii++];
				}
			}
		}

		inline void string::push(const string &str) {
			if (str.dt != nullptr) {
				if (this->dt == nullptr) {
					this->len = str.len;
					this->dt = (char*)mem::heap::alloc(this->len + string::alloc_add);
					for (u32 i = 0; i < this->len; i++) {
						this->dt[i] = str.dt[i];
					}
				} else {
					this->len += str.len;
					this->dt = (char*)mem::heap::realloc(this->dt, this->len, string::realloc_add);
					for (u32 i = this->len - str.len; i < this->len; i++) {
						this->dt[i] = str.dt[i - this->len];
					}
				}
			}
			/*if (this->dt == nullptr) {
				this->len = str.len;
				this->dt = (char*)mem::heap::alloc(this->len + string::alloc_add);
				for (u32 i = 0; i < this->len; i++) {
					this->dt[i] = str.dt[i];
				}
			} else {
				u32 add = str.len;
				this->dt = (char*) mem::heap::realloc(this->dt, this->len + add, string::realloc_add);
				this->len += add;
				u32 ii = 0;
				for (u32 i = this->len - add; i < this->len; i++) {
					this->dt[i] = str.dt[ii++];
				}
			}*/
		}

		inline void string::push(char c) {
			if (this->dt == nullptr) {
				this->len = 1;
				this->dt = (char*)mem::heap::alloc(string::alloc_add + (1 * (string::alloc_add == 0)));
				this->dt[0] = c;
			} else {
				this->dt = (char*)mem::heap::realloc(this->dt, ++this->len, string::realloc_add);
				this->dt[this->len-1] = c;
			}
		}

		void string::operator=(char c) {
			if (this->dt == nullptr) {
				this->dt = (char*)mem::heap::alloc(string::alloc_add + (1 * (string::alloc_add == 0)));
			} else if (this->len > 4) {
				//  try to find smaller block
				mem::heap::divide_block((struct mem::heap::segment*)((size_t)this->dt - sizeof(struct mem::heap::segment)), 1 + string::alloc_add);
				/*char* dat = (char*)mem::heap::alloc(string::alloc_add + (1 * (string::alloc_add == 0)));
				mem::heap::free(this->dt);
				this->dt = dat;*/
			}
			this->dt[0] = c;
			this->len = 1;
		}

		void string::operator=(const char* str) {
			u32 ln = string::strlen(str);
			if (this->len < ln) {
				this->dt = (char*)mem::heap::realloc(this->dt, ln, string::realloc_add);
			} else if (this->len > ln + string::realloc_add) {
				//  try to find smaller block
				/*char* dat = (char*)mem::heap::alloc(ln + string::alloc_add);
				mem::heap::free(this->dt);
				this->dt = dat;*/
				mem::heap::divide_block((struct mem::heap::segment*)((size_t)this->dt - sizeof(struct mem::heap::segment)), ln + string::alloc_add);
			}
			this->len = ln;
			for (u32 i = 0; i < this->len; i++) {
				this->dt[i] = str[i];
			}
		}

		void string::operator=(const string& str) {
			if (str.dt == nullptr) {
				this->dt = nullptr;
				this->len = 0;
				this->cto = str.cto;
			} else {
				if (this->len < str.len) {
					this->dt = (char*)mem::heap::realloc(this->dt, str.len, string::realloc_add);
				} else if (this->len > str.len + string::realloc_add) {
					//  try to find smaller block
					/*char* dat = (char*)mem::heap::alloc(str.len + string::alloc_add);
					mem::heap::free(this->dt);
					this->dt = dat;*/
					mem::heap::divide_block((struct mem::heap::segment*)((size_t)this->dt - sizeof(struct mem::heap::segment)), str.len + string::alloc_add);
				}
				this->len = str.len;
				for (u32 i = 0; i < this->len; i++) {
					this->dt[i] = str[i];
				}
			}
		}

		inline void string::operator+=(char c) {
			this->push(c);
		}

		inline void string::operator+=(const char *str) {
			this->push(str);
		}

		inline void string::operator+=(const string& str) {
			this->push(str);
		}

		inline bool string::operator==(const char c) const {
			return (this->len == 1) && (this->dt[0] == c);
		}

		inline bool string::operator==(const char* str) const {
			if (string::strlen(str) != this->len) {
				return false;
			}
			for (u32 i = 0; i < this->len; i++) {
				if (str[i] != this->dt[i]) {
					return false;
				}
			}
			return true;
		}

		bool string::operator==(const string& str) const {
			if (str.len != this->len) {
				return false;
			}
			for (u32 i = 0; i < this->len; i++) {
				if (this->dt[i] != str[i]) {
					return false;
				}
			}
			return true;
		}

		bool string::operator!=(const char* str) const {
			if (string::strlen(str) != this->len) {
				return true;
			}
			for (u32 i = 0; i < this->len; i++) {
				if (this->dt[i] != str[i]) {
					return true;
				}
			}
			return false;
		}

		bool string::operator!=(const string& str) const {
			if (str.len != this->len) {
				return true;
			}
			for (u32 i = 0; i < this->len; i++) {
				if (this->dt[i] != str.dt[i]) {
					return true;
				}
			}
			return false;
		}

		i8 string::cmp(const char *str) {
			u32 ln = string::strlen(str) - 1;
			if (this->len < ln) {
				return -1;
			} else if (this->len > ln) {
				return 1;
			}
			for (u32 i = 0; i < this->len; i++) {
				if (this->dt[i] != str[i]) {
					return (i8)(((this->dt[i] > str[i]) * 2) - 1);
					//  dt[i] > -> 2 - 1 = 1
					//  dt[i] < -> 0 - 1 = -1
				}
			}
			return 0;
		}

		i8 string::cmp(const string &str) {
			if (this->len < str.len) {
				return -1;
			} else if (this->len > str.len) {
				return 1;
			}
			for (u32 i = 0; i < this->len; i++) {
				if (this->dt[i] != str.dt[i]) {
					return (i8)(((this->dt[i] > str.dt[i]) * 2) - 1);
					//  this->dt[i] > -> 2 - 1 = 1
					//  this->dt[i] < -> 0 - 1 = -1
				}
			}
			return 0;
		}


		inline void string::clear() {
			if (this->dt != nullptr) {
				mem::heap::free(this->dt);
			}
			this->dt = nullptr;
			this->len = 0;
		}



		inline char& string::at(u32 i) const {
			return this->dt[i];
		}

		inline char& string::operator[](u32 i) const {
			return this->dt[i];
		}

		inline u32 string::size() const {
			return this->len;
		}
		inline const char* string::data() const {
			return this->dt;
		}

		inline void string::pop(u32 count) {
			if (this->dt != nullptr) {
				if (this->len <= count) {
					mem::heap::free(this->dt);
					this->dt = nullptr;
					this->len = 0;
				} else {
					this->len -= count;
					if (mem::heap::block_size(this->dt) - this->len > string::pop_min_realloc) {
						//  abandon the bigger block and find smaller
						/*char* dat = (char*)mem::heap::alloc(this->len + (string::alloc_add / 2));
						for (u32 i = 0; i < this->len; i++) {
							dat[i] = this->dt[i];
						}
						mem::heap::free(this->dt);
						this->dt = dat;*/
						mem::heap::divide_block((struct mem::heap::segment*)((size_t)this->dt - sizeof(struct mem::heap::segment)), this->len + (string::alloc_add / 2));
					}
				}
			}
		}

		inline void string::operator--() {
			if (this->dt != nullptr) {
				if (this->len == 1) {
					this->len = 0;
					mem::heap::free(this->dt);
					this->dt = nullptr;
				} else {
					this->len--;
				}
			}
		}

		inline void string::take_over(string &str) {
			if (str.cto) {
				if (this->dt != nullptr) {
					mem::heap::free(this->dt);
				}
				this->dt = str.dt;
				this->len = str.len;

				str.dt = nullptr;
				str.len = 0;
			}
		}


		inline bool string::can_take_over() const {
			return this->cto;
		}





		string::~string() {
			if (this->dt != nullptr) {
				mem::heap::free(this->dt);
			}
			this->dt = nullptr;
			this->len = 0;
			this->cto = false;
		}

	#else
		#warning string.cpp already included
	#endif
#else
	#error string.cpp: string.h not included
#endif