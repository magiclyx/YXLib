//
//  yx_testcase_core_mempool_global.h
//  YXLib
//
//  Created by Yuxi Liu on 2/10/15.
//  Copyright (c) 2015 Yuxi Liu. All rights reserved.
//

#ifndef _YX_TESTCASE_CORE_MEMPOOL_GLOBAL_H_
#define _YX_TESTCASE_CORE_MEMPOOL_GLOBAL_H_

#include "../../../core/yx_core_headers.h"
#include "../yx_testcase_core_global.h"


typedef struct _yx_testutility_allocmem_context{
    yx_allocator allocator;
    yx_irange range;
    yx_uint times;
    yx_bool test_readwrte;
    
    void** mem_array;
    yx_size* size_array;
}*_yx_testutility_alloc_mem;


//create & destroy
_yx_testutility_alloc_mem _yx_testutility_allocmem_create(yx_allocator allocator, yx_irange range, yx_uint times);
void _yx_testutility_allocmem_destroy(_yx_testutility_alloc_mem* mem_ref);



/***************************************************
 operation
 ***************************************************/

/*whether using read/write tests*/
void _yx_testutility_allocmem_enableRWTest(_yx_testutility_alloc_mem mem, yx_bool enable);
void _yx_testutility_allocmem_setAllocator(_yx_testutility_alloc_mem mem, yx_allocator allocator);



/***************************************************
 alloc & release
 ***************************************************/

/*alloc the memory*/
yx_bool _yx_testutility_allocmem_alloc(_yx_testutility_alloc_mem mem);
/*Repeat the last distribution scenario*/
yx_bool _yx_testutility_allocmem_realloc(_yx_testutility_alloc_mem mem);
/*test the mem by using memalign. all mem are larger than sizeof(void*) and its size are power of 2*/
yx_bool _yx_testutility_allocmem_memalign(_yx_testutility_alloc_mem mem);
/*alloc the memory, the size of mem a multiple of the YX_ALIGNMENT*/
yx_bool _yx_testutility_allocmem_allocBlock(_yx_testutility_alloc_mem mem);
/*free the memory*/
yx_bool _yx_testutility_allocmem_free(_yx_testutility_alloc_mem mem);


/***************************************************
 test the mem
 ***************************************************/
/*test the memory*/
yx_bool _yx_testutility_allocmem_test(_yx_testutility_alloc_mem mem);

/***************************************************
 info
 ***************************************************/
yx_size _yx_testutility_allocmem_totleSize(_yx_testutility_alloc_mem mem);

/***************************************************
 others
 ***************************************************/
#define _yx_testutility_allocmem_count(mem) (mem->times)
#define _yx_testutility_allocmem_memAtIndex(mem, index) (mem->mem_array[index])
#define _yx_testutility_allocmem_sizeAtIndex(mem, index) (mem->size_array[index])





////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
/*standard checking*/
yx_bool _yx_testutility_check_allocator(yx_allocator allocator);

#endif /* defined(_YX_TESTCASE_CORE_MEMPOOL_GLOBAL_H_) */






