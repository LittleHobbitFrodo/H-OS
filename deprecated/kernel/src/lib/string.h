//
//  string.h
//      part of the H-OS project
//

#ifndef H_OS_LIB_STRING_H
	#define H_OS_LIB_STRING_H

	//bool operator==(const char* s1, const char* s2);

	class string {
		private:
			char* dt;
			u32 len;
			bool cto;

		public:

		inline void take_over(string& str);

		inline i8 cmp(const char* str);
		inline i8 cmp(const string& str);

		inline static size_t strlen(const char* str);

		const static u32 alloc_add {8};
			//  allocate few bytes more for better performance while pushing
		const static u32 realloc_add {8};

		const static u32 pop_min_realloc {24};
			//  reallocate the string when block.size - this->size() is lower than this
				//  frees the bigger block and use smaller

		inline u32 size() const;
		inline const char* data() const;

		string();
		string(const char* s, bool can_take_over = false);
		string(const char* s, size_t sz, bool can_take_over = false);
		string(const char c, bool can_take_over = false);
		string(const string& str, bool can_take_over = false);
		string(u32 l, bool can_take_over = false);

		inline void push(const char* str);
		inline void push(const string& str);
		inline void push(char c);

		inline void operator=(char c);
		inline void operator=(const char* str);
		inline void operator=(const string& str);

		inline void operator+=(const char* str);
		inline void operator+=(char c);
		inline void operator+=(const string& str);

		inline bool operator==(const char c) const;
		inline bool operator==(const char* str) const;
		inline bool operator==(const string& str) const;

		inline bool operator!=(const char* str) const;
		inline bool operator!=(const string& str) const;

		inline void clear();

		inline char& at(u32 i) const;
		inline char& operator[](u32 i) const;

		inline void pop(u32 count = 1);
		inline void operator--();

		inline bool can_take_over() const;


		~string();
	};

#else
	#warning string.h already included
#endif