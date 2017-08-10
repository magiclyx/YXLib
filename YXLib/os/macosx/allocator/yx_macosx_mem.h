//
//  yx_macosx_alloc.h
//  YXLib
//
//  Created by Yuxi Liu on 9/4/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_MEM_H_
#define _YX_MACOSX_MEM_H_

#include "../yx_macosx_global.h"


#define yx_os_alloc(size)  malloc(size)
#define yx_os_calloc(size) malloc(size)
#define yx_os_malloc(size) malloc(size)

static yx_inline void* yx_os_memalign(size_t alignment, size_t size){
    void* p;
    posix_memalign(&p, alignment, size);
    return p;
}


#define yx_os_free(pt) free(pt)



#define yx_os_memcpy(dst, src, len)         (void) memcpy(dst, src, len)
#define yx_os_memset(dst, c, len)           (void) memset(dst, c, len)
#define yx_os_memzero(mem, len)             (void) memset(mem, 0, len)
#define yx_os_memcmp(mem1, mem2, len)       memcmp((const yx_char*)mem1, (const yx_char*)mem2, len)
#define yx_os_memmove(dst, src, len)        (void) memmove(dst, src, len)

#define yx_os_memfill(dst, src, n)          (((yx_byte *) memcpy(dst, src, n)) + (n))
#define yx_os_memfillmem(dst, src, n)       (((yx_byte *) memmove(dst, src, n)) + (n))


#endif
