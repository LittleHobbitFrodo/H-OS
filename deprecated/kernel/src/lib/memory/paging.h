//
//  memory/paging.h
//      part of the H-Os project
//

#ifndef H_OS_LIB_MEMORY_H
	#error memory/paging.h: memory.h not included
#endif

#ifndef H_OS_LIB_MEMORY_PAGING_H
	#define H_OS_LIB_MEMORY_PAGING_H

	namespace mem {

		namespace base {
			static void* virtual_ {nullptr};
			static void* physical {nullptr};
		}

		namespace page {


			static volatile limine::limine_paging_mode_request mode_request = {
					.id = LIMINE_PAGING_MODE_REQUEST,
					.revision = 0
			};

			typedef struct entry {
				inline void* address();
				inline void address(void* ptr);
				inline u64 flags();
				inline void unset(u64 bts);
				inline bool executable();
				inline bool present();
				u64 bits;

				struct flags {
					const static size_t present {1};
					const static size_t write {1 << 1};             //  enables writing
					const static size_t user {1 << 2};              //  enables access from all sources (no only supervisor)
					const static size_t write_through {1 << 3};     //  enable write-through caching
					const static size_t cache_disable {1 << 4};     //  disable caching
					const static size_t accessed {1 << 5};          //  set by the cpu if accessed
					const static size_t dirty {1 << 6};             //  set by cpu if written to
					const static size_t PAT {1 << 7};              //  ignored
					const static size_t global {1 << 8};            //  makes page global (not flushed on context switch)
					const static size_t no_exec {(size_t)1 << 63};

					const static size_t address_shift {12};
				};

			} __attribute__((packed)) entry;

			typedef struct pt {
				mem::page::entry entries[512];
			} __attribute__((packed)) pt;

			/*typedef struct pd {
				mem::page::entry* ptrs[512];
			} __attribute__((packed)) pd;

			typedef struct pdpt {
				mem::page::pd*
			} __attribute__((packed)) pdpt;*/

			extern "C" mem::page::entry* find();
			void init();
			void use();
			template<typename T> void use(aligned_ptr<T>*);

			const static u32 count {512};
			const static u64 page_size {4096};

			static aligned_ptr<mem::page::entry> custom;

		}
	}

#else
	#warning memory/paging.h already included
#endif