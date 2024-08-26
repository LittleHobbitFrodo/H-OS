//
//	memory/heap.h
//		part of the CORE kernel belonging to the H-OS project
//

#ifndef H_OS_LIB_MEMORY_HEAP_H
	#define H_OS_LIB_MEMORY_HEAP_H

	#ifndef HEAP_INITIAL_BLOCK_SIZE
		#define HEAP_INITIAL_BLOCK_SIZE 32
	#endif

	#define HEAP_MINIMAL_ENTRY_SIZE 512
		//	minimum memmap entry size (in kb)

	typedef struct heap_segment{
		struct heap_segment* next;
		size_t size;
		bool used;
	} heap_segment;

	bool heap_reserve_memory(bool include_reclaimable_entries);

	void heap_init();

	static heap_segment* heap_start = null;
	static void* heap_end_physical = null;

	static heap_segment* heap_used_until = null;
	static heap_segment* heap_end = null;





	void* alloc(size_t bytes);
	void* realloc(__attribute__((nonnull)) void* ptr, size_t bytes);
	void* realloca(__attribute__((nonnull)) void* ptr, size_t bytes, size_t add);	//	reserve more space if reallocation is needed

	__attribute__((always_inline)) inline void free(__attribute__((nonnull)) void* ptr) {
		((heap_segment*)((size_t)ptr - sizeof(heap_segment)))->used = false;
	}

	__attribute__((always_inline)) inline size_t heap_bsize(__attribute__((nonnull)) void* ptr) {
		return ((heap_segment*)((size_t)ptr - sizeof(heap_segment)))->size;
	}

	[[nodiscard]] void* heap_expand(size_t bytes);
	void heap_connect(heap_segment* beg, heap_segment* end);
	heap_segment* heap_divide(heap_segment* seg, size_t size);
	[[nodiscard]] void* heap_enlarge(size_t bytes);

	void heap_debug();

	void* align_alloc(size_t bytes, __attribute__((nonnull)) size_t* align_);
	void* align_alloco(size_t bytes, __attribute__((nonnull)) size_t* align);

	void* align_realloc(__attribute__((nonnull)) void* ptr, __attribute__((nonnull)) size_t* offset, size_t align, size_t bytes);
	void* align_reallocf(__attribute__((nonull)) void* ptr, __attribute__((nonnull)) size_t* offset, size_t align, size_t bytes, void (*on_realloc)(void*));
		//	if data is reallocated -> calls on_realloc and pass pointer to it
	void* align_realloca(__attribute__((nonnull)) void* ptr, __attribute__((nonnull)) size_t* offset, size_t align, size_t bytes, size_t add);
	void* align_reallocaf(__attribute((nonnull)) void* ptr, __attribute__((nonnull)) size_t* offset, size_t align, size_t bytes, size_t add, void (*on_realloc)(void*));
		//	if data is reallocated -> calls on_realloc and pass pointer to it

	void* heap_align_enlarge(size_t bytes, __attribute__((nonnull)) size_t* align_);
	void* heap_align_expand(size_t bytes, size_t* align_);

	void* heap_align_enlargeo(size_t bytes, __attribute__((nonnull)) size_t* align_);
	void* heap_align_expando(size_t bytes, __attribute__((nonnull)) size_t* align_);

	__attribute__((always_inline)) inline void align_free(__attribute__((nonnull)) void* ptr, size_t offset) {
		free((void*)((size_t)ptr - offset));
	}


#else
	#warning memory/heap.h already included
#endif