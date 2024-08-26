//
//  cpu.cpp
//    part of the H-OS project
//

#ifdef H_OS_LIB_CPU_H
  #ifndef H_OS_LIB_CPU_CPP
    #define H_OS_LIB_CPU_CPP

    hw::cpu::vendor_t::vendor_t() {
      this->name = nullptr;
      this->id = nullptr;
    }

    /*constexpr hw::cpu::vendor_t::vendor_t(const char *n, const char *i) {
      this->name = (char*)n;
      this->id = (char*)i;
    }*/





    void hw::cpu::vids::get() {

      u32 data[3];
      asm volatile("cpuid":  "=b"(data[0]), "=d"(data[1]), "=c"(data[2]) : "a"(0));
      char str[13];
      const char* s = (const char*)&str;

      for (u32 i = 0; i < 12; i++) {
        str[i] = ((char*)&data)[i];
      }
      str[12] = '\0';

      output << "vendor string: " << s << out::endl;
      for (u32 i = 0; i < 13; i++) {
        output << (int)s[i] << " ";
      }

      output << "vendor[0]: " << vendors[0].name << " : " << vendors[0].id << out::endl;

      namespace c = hw::cpu;

      for (u32 i = 0; i < c::vendor_count; i++) {
        output << "i: " << i << out::endl;
        for (u32 ii = 0; ii < MAX_I32 / 3; ii++);
        const hw::cpu::vendor_t& vendor = c::vendors[i];
        output << "vendor.id: " << (void*)vendor.id << out::endl;
        for (u32 ii = 0; ii < MAX_I32; ii++);
        if (utilstr::cmp(vendor.id, s)) {
          output << "found: " << i << vendor.id << out::endl;
          for (u32 ii = 0; ii < MAX_I32; ii++);
          c::vendor = i;
          c::vid = (char*)vendor.id;
          c::hypervised = (i >= 18);
          break;
        }
      }
      output << "ret" << out::endl;
      for (u32 i = 0; i < MAX_I32; i++);
    }

    void hw::cpu::init() {

    }


  #else
    #warning cpu.cpp already included
  #endif
#else
  #error cpu.cpp: cpu.h not included
#endif