//
//  memory/heap.h
//      part of the H-OS project
//

#ifndef H_OS_LIB_MEMORY_H
	#error memory/heap.h: memory.h not included
#endif

#ifndef H_OS_LIB_MEMORY_HEAP_H
	#define H_OS_LIB_MEMORY_HEAP_H

	namespace mem {
		namespace heap {

			bool reserve_memory();
				//  writes data into mem::heap::start, mem::heap::end_physical
				//  required for heap initialization (mem::heap::init())

			void debug();

			const static size_t initial_block_size {7};
			const static size_t alloc_align_max {127};
			const static size_t divide_min_bytes {16};

			static u32 minimum_memmap_entry_size_in_kb {512};
			//  may increase in further versions

			void init();

			typedef struct segment {
				struct mem::heap::segment* next;
				u32 size;
				bool used;

				segment();
				segment(mem::heap::segment& seg);

			} segment;

			inline size_t size();

			static struct mem::heap::segment* start {nullptr};
			static struct mem::heap::segment* end {nullptr};
			static struct mem::heap::segment* used_until {nullptr};
			static void* end_physical {nullptr};

			inline void* alloc(size_t bytes);
			inline void* alloc(size_t bytes, u8& align);
			inline void free(void* ptr);
			//  must be aligned at the beginning og the chunk

			inline void* realloc(void* ptr, size_t bytes);
			inline void* realloc(void* ptr, size_t bytes, size_t add);
			inline void* realloc(void* ptr, size_t bytes, size_t add, u8& offset, u8 original_align);
			//  checks if block is valid length
				//  true -> return the same block
				//  false -> allocates new bytes + add bytes

			inline u32 block_size(void* ptr);

			inline struct mem::heap::segment* divide_block(struct mem::heap::segment* seg, size_t size);

			inline void* expand(size_t bytes);
				//  expands the heap and allocates bytes

		}
	}

#else
	#warning memory/heap.h already included
#endif