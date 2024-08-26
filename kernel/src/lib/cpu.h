//
//	cpu.h
//		part of the CORE kernel belonging to the H-OS project
//

#ifndef H_OS_LIB_CPU_H
	#define H_OS_LIB_CPU_H

	typedef struct cpu_vendor_t {

		char* name;
		char* id;
		bool hypervised;

	} cpu_vendor_t;

	static struct cpu {

		cpu_vendor_t* vendor;

		char model[49];	//	48 + nullbyte

	} cpu;

	enum cpu_init_codes {
		ok_,
		vendor_not_found
	};

	enum cpu_init_codes cpu_init();
	void cpu_info();

	const static cpu_vendor_t cpu_vendors[] = {{.name = "AMD", .id = "AuthenticAMD", .hypervised = false},
		{.name = "AMD",		.id = "AMDisbetter!", .hypervised = false},
	{.name = "Intel",			.id = "GenuineIntel", .hypervised = false},
	{.name = "VIA",			.id = "VIA VIA VIA ", .hypervised = false},
	{.name = "Transmeta",		.id = "GenuineTMx86", .hypervised = false},
		{.name = "Transmeta",	.id = "TransmetaCPU", .hypervised = false},
	{.name = "Cyrix",			.id = "CyrixInstead", .hypervised = false},
	{.name = "Centaur",		.id = "CentaurHauls", .hypervised = false},
	{.name = "NexGen",		.id = "NexGenDriven", .hypervised = false},
	{.name = "UMC",			.id = "UMC UMC UMC ", .hypervised = false},
	{.name = "SiS",			.id = "SiS SiS SiS ", .hypervised = false},
	{.name = "NSC",			.id = "Geode by NSC", .hypervised = false},
	{.name = "Vortex86",		.id = "Vortex86 SoC", .hypervised = false},
	{.name = "AO486",			.id = "MiSTer AO486", .hypervised = false},
		{.name = "AO486",		.id = "GenuineAO486", .hypervised = false},
	{.name = "Zhaoxin",		.id = "  Shanghai  ", .hypervised = false},
	{.name = "Hygon",			.id = "HygonGenuine", .hypervised = false},
	{.name = "Elbrus",		.id = "E2K MACHINE ", .hypervised = false},
	{.name = "qemu",			.id = "TCGTCGTCGTCG", .hypervised = true},
	{.name = "KVM",			.id = " KVMKVMKVM  ", .hypervised = true},
	{.name = "VMware",		.id = "VMwareVMware", .hypervised = true},
	{.name = "VirtualBox",	.id = "VBoxVBoxVBox", .hypervised = true},
	{.name = "Xen",			.id = "XenVMMXenVMM", .hypervised = true},
	{.name = "Hyper-v",		.id = "Microsoft Hv", .hypervised = true},
	{.name = "Parallels",		.id = " prl hyperv", .hypervised = true},
	{.name = "Parallels",		.id = " lrpepyh vr ", .hypervised = true},
	{.name = "Bhyve",			.id = "bhyve bhyve ", .hypervised = true},
	{.name = "QNX",			.id = " QNXQVMBSQG ", .hypervised = true}};

	const static size_t cpu_vendor_count = 28;

#else
	#warning cpu.h already included
#endif