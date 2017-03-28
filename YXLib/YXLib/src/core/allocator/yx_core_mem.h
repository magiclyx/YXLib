//
//  yx_core_mem.h
//  YXLib
//
//  Created by Yuxi Liu on 11/5/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_MEM_H_
#define _YX_CORE_MEM_H_

#include "../yx_core_global.h"
#include "../rttidef/yx_core_rttidef.h"


#pragma mark - aligment

/**
 *  YX_STRUCT_ALIGNMENT_INFILLING_STUFF_SIZE - calculate the size of the fillers
 *  YX_STRUCT_ALIGNMENT_INFILLING            - fill the struct with a yx_byte array
 */
#define YX_STRUCT_ALIGNMENT_INFILLING_STUFF_SIZE(structSize) (((structSize) + (YX_ALIGNMENT - 1)) / YX_ALIGNMENT)*YX_ALIGNMENT - (structSize)
#define YX_STRUCT_ALIGNMENT_INFILLING(structSize) yx_byte __struct_aligment_infilling__[YX_STRUCT_ALIGNMENT_INFILLING_STUFF_SIZE(structSize)]

/*a macro to alignment the pointer*/
#define YX_POINT_ALIGNMENT(pt) \
(u_char *) (((uintptr_t) (pt) + ((uintptr_t) YX_ALIGNMENT - 1)) & ~((uintptr_t) YX_ALIGNMENT - 1))

#define YX_UINT32_ALIGNMENT(val) \
(yx_uint32) (((yx_uint32) (val) + ((yx_uint32) YX_ALIGNMENT - 1)) & ~((yx_uint32) YX_ALIGNMENT - 1))

#define YX_UINT64_ALIGNMENT(val) \
(yx_uint64) (((yx_uint64) (val) + ((yx_uint64) YX_ALIGNMENT - 1)) & ~((yx_uint64) YX_ALIGNMENT - 1))

#define YX_UINT_ALIGNMENG(val) \
(yx_uint) (((yx_uint) (val) + ((yx_uint) YX_ALIGNMENT - 1)) & ~((yx_uint) YX_ALIGNMENT - 1))

#define YX_ULONG_ALIGNMENG(val) \
(yx_ulong) (((yx_ulong) (val) + ((yx_ulong) YX_ALIGNMENT - 1)) & ~((yx_ulong) YX_ALIGNMENT - 1))


/*
 //:~ TODO
 */
#pragma mark - magic number
#define YX_MEMORY_MAGICNUM_STACK_UNINIT 0xCCCCCCCC

#define YX_MEMORY_MAGICNUM_HEAP_UNINIT 0xCDCDCDCD
#define YX_MEMORY_MAGICNUM_HEAP_FREED 0xFEEEFEEE
#define YX_MEMORY_MAGICNUM_HEAP_GUARD 0xABABABAB
#define YX_MEMORY_MAGICNUM_HEAP_MALLOCED 0xBAADF00D



#endif










