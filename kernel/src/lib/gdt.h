//
//  gdt.h
//      part of the H-OS project
//

#ifndef H_OS_LIB_GDT_H
	#define H_OS_LIB_GDT_H


	/*typedef struct far_ptr {

	} __attribute__((packed));*/

	extern "C" void set_cs(u16);

	namespace gdt {

		typedef struct entry {

			private:

			//  structure:
			//  bits        purpose
			//  0 - 15      limit
			//  16 - 39     base
			//  40 - 47     access byte
			//  48 - 51     limit
			//  52 - 55     flags
			//  56 - 95     base
			//  96 - 127    reserved

			//private:

			u16 lim0;
			u16 base0;
			u8 base1;
			u8 acc;
			u8 gran;
			u8 base2;
			u32 base3;
			u32 reserved;


			public:

			void null();

			const static u32 limit_max {0xfffff};

			inline void* base();
			inline u64 limit();
			inline u8 access();
			inline bool granularity();
			inline u8 get_granularity();

			inline void set_base(void* base);
			inline void set_limit(u32 limit);
			inline void set_access(u8 access);
			inline void set_granularity(bool enable);
			inline void set_gbyte(u8 granularity);

			entry();
			entry(void* base, u32 limit, u8 access, u8 granularity);

			void debug();

		} __attribute__((packed)) entry;

		namespace granularity {

			const static u8 limit {0x0f};

			const static u8 available {1 << 4};
				//	unused by system, Os can use
			const static u8 code_64 {1 << 5};
				//	64bit segment -> code
			const static u8 operating_size {1 << 6};
				//	ignored
			const static u8 granularity {1 << 7};
				//	if set size is not in bytes but 4kb
		}

		namespace access {

			namespace bits {

				const static u8 accessed {1};

				namespace code {
					const static u8 executable {1 << 3};
					const static u8 conforming {1 << 2};
						//	can be executed from lower or equal privilege level
					const static u8 readable {1 << 1};
				}

				namespace data {
					const static u8 writable {1 << 3};
					const static u8 expansion {1 << 2};
					const static u8 readable {1 << 1};
				}

				const static u8 segment_type {1 << 4};
				const static u8 dpl {1 << 6 | 1 << 5};
					//	privilage level (lower is higher)
				const static u8 present {1 << 7};
			}

			//  4th bit
			const static u8 system {0};
			const static u8 data {1 << 4};

			const static u8 privilege[4] {0, 0b00100000,
										  0b01000000, 0b01100000};
				//  smaller index is more privileged
			const static u8 executable {1 << 3};



			namespace seg_data {
				//  2nd bit
				const static u8 up {0};
				const static u8 down {0b10};

				//  1st bit
				const static u8 write {1};
				const static u8 readonly {0};
			}
			namespace seg_sys {
				//  2nd bit
				const static u8 non_conforming {0};
				const static u8 conforming {0b10};

				//  1st bit
				const static u8 read {1};
				const static u8 not_readable {0};

			}


			namespace map {
				//	defaults for each memmap entry type
				namespace a = gdt::access;
				const static u8 usable {(u8)(a::data | a::privilege[3] | a::seg_data::write | a::bits::present | a::bits::data::readable | a::bits::data::writable)};
				const static u8 kernel {(u8)(a::system | a::executable | a::seg_sys::not_readable | a::privilege[0] | a::bits::present)};
				const static u8 reserved {(u8)(a::system | a::privilege[0] | a::executable | a::seg_sys::read | a::bits::present)};
				const static u8 heap {(u8)(a::data | a::privilege[0] | a::seg_data::write | a::bits::present)};
				const static u8 acpi {(u8)(a::data | a::privilege[0] | a::seg_data::write | a::bits::present)};
				const static u8 bad {(u8)(a::system | a::privilege[0] | a::seg_sys::not_readable | a::bits::present)};
				const static u8 stack {(u8)(a::data | a::privilege[0] | a::bits::present)};
				const static u8 other {(u8)(a::data | a::privilege[0] | a::seg_data::write | a::bits::present)};

			}

		}

		typedef struct descriptor {
			u16 size;
			u64 base;
			descriptor();
		} __attribute__((packed)) descriptor;

		static struct gdt::descriptor pointer = gdt::descriptor();

		//const static u16 size {5};

		//static struct gdt::entry table[gdt::size];  //  potentionally expand later
		static aligned_vector<gdt::entry> table = aligned_vector<gdt::entry>(8);


		void init();
		inline void load(gdt::descriptor* ptr = &gdt::pointer);
		void apply(bool stack = false);	//	calls gdt::load function
		void display();

		/*typedef struct table {
			//  some stuff in here
		} __attribute__((packed)) table;*/

	}

#else
	#warning gdt.h already included
#endif