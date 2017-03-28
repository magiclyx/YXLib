//
//  sample.c
//  YXLib
//
//  Created by Yuxi Liu on 11/8/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#include "sample.h"


#if use_type_1

/*方式一*/

typedef struct _yx_sample_context{
    yx_handle allocator;
    
    //others ...
}yx_sample_context, *yx_smple_context_ref;






yx_sample yx_sample_create(yx_allocator allocator){
    
    yx_smple_context_ref context;
    
    /*这里，如果使用者没有传递allocator, 就是用当前全局设置的allocator*/
    if (NULL == allocator) {
        allocator = yx_get_allocator();
    }
    
    context = (yx_smple_context_ref)yx_allocator_alloc(allocator, sizeof(yx_sample_context));
    if (NULL == context) {
        /*
         do something ....
         */
    }
    
    /*将全局设置的allocator设置给context*/
    context->allocator = allocator;
    
    return (yx_sample)context; //这里要写明强转
}



void yx_sample_destroy(yx_allocator* sample_ptr){
    
    if (NULL == sample_ptr){
        /*do something ...*/
    }
    
    /*创建本地context,结构。 然后给sampleRef_ptr赋空。 这样调用者所持有的指针为空*/
    yx_smple_context_ref context = (yx_smple_context_ref)(*sample_ptr);
    *sample_ptr = NULL;
    
    
    
    /*
     release xxxwx
     */
    yx_allocator allocator = context->allocator; //这个看需要，也可以直接把context->allocator 传递给yx_allocator_free
    
    yx_allocator_free(allocator, context);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
#else
////////////////////////////////////////////////////////////////////////////////////////////////////

/*方式二*/

yx_smple_ref yx_sample_create(yx_allocator allocator){
    
    yx_smple_ref context;
    
    /*这里，如果使用者没有传递allocator, 就是用当前全局设置的allocator*/
    if (NULL == allocator) {
        allocator = yx_get_allocator();
    }
    
    context = (yx_smple_ref)yx_allocator_alloc(allocator, sizeof(yx_sample));
    if (NULL == context) {
        /*
         do something ....
         */
    }
    
    /*将全局设置的allocator设置给context*/
    context->allocator = allocator;
    
    return context;
}

void yx_sample_destroy(yx_smple_ref* sampleRef_ptr){
    
    if (NULL == sampleRef_ptr){
        /*do something ...*/
    }
    
    /*创建本地context,结构。 然后给sampleRef_ptr赋空。 这样调用者所持有的指针为空*/
    yx_smple_ref context = (yx_smple_ref)(*sampleRef_ptr);
    *sampleRef_ptr = NULL;
    
    
    
    /*
     release xxxwx
     */
    yx_allocator allocator = context->allocator; //这个看需要，也可以直接把context->allocator 传递给yx_allocator_free
    
    yx_allocator_free(allocator, context);
    
}


#endif //use_type_1









