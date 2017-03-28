//
//  yx_macosx_allocator.c
//  YXLib
//
//  Created by Yuxi Liu on 9/26/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#include "yx_macosx_allocator.h"

#include "../yx_macosx_global.h"
#include "yx_macosx_mem.h"

yx_allocator global_allocator = YX_HANDLE_NULL;
yx_allocator persistent_allocator = YX_HANDLE_NULL;



static
yx_ptr _yx_stdC_alloc(yx_allocator handle, yx_size size){
    yx_unused(handle);
    yx_unused(size);
    
#if (YX_LONG_SIZE == YX_ALIGNMENT)
    return yx_os_malloc(size);
#else
    return yx_os_memalign(YX_ALIGNMENT, size);
#endif
    
}


static
yx_ptr _yx_stdC_memalign(yx_allocator allocator, yx_size size, yx_size alignment){
    yx_unused(allocator);
    
    return yx_os_memalign(alignment, size);
}



static
void _yx_stdC_free(yx_allocator handle, yx_ptr ptr){
    yx_unused(handle);
    yx_unused(ptr);
    
    yx_os_free(ptr);
}


yx_allocator yx_stdC_allocator(void){
    
    static yx_allocator_context allocator = {
                                            yx_rtti_allocator_stdC,
                                            YX_HANDLE_NULL,
                                            _yx_stdC_alloc,
                                            _yx_stdC_memalign,
                                            _yx_stdC_free};
    
    
    return (yx_allocator)(&allocator);
}



