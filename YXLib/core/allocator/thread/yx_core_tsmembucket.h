//
//  yx_core_tsmembucket.h
//  YXCLib
//
//  Created by Yuxi Liu on 2017/12/3.
//  Copyright © 2017年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_TSMEMBUCKET_H_
#define _YX_CORE_TSMEMBUCKET_H_

#include "../../yx_core_global.h"

yx_allocator yx_tsmembucket_create(yx_allocator parentpool, yx_size cellSize);
void yx_tsmembucket_destroy(yx_allocator* allocator_ref);

yx_ptr yx_tsmembucket_alloc(yx_allocator allocator);
void yx_tsmembucket_free(yx_allocator allocator, yx_ptr ptr);


/*get parent allocator*/
yx_allocator yx_tsmembucket_parentpool(yx_allocator allocator);


#endif /* yx_core_tsmembucket_h */
