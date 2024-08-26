//
//    cpu.h
//        part of the H-OS project
//

#ifndef H_OS_LIB_CPU_H
  #define H_OS_LIB_CPU_H

  namespace hw {
    namespace cpu {

      void init();

      static char* name = {nullptr};

      static char* vid {nullptr};
      static u8 vendor {0};
      static bool hypervised = false;

      typedef struct vendor_t {
        char* name;
        char* id;

        vendor_t();
        vendor_t(const char *n, const char *i) : name((char*)n), id((char*)i) {}
      } vendor_t;

      const static vendor_t vendors[] = {vendor_t("AMD", "AuthenticAMD"), vendor_t("intel", "GenuineIntel"),
        vendor_t("VIA", "VIA VIA VIA "), vendor_t("Transmeta", "GenuineTMx86"),
        vendor_t("Cyrix", "CyrixInstead"), vendor_t("Centaur", "CentaurHauls"),
        vendor_t("NexGen", "NexGenDriven"), vendor_t("UMC", "UMC UMC UMC "),
        vendor_t("SiS", "SiS SiS SiS "), vendor_t("NSC", "Geode by NSC"),
        vendor_t("Vortex86", "Vortex86 SoC"), vendor_t("AO486", "MiSTer AO486"),
        vendor_t("Zhaoxin", "  Shanghai  "), vendor_t("Hygon", "HygonGenuine"),
        vendor_t("Elbrus", "E2K MACHINE "), vendor_t("QEMU", "TCGTCGTCGTCG"),
        vendor_t("KVM", " KVMKVMKVM  "), vendor_t("VMware", "VMwareVMware"),
        vendor_t("VirtualBox", "VBoxVBoxVBox"), vendor_t("Xen", "XenVMMXenVMM"),
        vendor_t("Hyper-V", "Microsoft Hv"), vendor_t("Parallels", " prl hyperv "),
        vendor_t("Bhyve", "bhyve bhyve "), vendor_t("QNX", " QNXQVMBSQG ")};
      const static size_t vendor_count = 24;

      namespace vids {  //  vendor ids
        void get();

        const static char* amd_old  {"AMDisbetter!"};
        const static char* transmeta_old   {"TransmetaCPU"};
        const static char* ao486_old  {"GenuineAO486"};
        const static char* para_alt {" lrpepyh vr "};
      }

      namespace model {
        const static u32 eax {1};
        void get();
      }

    }
  }

#else
  #warning cpu.h already included
#endif