//
//  yx_macosx_shmem.c
//  YXLib
//
//  Created by LiuYuxi on 15/8/10.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_macosx_shmem.h"

yx_bool yx_os_shmem_init(yx_os_shm_ref shm, yx_size size)
{
    assert(NULL != shm);
    
    
    shm->size = size;
    shm->addr = (u_char *) mmap(NULL, size,
                                PROT_READ|PROT_WRITE,
                                MAP_ANON|MAP_SHARED, -1, 0);
    
    if (yx_unlikely( MAP_FAILED == shm->addr ))
    {
        shm->addr = NULL;
        return yx_false;
    }
    
    
    return yx_true;

}

yx_bool yx_os_shmem_recycle(yx_os_shm_ref shm)
{
    if (yx_unlikely( munmap((void *) shm->addr, shm->size) == -1 ))
    {
        return yx_false;
    }

    return yx_true;
}

