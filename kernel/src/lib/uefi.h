//
//	uefi.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "./integers.h"


#include "../gnu-efi/efi.h"
#include "../gnu-efi/efilib.h"

//	header for uefi runtime services

static void uefi_init();

typedef struct uefi_t {
	bool supported;
	EFI_SYSTEM_TABLE* system_table;
	EFI_RUNTIME_SERVICES* runtime;
} uefi_t;

static uefi_t uefi;

enum uefi_reset_mode {
	uefi__shutdown = (u8)((EFI_RESET_TYPE)EfiResetShutdown),
	uefi__restart = (u8)((EFI_RESET_TYPE)EfiResetCold),
	uefi__reboot = (u8)((EFI_RESET_TYPE)EfiResetWarm)
} uefi_reset_mode;

__attribute__((noreturn)) static void uefi_reset(enum uefi_reset_mode mode);

