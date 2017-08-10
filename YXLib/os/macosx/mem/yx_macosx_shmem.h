//
//  yx_macosx_shmem.h
//  YXLib
//
//  Created by LiuYuxi on 15/8/10.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_SHMEM_H_
#define _YX_MACOSX_SHMEM_H_

#include "../yx_macosx_global.h"
#include "../basictypes/yx_macosx_types.h"

typedef struct yx_os_shm
{
    yx_ptr addr;
    yx_size size;
    char* name;
}yx_os_shm;

typedef struct yx_os_shm* yx_os_shm_ref;


yx_bool yx_os_shmem_init(yx_os_shm_ref shm, yx_size size);
yx_bool yx_os_shmem_recycle(yx_os_shm_ref shm);

#endif /* defined(_YX_MACOSX_SHMEM_H_) */
