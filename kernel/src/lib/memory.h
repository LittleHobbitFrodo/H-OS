//
//  memory.h
//      part of the H-OS project
//

#ifndef H_OS_LIB_MEMORY_H
	#define H_OS_LIB_MEMORY_H

	#ifndef INTERRUPT_HANDLER_STACK_SIZE_KB
		#define INTERRUPT_HANDLER_STACK_SIZE_KB 8
	#else
		#if INTERRUPT_HANDLER_STACK_SIZE_KB < 8
			#warning macro interrupt_handler_stack_size must be bigger than 8 (defaulting to 8)
			#define INTERRUPT_HANDLER_STACK_SIZE_KB 4
		#endif
	#endif

	namespace mem {

		inline void* align(void* ptr, size_t align);

		constexpr size_t set_bits(u16 count, bool beggining = true);
		void set(void* ptr, size_t size, u8 value);

		const static size_t kilobyte {1024};
		const static size_t megabyte {1024 * 1024};
		const static size_t gigabyte {1024 * 1024 * 1024};

		typedef struct byte_t;
		typedef struct kb_t;
		typedef struct mb_t;
		typedef struct gb_t;

		template<typename T> struct is_mem_conv_t {
			static const bool value = false;
		};
		template<> struct is_mem_conv_t<size_t> {
			static const bool value = true;
		};
		template<> struct is_mem_conv_t<mem::byte_t> {
			static const bool value = false;
		};
		template<> struct is_mem_conv_t<mem::kb_t> {
			static const bool value = false;
		};
		template<> struct is_mem_conv_t<mem::mb_t> {
			static const bool value = false;
		};
		template<> struct is_mem_conv_t<mem::gb_t> {
			static const bool value = false;
		};

		//  types with automatic conversion

		//  bytes are size_t type

		[[maybe_unused]] typedef struct byte_t {
			size_t count;
			const static size_t per_gb {1024 * 1024 * 1024};
			const static size_t per_mb {1024 * 1024};
			const static size_t per_kb {1024};
			template<typename T> typename type::enable<mem::is_mem_conv_t<T>::value, char>::valid inline calculate(T cnt);
			byte_t();
			byte_t(size_t bytes);
		} byte_t;

		[[maybe_unused]] typedef struct kb_t {
			size_t count;
			const static size_t per_gb {1024 * 1024};
			const static size_t per_mb {1024};
			template<typename T, bool return_bytes = false> typename type::enable<mem::is_mem_conv_t<T>::value && return_bytes, size_t>::valid inline calculate(T cnt);
			kb_t();
			kb_t(size_t kb);
		} kb_t;

		[[maybe_unused]] typedef struct mb_t {
			size_t count;
			const static size_t per_gb {1024};
			template<typename T, bool return_kbs = false> typename type::enable<mem::is_mem_conv_t<T>::value && return_kbs, mem::kb_t>::valid inline calculate(T cnt);
			mb_t();
			mb_t(size_t mb);
		} mb_t;

		[[maybe_unused]] typedef struct gb_t {
			size_t count;
			template<typename T, bool return_mbs = false> typename type::enable<mem::is_mem_conv_t<T>::value && return_mbs, mem::mb_t>::valid inline calculate(T cnt);
			gb_t();
			gb_t(size_t gb);
		} gb_t;

		[[maybe_unused]] static volatile limine::limine_framebuffer_request framebuffer = {
				.id = LIMINE_FRAMEBUFFER_REQUEST,
				.revision = 0
		};

		namespace info {
			static size_t total {0};
			[[maybe_unused]] static size_t usable {0};
			[[maybe_unused]] static size_t used {0};
		}



		namespace map {

			void parse();

			typedef struct entry {
				void* base;
				u64 len;
				u8 type;

				entry();

			} entry;

			//	vector declared in vector.h
				//	original memmap could be overwritten
				//	also makes memmap more simple

			static volatile limine::limine_memmap_request request = {
					.id = LIMINE_MEMMAP_REQUEST,
					.revision = 0
			};

			namespace types {
				const static u8 usable {0};
				const static u8 kernel {1};
				const static u8 heap {2};
				const static u8 reserved {3};
				const static u8 reclaimable {4};	//	acpi, bootloader
				const static u8 acpi {5};
				const static u8 bad {6};
				const static u8 stack {7};
				const static u8 other {8};	//	fb, ...

				const static u8 undefined {MAX_U8};
			}

			u8 type(u64 constant);

			void reclaim();
				//  reclaims bootloader reclaimable memmap entries
			void display();
		}




		size_t init();

		const static u32 interrupt_handler_stack_size {INTERRUPT_HANDLER_STACK_SIZE_KB * mem::byte_t::per_kb};

	}



	template<typename T> class safe_ptr {
		T* p;

		public:

		safe_ptr();

		safe_ptr(size_t bytes);

		inline void alloc(size_t bytes);

		inline void realloc(size_t bytes, size_t add = 0);

		inline void free();

		inline T* ptr();

		~safe_ptr();


	};

	template<typename T> class aligned_ptr {
		u8 align_;
		T* ptr_;
		u8 offset_;

		public:

		aligned_ptr();
		explicit aligned_ptr(u8 align);
		aligned_ptr(size_t bytes, u8 a);

		inline bool set_align(u8 align);
			//	ptr must be null
		[[nodiscard]] inline u8 align() const;
		inline T* ptr() const;
		inline u8 offset() const;

		~aligned_ptr();

		inline void alloc(size_t bytes);
		inline void realloc(size_t bytes, size_t add = 0);
		inline void free();

	};


#else
	#warning memory.h already included
#endif