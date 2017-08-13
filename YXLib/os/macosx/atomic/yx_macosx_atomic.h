//
//  yx_macosx_atomic.h
//  YXLib
//
//  Created by Yuxi Liu on 2017/8/13.
//  Copyright © 2017年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_ATOMIC_H_
#define _YX_MACOSX_ATOMIC_H_

#include "../yx_macosx_global.h"

typedef _Atomic(yx_bool)       yx_os_atomic_bool;
typedef _Atomic(yx_byte)       yx_os_atomic_byte;
typedef _Atomic(yx_ptr)        yx_os_atomic_ptr;

typedef _Atomic(yx_char)       yx_os_atomic_char;
typedef _Atomic(yx_uchar)      yx_os_atomic_uchar;
typedef _Atomic(yx_int)        yx_os_atomic_int;
typedef _Atomic(yx_uint)       yx_os_atomic_uint;

typedef _Atomic(yx_char8)      yx_os_atomic_char8;
typedef _Atomic(yx_uchar8)     yx_os_atomic_uchar8;

typedef _Atomic(yx_int8)       yx_os_atomic_int8;
typedef _Atomic(yx_uint8)      yx_os_atomic_uint8;
typedef _Atomic(yx_int16)      yx_os_atomic_int16;
typedef _Atomic(yx_uint16)     yx_os_atomic_uint16;
typedef _Atomic(yx_int32)      yx_os_atomic_int32;
typedef _Atomic(yx_uint32)     yx_os_atomic_uint32;
typedef _Atomic(yx_int64)      yx_os_atomic_int64;
typedef _Atomic(yx_uint64)     yx_os_atomic_uint64;



/*****************************************************************************************************/
/*Add*/
/*****************************************************************************************************/

#define yx_atomic_add(atomic_ptr, value) \
    atomic_fetch_add(atomic_ptr, value)

#define yx_atomic_add_explicit(atomic_ptr, value, mem_order) \
    atomic_fetch_add_explicit(atomic_ptr, value, mem_order)



/*****************************************************************************************************/
/*Sub*/
/*****************************************************************************************************/

#define yx_atomic_sub(atomic_ptr, value) \
    atomic_fetch_sub(atomic_ptr, value)

#define yx_atomic_sub_explicit(atomic_ptr, value, mem_order) \
    atomic_fetch_sub_explicit(atomic_ptr, value, mem_order)


/*****************************************************************************************************/
/*And*/
/*****************************************************************************************************/

#define yx_atomic_and(atomic_ptr, value) \
    atomic_fetch_and(atomic_ptr, value)

#define yx_atomic_and_explicit(atomic_ptr, value, mem_order) \
    atomic_fetch_and_explicit(atomic_ptr, value, mem_order)


/*****************************************************************************************************/
/*Or*/
/*****************************************************************************************************/

#define yx_atomic_or(atomic_ptr, value) \
    atomic_fetch_or(atomic_ptr, value)

#define yx_atomic_or_explicit(atomic_ptr, value, mem_order) \
    atomic_fetch_or_explicit(atomic_ptr, value, mem_order)


/*****************************************************************************************************/
/*Xor*/
/*****************************************************************************************************/

#define yx_atomic_xor(atomic_ptr, value) \
    atomic_fetch_xor(atomic_ptr, value)

#define yx_atomic_xor_explicit(atomic_ptr, value, mem_order) \
    atomic_fetch_xor_explicit(atomic_ptr, value, mem_order)


/*****************************************************************************************************
* Compare and Exchange
*  if (memcmp(obj, expected, sizeof *obj) == 0)
*      memcpy(obj, &desired, sizeof *obj);
*  else
*      memcpy(expected, obj, sizeof *obj);
*****************************************************************************************************/
#define yx_atomic_cmpxchg(atomic_ptr, expected_ptr, desired) \
    atomic_compare_exchange_strong(atomic_ptr, expected_ptr, desired)

#define yx_atomic_cmpxchg_explicit(atomic_ptr, expected_ptr, desired, mem_order_succ, mem_order_fail) \
    atomic_compare_exchange_strong(atomic_ptr, expected_ptr, desired, mem_order_succ, mem_order_fail)



#endif /* _YX_MACOSX_ATOMIC_H_ */










