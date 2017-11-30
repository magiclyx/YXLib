//
//  yx_core_context.c
//  YXLib
//
//  Created by LiuYuxi on 15/10/10.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_context.h"
#include "../debug/yx_core_assert.h"

#if YX_MEM_VERIFY
    #include "../allocator/yx_core_mempool_debug.h"
#else
    #include "../allocator/yx_core_mempool_basic.h"
#endif


yx_core_context core_context =
{
    NULL,
    NULL,
    NULL,
};


yx_bool yx_core_context_init(void)
{
    
    /***************************************************
     内存池
     ***************************************************/
    
    yx_allocator basic_allocator;
    yx_allocator raw_allocator;
    /*设置全局内存分配器，如果设置了验证内存，则使用调试内存分配器*/
#if YX_MEM_VERIFY
    yx_allocator debug_allocator = yx_debugMempool_create(yx_stdC_allocator(), 10);
    if (yx_unlikely( NULL == debug_allocator ))
        return yx_false;
    yx_debugMempool_assertion(debug_allocator, yx_true);
    
    basic_allocator = raw_allocator = debug_allocator;
#else
    basic_allocator = yx_basicMempool_create(yx_stdC_allocator());
    if (yx_unlikely( NULL == basic_allocator ))
        return yx_false;
    
    raw_allocator = yx_stdC_allocator();
#endif
    
    /*setup the global allocator*/
    core_context.basic_allocator = basic_allocator;
    /*set persistent_allocator*/
    core_context.raw_allocator = raw_allocator;
    

    
    
    //others..
    //

    
    return yx_true;
}

yx_bool yx_core_context_recycle(void)
{
    
    
    //
    //others..
    
    
    
    
    
    /***************************************************
     内存池
     ***************************************************/
    
#if YX_MEM_VERIFY
    YX_ASSERT(core_context.basic_allocator == core_context.raw_allocator);
    yx_allocator debug_allocator = core_context.basic_allocator;
    yx_debugMempool_destroy(&debug_allocator);
#else
    yx_allocator basic_allocator = core_context.basic_allocator;
    yx_basicMempool_destroy(&basic_allocator);
#endif
    
    core_context.basic_allocator = NULL;
    core_context.raw_allocator = NULL;
    

    /***************************************************
     module
     ***************************************************/

    
    return yx_true;
}


