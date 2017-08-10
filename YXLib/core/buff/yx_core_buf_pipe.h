//
//  yx_core_buf_pipe.h
//  YXLib
//
//  Created by LiuYuxi on 15/8/24.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_BUF_PIPE_H_
#define _YX_CORE_BUF_PIPE_H_

#include "../yx_core_global.h"
#include "yx_core_buf.h"

/*
 将buf中size长度的数据尝试写入管道, 写入成功后，更改buf游标
 */
yx_int yx_core_buf_dump_to_pipe(yx_core_buf_ref core_buf, yx_os_pipe_ref pipe, yx_size* size);

/*
 从管道尝试收集size长度的数据, 收集成功后，会append到buf末尾且不会改变buf指针
 */
yx_int yx_core_buf_collect_from_pipe(yx_core_buf_ref core_buf, yx_os_pipe_ref pipe, yx_size* size);


/*
 尝试从管道读取数据，写入buf当前游标位置。 写入成功后移动游标
 */
yx_int yx_core_buf_write_from_pipe(yx_core_buf_ref core_buf, yx_os_pipe_ref pipe, yx_size* size);




#endif /* defined(_YX_CORE_BUF_PIPE_H_) */
