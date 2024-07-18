//
//  gdt.cpp
//      part of the H-OS project
//

#ifdef H_OS_LIB_GDT_H
	#ifndef H_OS_LIB_GDT_CPP
		#define H_OS_LIB_GDT_CPP

		gdt::descriptor::descriptor() {
			this->base = 0;
			this->size = 0;
		}

		gdt::entry::entry() {
			this->null();
		}

		gdt::entry::entry(void *base, u32 limit, u8 access, u8 granularity) {
			this->null();
			this->set_base(base);
			this->set_limit(limit);
			this->set_access(access);
			this->set_gbyte(granularity);
		}

		void gdt::entry::set_base(void *base) {
			size_t b = (size_t)base;
			this->base0 = (u16)b;
			this->base1 = (u8)(b >> 16);
			this->base2 = (u8)(b >> 24);
			this->base3 = (u32)(b >> 32);
		}

		void *gdt::entry::base() {
			size_t b = (size_t)this->base3 << 32;
			b |= (size_t)this->base0 & 0xffff;
			b |= (size_t)(this->base1 << 16) & 0xff0000;
			b |= (size_t)(this->base2 << 24) & 0xff000000;
			return (void*)b;
		}

		void gdt::entry::set_limit(u32 limit) {
			u32 lm;
			if (limit > gdt::entry::limit_max) {
				//	set granularity
				this->set_granularity(true);
				lm = limit >> 12;
			} else {
				lm = limit;
			}
			this->lim0 = (u16)(lm & 0xffff);
			this->gran &= 0xf0;
			this->gran |= (u8)(lm >> 16) & 0x0f;
		}

		u64 gdt::entry::limit() {
			u64 limit = (u64)this->lim0 | (u64)((this->gran & 0x0f) << 16);
			if (this->granularity()) {
				limit <<= 12;
			}
			return limit;
		}

		void gdt::entry::set_access(u8 access) {
			this->acc = access;
		}

		u8 gdt::entry::access() {
			return this->acc;
		}

		void gdt::entry::set_granularity(bool enable) {
			if (enable) {
				this->gran |= gdt::granularity::granularity;
			} else {
				this->gran &= ~(gdt::granularity::granularity);
			}
		}

		bool gdt::entry::granularity() {
			return (this->gran & gdt::granularity::granularity) != 0;
		}

		u8 gdt::entry::get_granularity() {
			return this->gran & 0xf0;
		}

		void gdt::entry::set_gbyte(u8 granularity) {
			this->gran |= (granularity & gdt::granularity::granularity) & 0xf0;
		}


		void gdt::init() {

			gdt::table.resize(mem::map::custom.size() + 1);	//	null entry

			//	null entry
			gdt::table[0].null();

			for (u32 i = 1; i < gdt::table.size(); i++) {

				struct mem::map::entry& ce = mem::map::custom[i - 1];

				struct gdt::entry& ent = gdt::table[i];
				ent.null();	//	no bits are set
				ent.set_base(ce.base);
				ent.set_limit(ce.len);
				ent.set_base(ce.base);

				namespace a = gdt::access;
				namespace s = a::seg_sys;
				namespace d = a::seg_data;

				switch (mem::map::custom[i-1].type) {
					case mem::map::types::usable: {
						ent.set_access(gdt::access::map::usable);
						break;
					}
					case mem::map::types::kernel: {
						ent.set_access(gdt::access::map::kernel);
						ent.set_gbyte((u8)(ent.get_granularity() | gdt::granularity::code_64));
						break;
					}
					case mem::map::types::heap: {
						ent.set_access(gdt::access::map::heap);
						break;
					}
					case mem::map::types::reserved: {
						ent.set_access(gdt::access::map::reserved);
						break;
					}
					case mem::map::types::acpi: {
						ent.set_access(gdt::access::map::acpi);
						break;
					}
					case mem::map::types::bad: {
						ent.set_access(gdt::access::map::bad);
						break;
					}
					case mem::map::types::other: {
						ent.set_access(gdt::access::map::other);
						break;
					}
					case mem::map::types::stack: {
						ent.set_access(gdt::access::map::stack);
						break;
					}
					default: {
						output << out::change(out::col::critical) << "ERROR" << out::change() << ":\tunknown memmap entry " << i << " (index: " << mem::map::custom[i].type << ")\n";
						break;
					}
				}
			}
			gdt::pointer.base = (size_t)gdt::table.data();
			gdt::pointer.size = (sizeof(struct gdt::entry) * gdt::table.size()) - 1;
		}


		void gdt::entry::null() {
			this->lim0 = 0;
			this->base0 = 0;
			this->base1 = 0;
			this->acc = 0;
			this->gran = 0;
			this->base2 = 0;
			this->base3 = 0;
			this->reserved = 0;
		}


		void gdt::display() {
			for (u32 i = 1; i < gdt::table.size(); i++) {
				struct gdt::entry& ent = gdt::table[i];
				if (ent.access() & gdt::access::data) {
					output << out::change(out::col::blue) << "data segment: " << out::change() << ent.base() << " to " << (void*)((size_t)ent.base() + (ent.limit() * ((ent.granularity())? mem::byte_t::per_kb * 4 : 1))) << out::endl;
					if (((ent.access() & gdt::access::bits::data::readable) != 0) ^ ((ent.access() & gdt::access::bits::data::writable) != 0)) {
						out::actions::tab();
						if ((ent.access() & gdt::access::bits::data::readable) != 0) {
							out::printc('r');
						}
						if ((ent.access() & gdt::access::bits::data::writable) != 0) {
							out::printc('w');
						}
						out::actions::endl();
					}
				} else {
					output << out::change(out::col::blue) << "system segment: " << out::change() << ent.base() << " to " << (void*)((size_t)ent.base() + (ent.limit() * ((ent.granularity())? mem::byte_t::per_kb : 1))) << out::endl;
					if ((ent.access() & gdt::access::bits::code::executable) != 0) {
						output << "\texecutable" << out::endl;
					}
					if ((ent.access() & gdt::access::bits::code::readable) != 0) {
						output << "\treadable" << out::endl;
					}
					if ((ent.access() & gdt::access::bits::code::conforming) != 0) {
						output << "\tconforming" << out::endl;
					}
				}
				if (ent.granularity()) {
					output << "\tgranular" << out::endl;
				}
				output << "\tring " << ((ent.access() & gdt::access::bits::dpl) >> 5) << out::endl;
				if ((ent.access() & gdt::access::bits::present) != 0) {
					output << "\tpresent" << out::endl;
				} else {
					output << "\tnot present" << out::endl;
				}
			}
		}


		void gdt::entry::debug() {
			u16 b0 = this->base0;
			u8 b1 = this->base1;
			u8 b2 = this->base2;
			u32 b3 = this->base3;

			output << out::change(out::col::blue) << this->base() << " to " << (void*)((size_t)this->base() + this->limit()) << "\t: " << out::binary(b0) << " : " << out::binary(b1) << " : " << out::binary(b2) << " : " << out::binary(b3) << out::change() << out::endl;

			u16 l0 = this->lim0;
			u8 l1 = (this->gran & gdt::granularity::limit);

			output << "\tlimit:\t" << this->limit() << " : " << out::binary(l0) << " : " << out::binary(l1) << out::endl;
			u8 gr = (this->gran & 0xf0);
			output << "\tgranul:\t" << this->granularity() << " : " << out::binary(gr) << " : ";
			namespace g = gdt::granularity;
			if ((this->gran & g::available) != 0) {
				output << " avaliable";
			}
			if ((this->gran & g::code_64) != 0) {
				output << " 64bit code";
			}
			if ((this->gran & g::operating_size) != 0) {
				output << " operating size";
			}
			if ((this->gran & g::granularity) != 0) {
				output << " granular";
			}
			output << out::endl;

			namespace a = gdt::access;
			if ((this->acc & a::data) != 0) {
				output << "data segment:\tring:\t" << ((this->acc & a::bits::dpl) >> 5) << out::endl;
				if ((this->acc & a::seg_data::down) != 0) {
					output << "\tdown" << out::endl;
				} else {
					output << "\tup";
				}
				if ((this->acc & a::seg_data::write) != 0) {
					output << " write";
				} else {
					output << " readonly";
				}
				output << out::endl;
			}

		}


		void gdt::load(gdt::descriptor *ptr) {
			//	load the table
			asm volatile("cli\n"
				"lgdt [%0]\n"
				"sti\n" :: "r"(ptr));
		}

		void gdt::apply(bool stack) {
			gdt::load();

			//	set segment registers
				//	segment registers:
					//	cs -> code segment
					//	ds -> data segment
					//	gs -> kernel specific data (heap)
					//	ss -> stack segment

			//	find each segment
			u16 ds = 0, gs = 0, ss = 0, cs = 0;
			//	code segment
			for (u32 i = 0; i < mem::map::custom.size(); i++) {
				namespace t = mem::map::types;
				switch (mem::map::custom[i].type) {
					//	+ 1 -> first gdt entry is null
					case t::kernel: {
						segment::set(cs, i+1, segment::gdt | segment::privilege[0]);
						break;
					}
					case t::usable: {
						//	data segment (first usable memmap entry)
						if (ds == 0) {
							segment::set(ds, i+1, segment::gdt | segment::privilege[3]);
						}
						break;
					}
					case t::heap: {
						//	gs -> kernel data
						if (gs == 0) {
							segment::set(gs, i+1, segment::gdt | segment::privilege[0]);
						}
						break;
					}
					case t::stack: {
						if (ss == 0) {
							segment::set(ss, i+1, segment::gdt | segment::privilege[0]);
							stack::stack_segment = ss;
						}
						break;
					}
					default: {
						break;
					}
				}
			}

			//	load segments
			asm volatile("mov %0, %%ds" :: "r"(ds));
			asm volatile("mov %0, %%gs" :: "r"(gs));

			//	far jump just dont want to work :(

			//output << "set_cs()" << out::endl;
			//set_cs(cs);
			//output << out::change(out::col::green) << "cs set" << out::change() << out::endl;

		}



	#else
		#warning gdt.cpp already included
	#endif
#else
	#error gdt.cpp: gdt.h not included
#endif