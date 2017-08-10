//
//  yx_core_comcontact.h
//  YXLib
//
//  Created by LiuYuxi on 15/8/12.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_COMMUNICATOR_H_
#define _YX_CORE_COMMUNICATOR_H_

#include "../yx_core_global.h"

#include "yx_core_communicator_interface.h"
#include "yx_core_communicator_type.h"


/*通信类型*/
typedef enum yx_core_comcontact_type{
    yx_core_comcontact_nonnamepipe,
    yx_core_comcontact_socket,
}yx_core_comcontact_type;



typedef struct yx_core_communicator{
    
    yx_core_comcontact_type type;
    yx_allocator allocator;
    
    /*bind 是将communicator, 绑定到某个拥有者中。 value可以用来引用拥有者*/
    yx_value bind_value;
    yx_value bind_type;
    
    /*unbind 意味着连接断开, 这个回调是为了通知communicator的拥有者, 通信结束*/
    void (*willUnBind_callback)(struct yx_core_communicator* communicator);
    
    union{
        yx_os_pipe pipe;
    }com_handle;
    
    yx_core_commoperation operation;
    
}yx_core_communicator;

typedef struct yx_core_communicator* yx_core_communicator_ref;



/*
 create & destroy
 */
yx_core_communicator_ref yx_core_communicator_create(yx_allocator allocator, yx_core_commgenerator_ref gen);
void yx_core_communicator_destroy(yx_core_communicator_ref* communicator_ptr);


/*
 init & recycle
 */
yx_bool yx_core_communicator_init(yx_allocator allocator,
                                  yx_core_communicator_ref communicator,
                                  yx_core_commgenerator_ref gen);

void yx_core_communicator_recycle(yx_core_communicator_ref communicator);


/*
 bind the communicator to an object
 */
yx_bool yx_core_communicator_bind(yx_core_communicator_ref communicator, yx_value bind_type, yx_value value, void (*willUnBind_callback)(yx_core_communicator_ref communicator));


#define yx_core_communicator_bindtype(communicator)    (communicator->bind_type)
#define yx_core_communicator_bindvalue(communicator)   (communicator->bind_value)
#define yx_core_communicator_hasbind(communicator)     (yx_core_comcontact_bind_none != communicator->bind_type)



/*
 read & write
 */

#define yx_core_communicator_read(comm, buf, bufSize, bytesRead) \
    ( (comm)->operation.read(comm, buf, bufSize, bytesRead) )

#define yx_core_communicator_write(comm, buf, bufSize, bytesRead) \
    ( (comm)->operation.write(comm, buf, bufSize, bytesRead) )


#define yx_core_communicator_read_to_corebuf(comm, core_buf, size_ptr) \
    ( (comm)->operation.corebuf_read(comm, core_buf, size_ptr) )

#define yx_core_communicator_write_from_corebuf(comm, core_buf, size_ptr) \
    ( (comm)->operation.corebuf_write(comm, core_buf, size_ptr) )

/*
 size
 */

#define yx_core_communicator_sizeCanRead_withEventDriver(comm, event_info) \
    ( (comm)->operation.size_can_read_with_eventinfo(comm, event_info) )


#define yx_core_communicator_sizeCanWrite_withEventDriver(comm, event_info) \
    ( (comm)->operation.size_can_write_with_eventinfo(comm, event_info) )

#endif /* defined(_YX_CORE_COMMUNICATOR_H_) */






















































