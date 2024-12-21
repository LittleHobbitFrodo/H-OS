//
//	cpu.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "./integers.h"

enum environments {
	environment_hardware = 0,
	environment_emulator = 1,
	environment_vm = 2
} environments;

enum hypervisors {
	hypervisor_none = 0,
	hypervisor_qemu = 1,
	hypervisor_bochs = 2,
	hypervisor_unsupported = 3,
} hypervisors;

typedef struct cpu_vendor_t {
	char *name;
	char *id;
	enum environments environment;
} cpu_vendor_t;

typedef struct cpu_t {
	cpu_vendor_t *vendor;
	char model[49]; //	48 + nullbyte
	enum hypervisors hypervisor;
	size_t count;
} cpu_t;

static cpu_t cpu = {0};

static void cpu_init();

void cpu_info();

static cpu_vendor_t cpu_vendors[] = {
	{.name = "AMD", .id = "AuthenticAMD", .environment = environment_hardware},
	{.name = "AMD", .id = "AMDisbetter!", .environment = environment_hardware},
	{.name = "Intel", .id = "GenuineIntel", .environment = environment_hardware},
	{.name = "VIA", .id = "VIA VIA VIA ", .environment = environment_hardware},
	{.name = "Transmeta", .id = "GenuineTMx86", .environment = environment_hardware},
	{.name = "Transmeta", .id = "TransmetaCPU", .environment = environment_hardware},
	{.name = "Cyrix", .id = "CyrixInstead", .environment = environment_hardware},
	{.name = "Centaur", .id = "CentaurHauls", .environment = environment_hardware},
	{.name = "NexGen", .id = "NexGenDriven", .environment = environment_hardware},
	{.name = "UMC", .id = "UMC UMC UMC ", .environment = environment_hardware},
	{.name = "SiS", .id = "SiS SiS SiS ", .environment = environment_hardware},
	{.name = "NSC", .id = "Geode by NSC", .environment = environment_hardware},
	{.name = "Vortex86", .id = "Vortex86 SoC", .environment = environment_hardware},
	{.name = "AO486", .id = "MiSTer AO486", .environment = environment_hardware},
	{.name = "AO486", .id = "GenuineAO486", .environment = environment_hardware},
	{.name = "Zhaoxin", .id = "  Shanghai  ", .environment = environment_hardware},
	{.name = "Hygon", .id = "HygonGenuine", .environment = environment_hardware},
	{.name = "Elbrus", .id = "E2K MACHINE ", .environment = environment_hardware},
	{.name = "qemu", .id = "TCGTCGTCGTCG", .environment = environment_emulator},
	{.name = "KVM", .id = " KVMKVMKVM  ", .environment = environment_vm},
	{.name = "VMware", .id = "VMwareVMware", .environment = environment_vm},
	{.name = "VirtualBox", .id = "VBoxVBoxVBox", .environment = environment_vm},
	{.name = "Xen", .id = "XenVMMXenVMM", .environment = environment_vm},
	{.name = "Hyper-v", .id = "Microsoft Hv", .environment = environment_vm},
	{.name = "Parallels", .id = " prl hyperv", .environment = environment_vm},
	{.name = "Parallels", .id = " lrpepyh vr ", .environment = environment_vm},
	{.name = "Bhyve", .id = "bhyve bhyve ", .environment = environment_vm},
	{.name = "QNX", .id = " QNXQVMBSQG ", .environment = environment_vm}
};

static const size_t cpu_vendor_count = 28;
