//
//  yx_macosx_process_info.c
//  YXLib
//
//  Created by LiuYuxi on 15/8/11.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_macosx_process_info.h"

#include "yx_macosx_process.h"
#include "../allocator/yx_macosx_allocator.h"



yx_bool yx_os_processinfo_withpid(yx_os_processinfo_ref processinfo, yx_os_process_ref process)
{
    assert(NULL != processinfo);
    assert(NULL != process);
    
    int mib[4];
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PID;
    mib[3] = yx_os_process_handle(process);
    
    yx_size len = sizeof(yx_os_processinfo);
    if (yx_unlikely( 0 != sysctl(mib, 4, processinfo, &len, NULL, 0) ))
        return yx_false;
    
    
    return yx_true;
}




yx_bool yx_os_processinfobuf_init(yx_allocator allocator, yx_os_processinfo_buf_ref processinfo_buf)
{
    assert(NULL != processinfo_buf);
    
    processinfo_buf->allocator = allocator;
    processinfo_buf->info_buf = NULL;
    processinfo_buf->buf_size = 0;
    processinfo_buf->count = 0;
    
    return yx_true;
}
void yx_os_processinfobuf_recycle(yx_os_processinfo_buf_ref processinfo_buf)
{
    assert(NULL != processinfo_buf);
    
    if (NULL != processinfo_buf->info_buf)
    {
        yx_allocator_free(processinfo_buf->allocator, processinfo_buf->info_buf);
    }
}

yx_bool yx_os_processinfobuf_current(yx_os_processinfo_buf_ref processinfo_buf)
{
    assert(NULL != processinfo_buf);
    
    int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL ,0};
    
    int err;
    yx_size size;
    
    
    /*记录当前数值*/
    yx_os_processinfo_ref info_buf = processinfo_buf->info_buf;
    yx_allocator allocator = processinfo_buf->allocator;
    yx_size buf_size = processinfo_buf->buf_size;
    
    /*clear buf*/
    processinfo_buf->info_buf = NULL;
    processinfo_buf->count = 0;
    processinfo_buf->buf_size = 0;
    
    
    
    do
    {
        /*计算buf所需的空间*/
        size = 0;
        if (yx_unlikely(sysctl(mib, 4, NULL, &size, NULL, 0)))
            goto errout;
        
        
        /*
         1. 多预留10个空间
         2. 空间按照1024取整
         */
        size += 10 * sizeof(struct kinfo_proc);
        size = 1024 * ((size + 1023) / 1024);
        
        
        /*如果空间不够，释放掉原有空间，申请新空间*/
        if (buf_size < size)
        {
            if (NULL != info_buf)
                yx_allocator_free(allocator, info_buf);
            
            info_buf = yx_allocator_alloc(processinfo_buf->allocator, size);
            if (yx_unlikely( NULL == info_buf ))
                goto errout;
            
            buf_size = size;
        }
        
        
        /*获取进程列表*/
        err = sysctl(mib, 4, info_buf, &size, NULL, 0);
        
        
        /*如果因为buf不够，获取失败，重新尝试获取 */
    }while (0 != err  &&  ENOMEM == errno);
    
    
    /*是否获取失败 */
    if (yx_unlikely( 0 != err  ))
        goto errout;
    
    
    processinfo_buf->info_buf = info_buf;
    processinfo_buf->buf_size = buf_size;
    processinfo_buf->count = (yx_int)(size / sizeof(struct kinfo_proc));

    
    return yx_true;
    
errout:
    if (NULL != info_buf)
        yx_allocator_free(processinfo_buf->allocator, info_buf);
    
    return yx_false;
}


