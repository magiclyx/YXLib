//
//  yx_core_rtti.h
//  YXLib
//
//  Created by LiuYuxi on 15/4/17.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_RTTIDEF_H_
#define _YX_CORE_RTTIDEF_H_

#define yx_rtti_core_offset(offset)   (yx_rtti_os_base + offset)


////////////////////////////////////////////////////
/*reserved 0 - 999 offset value from os rtti base*/
#define yx_rtti_core_base             (yx_rtti_core_offset(999))
////////////////////////////////////////////////////



/*special (offset 0 - 9) */


/*mempool (offset 10 - 20)*/
#define  yx_rtti_allocator_basic       (yx_rtti_core_offset(10)) //basic memory pool
#define  yx_rtti_allocator_debug       (yx_rtti_core_offset(11)) //debug memory pool
#define  yx_rtti_allocator_memsection         (yx_rtti_core_offset(12)) //memsecion
#define  yx_rtti_allocator_membucket        (yx_rtti_core_offset(13)) //membucket
#define  yx_rtti_allocator_thread      (yx_rtti_core_offset(14)) //thread mempory pool

#define  yx_rtti_allocator_ts_memsection  (yx_rtti_core_offset(16)) //thread safe memsection
#define  yx_rtti_allocator_ts_membucket   (yx_rtti_core_offset(17)) //thread safe membucket


/*container (offset 30 - 70)*/
#define  yx_core_rtti_list             (yx_rtti_core_offset(30)) //list
#define  yx_core_rtti_queue            (yx_rtti_core_offset(31)) //list
#define  yx_core_rtti_rbtree           (yx_rtti_core_offset(32)) //rbtree
#define  yx_core_rtti_hashtable        (yx_rtti_core_offset(33)) //rbtree




#endif /* defined(_YX_CORE_RTTIDEF_H_) */



