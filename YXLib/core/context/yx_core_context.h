//
//  yx_core_context.h
//  YXLib
//
//  Created by LiuYuxi on 15/10/10.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_CONTEXT_H_
#define _YX_CORE_CONTEXT_H_

#include "../yx_core_global.h"

typedef struct yx_core_context{
    yx_allocator basic_allocator; /*基本内存分配器*/
    yx_allocator raw_allocator; /*使用std-c的分配器，用于生命周期为整个进程或不影响性能的一次性分配*/
}yx_core_context;


extern yx_core_context core_context;

yx_bool yx_core_context_init(void);
yx_bool yx_core_context_recycle(void);




#endif /* _YX_CORE_CONTEXT_H_ */
