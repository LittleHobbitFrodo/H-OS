//
//	segmentation.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifndef H_OS_LIB_SEGMENTATION_H
	#define H_OS_LIB_SEGMENTATION_H

	typedef struct segment_t {
		u16 privilege:	2;		//	ring?
		u16 TI:			1;		//	0:		gdt is used
								//	1:		ldt is used
		u16 index:	13;
	} segment_t;

#endif