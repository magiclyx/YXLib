//
//  yx_macosx_pipe.h
//  YXLib
//
//  Created by LiuYuxi on 15/8/12.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_PIPE_H_
#define _YX_MACOSX_PIPE_H_

#include "../basictypes/yx_macosx_types.h"

typedef enum yx_os_pipe_type{
    yx_os_pipe_readonly = 1,
    yx_os_pipe_writeonly = 2,
    yx_os_pipe_readwrite = 3,
}yx_os_pipe_type;

typedef struct yx_os_pipe{
    int pfd[2]; //0 write, 1 read
    yx_os_pipe_type type;
}yx_os_pipe;

typedef struct yx_os_pipe* yx_os_pipe_ref;

yx_bool yx_os_pipe_init_noname(yx_os_pipe_ref p, yx_os_pipe_type type);
void yx_os_pipe_recycle(yx_os_pipe_ref p);

#define yx_os_pipe_canwrite(pipe)     (yx_os_pipe_writeonly != (pipe)->type)
#define yx_os_pipe_canread(pipe)      (yx_os_pipe_writeonly != (pipe)->type)

#define yx_os_pipe_readHandle(pipe)   ((pipe)->pfd[1])
#define yx_os_pipe_writeHandle(pipe)  ((pipe)->pfd[0])

yx_bool yx_os_pipe_bind2stdio(yx_os_pipe_ref p);


yx_int yx_os_pipe_read(yx_os_pipe_ref p, yx_byte *buf, yx_size bufSize, yx_size *bytesRead);
yx_int yx_os_pipe_write(yx_os_pipe_ref p, yx_byte *buf, yx_size bufSize, yx_size *bytesWritten);


#endif /* defined(_YX_MACOSX_PIPE_H_) */


















































