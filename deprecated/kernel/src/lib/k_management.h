//
//  init.h
//      part of the H-OS project
//

#ifndef H_OS_LIB_K_MANAGEMENT_H
#define H_OS_LIB_K_MANAGEMENT_H

	extern "C" void kernel();

	extern "C" void init();

	extern "C" void panic(size_t errcode);


	namespace manage {

		static volatile limine::limine_kernel_file_request args = {
				.id = LIMINE_KERNEL_FILE_REQUEST,
				.revision = 0
		};

		u32 parse_cmd();

		namespace states {
			namespace init {
				const static u8 memory {0};
				const static u8 interrupts {1};
			}
		}
		static u8 state {manage::states::init::memory};

		namespace vocal {
			namespace levels {
				const static u8 stfu {0};
					//  reports only critical errors
				const static u8 quiet_please {1};
					//  reports warnings
				const static u8 normal {2};
					//  reports some achievements
				const static u8 vocal {3};
					//  reports most of the stuff happening insize
				const static u8 report_everything {4};
					//  reports basically everything
			}
			static u8 level {manage::vocal::levels::normal};

		}
		namespace codes {
			namespace mem {
				const static size_t ok{0};
				const static size_t memmap_not_found{1};
				const static size_t unsupported_paging_mode{2};
				const static size_t cannot_allocate_memory_for_kernel_heap{3};
				const static size_t cannot_locate_kernel_entry {4};
				const static size_t cannot_allocate_memory_for_stacks {5};
				const static size_t unable_to_allocate_paging_table {6};
				const static size_t base_addresses_not_available {7};
			}


			namespace parse {
				const static u32 ok{0};
				const static u32 cmdargs_null{1};
			}
		}
	}



#else
	#warning k_management.h already included
#endif

