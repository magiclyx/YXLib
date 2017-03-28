//
//  yx_core_communicator_pipe.c
//  YXLib
//
//  Created by LiuYuxi on 15/8/21.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_communicator_pipe.h"

#include "../yx_core_communicator.h"
#include "../yx_core_communicator_type.h"
#include "../yx_core_communicator_eventdriver.h"
#include "../../buff/yx_core_buf_pipe.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
/*create & recycle*/
////////////////////////////////////////////////////////////////////////////////////////////////////

static yx_bool __yx_core_communicator_init_nonamepipe(yx_allocator allocator, yx_core_communicator_ref communicator)
{
    yx_unused(allocator);
    
    communicator->allocator = allocator;
    
    communicator->type = yx_core_comcontact_nonnamepipe;
    communicator->bind_type = yx_core_comcontact_bind_none;
    communicator->bind_value = 0;
    
    return yx_os_pipe_init_noname(&(communicator->com_handle.pipe), yx_os_pipe_readwrite);
}

static void __yx_core_communicator_recycle_nonamepipe(yx_core_communicator_ref communicator)
{
    yx_os_pipe_recycle(&(communicator->com_handle.pipe));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/*create & recycle*/
////////////////////////////////////////////////////////////////////////////////////////////////////

static yx_bool __yx_core_communicator_read(yx_core_communicator_ref communicator, yx_byte *buf, yx_size bufSize, yx_size *bytesRead)
{
    return (0 != yx_os_pipe_read(&(communicator->com_handle.pipe), buf, bufSize, bytesRead));
}
static yx_bool __yx_core_communicator_write(yx_core_communicator_ref communicator, yx_byte *buf, yx_size bufSize, yx_size *bytesWritten)
{
    return (0 != yx_os_pipe_write(&(communicator->com_handle.pipe), buf, bufSize, bytesWritten));
}

static yx_bool __yx_core_communicator_corebuf_read(yx_core_communicator_ref communicator, yx_core_buf_ref core_buf, yx_size* size)
{
    return ( 0 == yx_core_buf_collect_from_pipe(core_buf, &(communicator->com_handle.pipe), size) );
}
static yx_bool __yx_core_communicator_corebuf_write(yx_core_communicator_ref communicator, yx_core_buf_ref core_buf, yx_size* size)
{
    return ( 0 == yx_core_buf_dump_to_pipe(core_buf, &(communicator->com_handle.pipe), size) );
}



////////////////////////////////////////////////////////////////////////////////////////////////////
/*event driver*/
////////////////////////////////////////////////////////////////////////////////////////////////////
static yx_bool __yx_core_communicator_register_dirver(yx_core_communicator_ref communicator, yx_os_eventdriver_ref eventdirver, yx_core_comeventdirver_opt opt, yx_value userdata)
{
    switch (opt) {
        case yx_core_comevent_read:
            return yx_os_eventdriver_add_pipe_read(eventdirver, &(communicator->com_handle.pipe), userdata);
            break;
        case yx_core_comevent_write:
            return yx_os_eventdriver_add_pipe_write(eventdirver, &(communicator->com_handle.pipe), userdata);
            break;
        default:
            break;
    }
    
    return yx_true;
}

static yx_bool __yx_core_communicator_unregister_dirver(yx_core_communicator_ref communicator, yx_os_eventdriver_ref eventdirver, yx_core_comeventdirver_opt opt)
{
    switch (opt) {
        case yx_core_comevent_read:
            return yx_os_eventdriver_del_piple_read(eventdirver, &(communicator->com_handle.pipe));
            break;
        case yx_core_comevent_write:
            return yx_os_eventdriver_del_piple_write(eventdirver, &(communicator->com_handle.pipe));
            break;
        default:
            break;
    }
    
    return yx_true;

}

static yx_size __yx_core_communicator_size_can_read_with_eventinfo(yx_core_communicator_ref communicator, yx_os_event_info_ref eventinfo)
{
    yx_unused(communicator);
    return yx_os_eventinfo_pipe_sizecanread(eventinfo);
}
static yx_size __yx_core_communicator_size_can_write_with_eventinfo(yx_core_communicator_ref communicator, yx_os_event_info_ref eventinfo)
{
    yx_unused(communicator);
    return yx_os_eventinfo_pipe_sizecanwrite(eventinfo);
}



////////////////////////////////////////////////////////////////////////////////////////////////////
/*set up*/
////////////////////////////////////////////////////////////////////////////////////////////////////

static void __compipe_setup_operator(yx_core_communicator_ref communicator, struct yx_core_commgenerator* generator)
{
    yx_unused(generator);
    
    communicator->operation.init = __yx_core_communicator_init_nonamepipe;
    communicator->operation.recycle = __yx_core_communicator_recycle_nonamepipe;
    
    communicator->operation.read = __yx_core_communicator_read;
    communicator->operation.write = __yx_core_communicator_write;
    
    communicator->operation.corebuf_read = __yx_core_communicator_corebuf_read;
    communicator->operation.corebuf_write = __yx_core_communicator_corebuf_write;
    
    communicator->operation.regist_event = __yx_core_communicator_register_dirver;
    communicator->operation.unregist_event = __yx_core_communicator_unregister_dirver;
    
    communicator->operation.size_can_read_with_eventinfo = __yx_core_communicator_size_can_read_with_eventinfo;
    communicator->operation.size_can_write_with_eventinfo = __yx_core_communicator_size_can_write_with_eventinfo;
}


void yx_core_commpipe_init(yx_core_commgenerator_ref generator)
{
    generator->setup_operator = __compipe_setup_operator;
}






