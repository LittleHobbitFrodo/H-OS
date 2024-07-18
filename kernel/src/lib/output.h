//
//  output.h
//      part of the H-OS project
//

#ifndef H_OS_LIB_OUTPUT_H
	#define H_OS_LIB_OUTPUT_H

	namespace font {
		void init();

		static u16 h {8};
		static u16 w {8};

		const static u8 place_sub {31};
		const static u8 size = 128 - place_sub;

		static u8 table[font::size][8];
	}

	namespace screen {
		inline u64 width();
		inline u64 height();

		inline u16 bpp();

		inline void flush();

		template<typename T> constexpr u8 scale();

		inline void* address();
	}

#ifndef OUT_TAB_SPACE_COUNT
	#define OUT_TAB_SPACE_COUNT 4
#endif

	namespace out {

		template<typename T> class binary {
			public:
			T& ref;
			binary(T& obj): ref(obj){}
		};

		namespace col {

			const static u32 white {0xffffff};
			const static u32 green {0x88ff88};
			const static u32 red {0xff8888};
			const static u32 blue {0x8888ff};
			const static u32 critical {0xff0000};

			const static u32 yellow {0xffff00};
			const static u32 orange {0xffa500};
			const static u32 cyan {0x00ffff};

			const static u32 grey {0xaaaaaa};
		}

		static size_t line {0};
		static size_t column {0};

		extern "C" inline void printc(char c);
		extern "C" inline void print(const char* str);
		extern "C" inline void printl(const char* str);

		static u8 space_between_lines {2};

		static size_t use_fb {0};

		namespace actions {
			inline void tab();
			inline void endl();
		}

		static u32 color {0xffffffff};

		typedef struct endl_t {
			//  just to exist
		} endl_t;

		const static endl_t endl;

		typedef struct null_t {
			//  just to exist
			//  does not print anything
		} null_t;

		inline const out::null_t& change(u32 color = out::col::white);

		const static out::null_t null;

		template<typename T> inline T& hex(T& data, bool yes);
		inline const out::null_t& hex(bool yes);

	}


	static struct output {

		bool hex {false};

		//  pointers
		inline struct output& operator<<(const char* str);
		inline struct output& operator<<(const char c);
		template<typename T> inline struct output& operator<<(T* ptr);
		inline struct output& operator<<(const bool& boolean);
		inline struct output& operator<<(const string& str);

		//  other specified types
		inline struct output& operator<<(const out::endl_t&);
		inline struct output& operator<<(const out::null_t&);
		//template<typename T> inline struct output& operator<<(const out::binary_t<T>& bin);
		template<typename T> inline typename type::enable<type::is_int<T>::value, struct output&>::valid operator<<(const out::binary<T>& bin);

		inline struct output& operator<<(const mem::byte_t& bt);
		inline struct output& operator<<(const mem::kb_t& kb);
		inline struct output& operator<<(const mem::mb_t& mb);
		inline struct output& operator<<(const mem::gb_t& gb);

	} output;

	//  if you are reading this trying to find out why does one of these functions throw syntax error
		//  you are putting wrong types inside it :D

	//  numbers -> must be declared as this way
	template<typename T> typename type::enable<type::is_int<T>::value && !type::is_same<T, char>::value, struct output&>::valid inline operator<<(struct output& o, T num);
		//  summary of this function:
			//  template<typename T> create blueprint
			//  typename type::enable<type::is_int<T>::value, struct output*> enables this function only if the T type is integer
				//  also it sets return type to struct output& (return type is specified in second parameter to type::enable<bool, {here}>)

		//  this syntax is hell





#else
	#warning output.h: already included
#endif