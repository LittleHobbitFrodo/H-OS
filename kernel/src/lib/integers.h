//
//	integers.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

//  defining int types -> like <stdint.h>

//  8bit
typedef unsigned char byte;
typedef signed char i8;
typedef unsigned char u8;

//  16bit
typedef signed short int i16;
typedef unsigned short int u16;

//  32bit
typedef signed int i32;
typedef unsigned int u32;

//  64bit
typedef signed long int i64;
typedef unsigned long int u64;

//  128 bit
typedef signed long long int i128;
typedef unsigned long long int u128;


//  max/min
#define MIN_UX 0

//  8bit
#define MIN_I8 (i8)-127
#define MAX_I8 (i8)127
#define MAX_U8 (u8)255

//  16bit
#define MIN_I16 (i16)-32768
#define MAX_I16 (i16)32767
#define MAX_U16 (u16)65535

//  32bit
#define MIN_I32 (i32)-2147483648
#define MAX_I32 (i32)2147483647
#define MAX_U32 (u32)4294967295

//  64bit
#define MIN_I64 (i64)-9223372036854775808
#define MAX_I64 (i64)9223372036854775807L
#define MAX_U64 (u64)18446744073709551615UL

//  128 bit
#define MIN_I128 -170141183460469231731687303715884105729LL
#define MAX_I128 170141183460469231731687303715884105727LL
#define MAX_U128 340282366920938463463374607431768211456ULL

//  ascii
#define MAX_ASCII 127

typedef i16 wordi;
typedef u16 word;

typedef u32 dword;
typedef i32 dwordi;

typedef u64 qword;
typedef i64 qwordi;

#define true 1
#define false 0
#define NULL (void*)0
#define null (void*)0
//#define nullptr (void*)0

typedef u8 bool;

//  size_t#
#ifndef _SIZE_T_DEFINED
	#define _SIZE_T_DEFINED
	#ifdef __i386__
		typedef u32 size_t;
		typedef i32 ssize_t;
		#define MAX_SIZE MAX_U32
		#define MAX_SSIZE MAX_I32
	#elif defined(__x86_64__) || defined(__arm__) || defined(__aarch64__)
		typedef u64 size_t;
		typedef i64 ssize_t;
	#define MAX_SIZE MAX_U64
		#define MAX_SSIZE MAX_I64
	#elif defined(__powerpc__)
		typedef u32 size_t;
		typedef i32 ssize_t;
		#define MAX_SIZE MAX_U32
		#define MAX_SSIZE MAX_I32
	#elif defined(__powerpc64__)
		typedef u64 size_t;
		typedef i64 ssize_t;
		#define MAX_SIZE MAX_U64
		#define MAX_SSIZE MAX_I64
	#else
		#error could not determine size_t, please define it before (_SIZE_T_DEFINED)
	#endif
#endif
