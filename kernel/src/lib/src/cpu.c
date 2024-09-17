//
//	cpu.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifdef H_OS_LIB_CPU_H
	#ifndef H_OS_LUB_CPU_C
		#define H_OS_LIB_CPU_C

		void cpu_info() {
			printl("CPU info:");
			print("\tvendor:\t"); printl(cpu.vendor->name);
			print("\tmodel:\t"); printl((const char*)&cpu.model);
			if (cpu.vendor->hypervised) {
				printl("\thypervised");
			}
		}

		enum cpu_init_codes cpu_init() {
			cpu.vendor = null;

			//	detect vendor
			u32 data[3];
			asm volatile("cpuid" : "=b"(data[0]), "=d"(data[1]), "=c"(data[2]) : "a"(0));
			char str[13];
			const char* s = (const char*)&str;
			memcpy((void*)&data, &str, 12);
			str[12] = '\0';

			for (size_t i = 0; i < cpu_vendor_count; i++) {
				if (strcmpb(cpu_vendors[i].id, s)) {
					cpu.vendor = (cpu_vendor_t*)&(cpu_vendors[i]);
					break;
				}
			}

			if (cpu.vendor == null) {
				output.color = col.critical;
				print("ERROR");
				output.color = col.white;
				printl(": could not find cpu vendor");
				return cic_vendor_not_found;
			}

			//	detect cpu model name

			u32 txt[4];

			//	first 16 characters
			asm volatile("cpuid" : "=a"(txt[0]), "=b"(txt[1]), "=c"(txt[2]), "=d"(txt[3]) : "a"(0x80000002));
			memcpy((void*)&txt, (void*)&cpu.model, 4 * sizeof(u32));

			asm volatile("cpuid" : "=a"(txt[0]), "=b"(txt[1]), "=c"(txt[2]), "=d"(txt[3]) : "a"(0x80000003));
			memcpy((void*)&txt, (void*)((size_t)&cpu.model + sizeof(u32) * 4), 4*sizeof(u32));

			cpu.model[sizeof(u32) * 8] = '\0';

			asm volatile("cpuid" : "=a"(txt[0]), "=b"(txt[1]), "=c"(txt[2]), "=d"(txt[3]) : "a"(0x80000004));
			memcpy((void*)&txt, (void*)((size_t)&cpu.model + sizeof(u32) * 8), 4*sizeof(u32));
			cpu.model[48] = '\0';

			return cic_ok_;
		}

	#else
		#warning cpu.c already included
	#endif
#endif
//	#error cpu.c: cpu.h not included
//#endif