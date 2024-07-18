//
//  vector.h
//      part of the H-Os project
//


////	OOOHH YEAAAAH

#ifndef H_OS_LIB_VECTOR_H
	#define H_OS_LIB_VECTOR_H

	template<typename T> class vector {

		T* dt;
		u32 len;

		public:

		const static u32 alloc_add {4};
		const static u32 realloc_add {4};

		vector();
		explicit vector(T& first);

		inline void shrink_to(u32 nsize);
		inline void shrink_until(T& value);
		inline void shrink_untiln(T value);
		inline void resize(u32 nsize, bool realloc = true);


		inline void realloc();
			//  function to optimize memory usage after popping
			//  this function actually considers if it is necessary to do the reallocation

		inline T& at(u32 i);
		inline T& operator[](u32 i);

		inline void push(T& instance);
		inline void pushn(T instance);
			//  same function (with no reference)

		inline void pop(u32 count = 1);
		inline void operator--();

		inline void clear();

		inline u32 size() const;
		inline const T* data() const;


		inline void operator+=(T& instance);

		~vector();

	};


	template<typename T> class aligned_vector {
		aligned_ptr<T> dt;
		u32 len;

	public:

		const static u32 alloc_add {4};
		const static u32 realloc_add {4};

		explicit aligned_vector(u8 align);
		explicit aligned_vector(T& first, u8 align);

		inline void shrink_to(u32 nsize);
		inline void shrink_until(T& value);
		inline void shrink_untiln(T value);
		inline void resize(u32 nsize);

		inline void set_align(u8 align);
		[[nodiscard]] inline u8 align() const;


		inline void realloc();
		//  function to optimize memory usage after popping
		//  this function actually considers if it is necessary to do the reallocation

		inline T& at(u32 i);
		inline T& operator[](u32 i);

		inline void push(T& instance);
		inline void pushn(T instance);
		//  same function (with no reference)

		inline void pop(u32 count = 1);
		inline void operator--();

		inline void clear();

		inline u32 size() const;
		inline const T* data() const;


		inline void operator+=(T& instance);

		~aligned_vector();

	};

//	mem::map::custom
namespace mem {
	namespace map {
		static vector<mem::map::entry> custom;
	}
}

#else
	#warning vector.h already included
#endif