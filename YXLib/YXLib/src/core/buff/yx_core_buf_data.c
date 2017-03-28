//
//  yx_core_buf_data.c
//  YXLib
//
//  Created by LiuYuxi on 15/10/6.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_buf_data.h"


yx_bool yx_core_buf_appened_ctx_init(yx_core_buf_ref buf, yx_core_buf_appened_ctx_ref ctx)
{
    YX_ASSERT(NULL != buf);
    YX_ASSERT(NULL != ctx);
    
    ctx->size_writen = 0;
    ctx->bufref = buf;
    
    /*最后一个buff中，剩余的空间*/
    if ( yx_core_listnode_isEmpty(&(buf->list_head)) ){
        ctx->space_in_buf = 0;
#if YX_DEBUG
        ctx->plast = NULL;
        ctx->pstart = NULL;
#endif
    }
    else{
        ctx->space_in_buf = buf->buf_size - buf->lastbuf_data_size;
        yx_core_buf_block_ref lastblock = yx_core_listnode_lastNodeOwner(&(buf->list_head), struct yx_core_buf_block, node);
        ctx->pstart = (yx_byte*)lastblock + sizeof(struct yx_core_buf_block);
        ctx->plast = ctx->pstart + buf->lastbuf_data_size;
    }

    return yx_true;
}

yx_bool yx_core_buf_appened_ctx_recycle(yx_core_buf_ref buf, yx_core_buf_appened_ctx_ref ctx)
{
    return yx_core_buf_appened_ctx_sync(buf, ctx);
}

yx_bool yx_core_buf_appened_ctx_sync(yx_core_buf_ref buf, yx_core_buf_appened_ctx_ref ctx)
{
    YX_ASSERT(NULL != buf);
    YX_ASSERT(NULL != ctx);
    
    buf->data_size += ctx->size_writen;
    buf->lastbuf_data_size = (yx_size)(ctx->plast - ctx->pstart);
    
    ctx->size_writen = 0;
    
    return yx_true;
}

yx_bool _yx_core_buf_appened_ctx_expand_(yx_core_buf_appened_ctx_ref ctx)
{
    YX_ASSERT(NULL != ctx);
    YX_ASSERT(0 == ctx->space_in_buf);
    
    yx_core_buf_ref core_buf = ctx->bufref;
    
    yx_core_buf_block_ref bufbloc = (yx_core_buf_block_ref)yx_allocator_alloc(core_buf->buf_allocator, sizeof(struct yx_core_buf_block) + core_buf->buf_size);
    if (yx_unlikely( NULL == bufbloc ))
        return yx_false;
    
    yx_core_listnode_insertTail(&(core_buf->list_head), &(bufbloc->node));
    
    ctx->pstart = (yx_byte*)bufbloc + sizeof(struct yx_core_buf_block);
    ctx->plast = ctx->pstart;
    ctx->space_in_buf = core_buf->buf_size;
    
    core_buf->lastbuf_data_size = 0;
    
    return yx_true;
}


yx_bool yx_core_buf_appened_byteWithCtx(yx_core_buf_appened_ctx_ref ctx, yx_byte byte)
{
    /*如果剩余空间为0， 添加新的buf*/
    if (0 == ctx->space_in_buf)
    {
        yx_core_buf_ref core_buf = ctx->bufref;
        
        yx_core_buf_block_ref bufbloc = (yx_core_buf_block_ref)yx_allocator_alloc(core_buf->buf_allocator, sizeof(struct yx_core_buf_block) + core_buf->buf_size);
        if (yx_unlikely( NULL == bufbloc ))
            return yx_false;
        
        yx_core_listnode_insertTail(&(core_buf->list_head), &(bufbloc->node));
        
        ctx->pstart = (yx_byte*)bufbloc + sizeof(struct yx_core_buf_block);
        ctx->plast = ctx->pstart;
        ctx->space_in_buf = core_buf->buf_size;
        
        core_buf->lastbuf_data_size = 0;
    }
    
    
    *(ctx->plast) = byte;
    ctx->plast++;
    
    ctx->size_writen++;
    ctx->space_in_buf--;

    return yx_true;
}




