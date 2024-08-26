//
//  vector.h
//      part of the H-Os project
//


////	OOOHH YEAAAAH

#ifndef H_OS_LIB_VECTOR_H
	#define H_OS_LIB_VECTOR_H

	template<typename T> class vector {

		//	this is simpl implementaion of vector class
		//	this implementation does not work the same way as std::vector does

		T* d;
		size_t l;

		public:
			const static u32 alloc_add {4};
			const static u32 realloc_add {4};

			vector();
			vector(size_t size);
			explicit vector(vector<T>& vec);

			~vector();

			template<bool realloc = false> inline void resize(size_t size);

			[[nodiscard]] inline T& at(size_t i) const;
			[[nodiscard]] inline T& operator[](size_t i) const;

			[[nodiscard]] T& push();
			void pop(size_t size = 1);
			void operator+=(T& add);
			void operator--();

			inline void clear();

			inline size_t size() const;
			inline T* data() const;
			inline bool empty() const;

			void realloc();

	};


template<typename T> class aligned_vector {

	//	this is simpl implementaion of vector class
	//	this implementation does not work the same way as std::vector does

	aligned_ptr<T> d;
	size_t l;

	public:
	const static u32 alloc_add {4};
	const static u32 realloc_add {4};

	aligned_vector();
	aligned_vector(size_t align);
	aligned_vector(size_t size, size_t align);
	explicit aligned_vector(aligned_vector<T>& vec);

	~aligned_vector();

	template<bool realloc = false> inline void resize(size_t size);

	[[nodiscard]] inline T& at(size_t i) const;
	[[nodiscard]] inline T& operator[](size_t i) const;

	[[nodiscard]] T& push();
	void pop(size_t size = 1);
	void operator+=(T& add);
	void operator--();

	inline void clear();

	inline size_t size() const;
	inline T* data() const;
	inline bool empty() const;

	inline size_t offset() const;
	inline size_t align() const;

	void realloc();

};


typedef struct static_index_not_found {};

template<typename T> constexpr size_t static_index(const T* arr, T value, size_t size);

//	mem::map::custom
namespace mem {
	namespace map {
		static vector<mem::map::entry> custom;
	}
}

#else
	#warning vector.h already included
#endif