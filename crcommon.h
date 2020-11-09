#pragma once

#ifndef _CR_COMMON_H_
#define _CR_COMMON_H_

#include <stdlib.h>
#include <cassert>

typedef unsigned char   cr_u8;
typedef unsigned short cr_u16;
typedef unsigned int   cr_u32;
typedef unsigned int   cr_u32;
typedef unsigned long  cr_u64;

typedef signed char     cr_s8;
typedef signed short   cr_s16;
typedef signed int     cr_s32;
typedef signed long    cr_s64;

typedef float  cr_r32;
typedef double cr_r64;

typedef char           cr_byte8;
typedef signed char   cr_sbyte8;
typedef unsigned char cr_ubyte8;

typedef bool cr_b8;

// Overloadable allocators

typedef void* (CR_ALLOC)(cr_u32 size);
typedef void* (CR_REALLOC)(void* ptr, cr_u32 size);
typedef void  (CR_FREE)(void* ptr);

extern CR_ALLOC*   cr_alloc;
extern CR_REALLOC* cr_realloc;
extern CR_FREE*    cr_free;

static void cr_zero_mem(void* data, cr_u32 size);

static void cr_zero_mem(void* data, cr_u32 size) {
	// TODO: Uncomment me!!!!!
    // assert(data);
	
    cr_byte8* w = (cr_byte8*) data;
	while(w - data < size) {
		*w = '\0'; w++;
	}
}

#endif
