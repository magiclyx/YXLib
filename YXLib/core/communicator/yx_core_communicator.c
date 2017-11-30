//
//  yx_core_comcontact.c
//  YXLib
//
//  Created by LiuYuxi on 15/8/12.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_communicator.h"
#include "../debug/yx_core_assert.h"

#include "implementation/yx_core_communicator_pipe.h"



yx_core_communicator_ref yx_core_communicator_create(yx_allocator allocator, yx_core_commgenerator_ref gen)
{
    if (NULL == allocator)
        allocator = yx_get_allocator();
    
    yx_core_communicator_ref communicator = yx_allocator_alloc(allocator, sizeof(struct yx_core_communicator));
    if (yx_unlikely( NULL == communicator ))
        goto errout;
    
    if (yx_unlikely( ! yx_core_communicator_init(allocator, communicator, gen) ))
        goto errout;
    
    
    return communicator;
    
errout:
    if (NULL != communicator)
        yx_allocator_free(allocator, communicator);
    
    return yx_false;
}


void yx_core_communicator_destroy(yx_core_communicator_ref* communicator_ptr)
{
    yx_core_communicator_ref communicator = *communicator_ptr;
    *communicator_ptr = NULL;
    
    yx_core_communicator_recycle(communicator);
    
    yx_allocator_free(communicator->allocator, communicator);
}


yx_bool yx_core_communicator_init(yx_allocator allocator, yx_core_communicator_ref communicator,
                                  yx_core_commgenerator_ref gen)
{
    assert(NULL != allocator);
    assert(NULL != communicator);
    assert(NULL != gen);
    
    
    /*set up operation*/
    gen->setup_operator(communicator, gen);
    
    
    /*call binding callback*/
    if (NULL != communicator->willUnBind_callback)
        communicator->willUnBind_callback(communicator);
    
    
    
    YX_ASSERT(NULL != communicator->operation.init);
    /*init the communicator*/
    return communicator->operation.init(allocator, communicator);
    
}

void yx_core_communicator_recycle(yx_core_communicator_ref communicator)
{
    YX_ASSERT(NULL != communicator);
    YX_ASSERT(NULL != communicator->operation.recycle);
    
    
    communicator->operation.recycle(communicator);
}


yx_bool yx_core_communicator_bind(yx_core_communicator_ref communicator, yx_value bind_type, yx_value value, void (*willUnBind_callback)(yx_core_communicator_ref communicator))
{
    YX_ASSERT(NULL != communicator);
    
    communicator->bind_type = bind_type;
    communicator->bind_value = value;
    communicator->willUnBind_callback = willUnBind_callback;
    
    return yx_true;
}




