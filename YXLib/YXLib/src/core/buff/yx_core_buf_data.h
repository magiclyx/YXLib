//
//  yx_core_buf_data.h
//  YXLib
//
//  Created by LiuYuxi on 15/10/6.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_BUF_DATA_H_
#define _YX_CORE_BUF_DATA_H_


#include "../yx_core_global.h"
#include "yx_core_buf.h"

typedef struct yx_core_buf_appened_ctx{
    yx_core_buf_ref bufref;
    yx_byte* plast;
    yx_byte* pstart;
    yx_size size_writen; //总共写入的数据
    yx_size space_in_buf; //最后buff缓冲中，cursor后的空间
}yx_core_buf_appened_ctx;

typedef yx_core_buf_appened_ctx* yx_core_buf_appened_ctx_ref;


yx_bool yx_core_buf_appened_ctx_init(yx_core_buf_ref buf, yx_core_buf_appened_ctx_ref ctx);
yx_bool yx_core_buf_appened_ctx_recycle(yx_core_buf_ref buf, yx_core_buf_appened_ctx_ref ctx);
yx_bool yx_core_buf_appened_ctx_sync(yx_core_buf_ref buf, yx_core_buf_appened_ctx_ref ctx);


yx_bool _yx_core_buf_appened_ctx_expand_(yx_core_buf_appened_ctx_ref ctx);


#define yx_core_buf_ctx_appened_byte(ctx, byte) \
do{ \
    if(0 == ctx->space_in_buf) \
        _yx_core_buf_appened_ctx_expand_(ctx); \
\
    *(ctx->plast) = byte; \
    ctx->plast++; \
\
    ctx->size_writen++; \
    ctx->space_in_buf--; \
\
}while(0)


#endif /* _YX_CORE_BUF_DATA_H_ */










