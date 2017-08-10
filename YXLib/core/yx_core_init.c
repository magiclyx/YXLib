//
//  yx_core_init.c
//  YXLib
//
//  Created by LiuYuxi on 15/8/13.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_init.h"



#if YX_MEM_VERIFY
    #include "allocator/yx_core_mempool_debug.h"
    static yx_allocator g_debug_allocator = NULL;
#else
    #include "allocator/yx_core_mempool_basic.h"
    static yx_allocator g_basic_allocator = NULL;
#endif



yx_bool yx_core_setup(int argc, const char** argv)
{
    
    /*
     set the command-line info
     */
    yx_os_environment_save_launch_parameter(argc,  argv);
    

    
    /*
     设置全局内存分配器，如果设置了验证内存，则使用调试内存分配器
     */
#if YX_MEM_VERIFY
    g_debug_allocator = yx_debugMempool_create(yx_stdC_allocator(), 10);
    if (yx_unlikely( NULL == g_debug_allocator ))
        return yx_false;
    
    yx_debugMempool_assertion(g_debug_allocator, yx_true);
    
    
    /*setup the global allocator*/
    yx_set_allocator(g_debug_allocator); //use debug pool
    
    /*set persistent_allocator*/
    yx_set_persistent_allocator(g_debug_allocator); //use debug pool
#else
    g_basic_allocator = yx_basicMempool_create(yx_stdC_allocator());
    if (yx_unlikely( NULL == g_basic_allocator ))
        return yx_false;
    
    /*setup the global allocator*/
    yx_set_allocator(g_basic_allocator); //use basic pool
    
    /*set persistent_allocator*/
    yx_set_persistent_allocator(yx_stdC_allocator()); //use stdC pool
#endif
    
    
    return yx_true;
}


yx_bool yx_core_shutdown(void)
{
    
    
    /*
     清除内存分配器
     */
#if YX_MEM_VERIFY
    yx_set_allocator(NULL);
    yx_set_persistent_allocator(NULL);
    
    yx_debugMempool_destroy(&g_debug_allocator);
    g_debug_allocator = NULL;
#else
    /*clear global allocator*/
    yx_set_allocator(NULL);
    
    yx_basicMempool_destroy(&g_basic_allocator);
    g_basic_allocator = NULL;
#endif

    
    return yx_true;
}







