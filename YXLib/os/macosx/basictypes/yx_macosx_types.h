//
//  yx_macosx_type.h
//  YXLib
//
//  Created by Yuxi Liu on 9/25/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_TYPES_H_
#define _YX_MACOSX_TYPES_H_

#include "../yx_macosx_global.h"
#include "yx_macosx_basictypes.h"


/*char*/
#pragma mark - char
#if(1 == YX_CHAR_SIZE)

typedef yx_char8 yx_char;
typedef yx_uchar8 yx_uchar;
#define YX_MIN_CHAR YX_MIN_INT8
#define YX_MAX_CHAR YX_MAX_INT8
#define YX_MAX_UCHAR YX_MAX_UINT8

#else
#error "unknown char size"
#endif


/*short size*/
#pragma mark - short
#if(2 == YX_SHORT_SIZE)

typedef yx_short16 yx_short;
typedef yx_ushort16 yx_ushort;
#define YX_MIN_SHORT YX_MIN_INT16
#define YX_MAX_SHORT YX_MAX_INT16
#define YX_MAX_USHORT YX_MAX_UINT16
#define YX_SHORT_LEN YX_INT16_LEN
#define YX_USHORT_LEN YX_UINT16_LEN

#else
#error "unknown short size"
#endif



/*int*/
#pragma mark - int
#if(2 == YX_INT_SIZE)

typedef yx_int16 yx_int;
typedef yx_uint16 yx_uint;
#define YX_MAX_INT YX_MAX_INT16
#define YX_MAX_UINT YX_MAX_UINT16
#define YX_MIN_UINT YX_MIN_INT16
#define YX_INT_LEN YX_MAX_INT16_LEN
#define YX_UINT_LEN YX_MAX_UINT16_LEN

#elif(4 == YX_INT_SIZE)

typedef yx_int32 yx_int;
typedef yx_uint32 yx_uint;
#define YX_MAX_INT YX_MAX_INT32
#define YX_MAX_UINT YX_MAX_UINT32
#define YX_MIN_INT YX_MIN_INT32
#define YX_INT_LEN YX_MAX_INT32_LEN
#define YX_UINT_LEN YX_MAX_UINT32_LEN


#elif(8 == YX_INT_SIZE)

typedef yx_int64 yx_int;
typedef yx_uint64 yx_uint;
#define YX_MAX_INT YX_MAX_INT64
#define YX_MAX_UINT YX_MAX_UINT64
#define YX_MIN_INT YX_MIN_INT64
#define YX_INT_LEN YX_MAX_INT64_LEN
#define YX_UINT_LEN YX_MAX_UINT64_LEN


#else
#error "unknown int size"
#endif



/*long*/
#pragma mark - long
#if(4 == YX_LONG_SIZE)
    typedef yx_int32 yx_long;
    typedef yx_uint32 yx_ulong;
    #define YX_MAX_LONG YX_MAX_INT32
    #define YX_MIN_LONG YX_MIN_INT32
    #define YX_MAX_ULONG YX_MAX_UINT32
    #define YX_LONG_LEN YX_MAX_INT32_LEN
    #define YX_ULONG_LEN YX_MAX_UINT32_LEN
#elif(8 == YX_LONG_SIZE)
    typedef yx_int64 yx_long;
    typedef yx_uint64 yx_ulong;
    #define YX_MAX_LONG YX_MAX_INT64
    #define YX_MIN_LONG YX_MIN_INT64
    #define YX_MAX_ULONG YX_MAX_UINT64
    #define YX_LONG_LEN YX_MAX_INT64_LEN
    #define YX_ULONG_LEN YX_MAX_UINT64_LEN
#else
    #error "unknown long size"
#endif



/*long long*/
#pragma mark - long long
#if(8 == YX_LONGLONG_SIZE)
    typedef yx_longlong64 yx_longlong;
    typedef yx_ulonglong64 yx_ulonglong;
    #define YX_MAX_LONGLONG YX_MAX_INT64
    #define YX_MIN_LONGLONG YX_MIN_INT64
    #define YX_MAX_ULONGLONG YX_MAX_UINT64
    #define YX_LONGLONG_LEN YX_MAX_INT64_LEN
    #define YX_ULONGLONG_LEN YX_MAX_UINT64_LEN
#else
    #error "unknown long long size"
#endif


/*real*/
#pragma mark - real
typedef yx_double64   yx_real;


/*index*/
#pragma mark - index
typedef yx_uint32 yx_index;
typedef yx_ulong yx_long_index;

///*yx_ptr_storage*/
//#if(4 == YX_PTR_SIZE)
//typedef yx_ulong32 yx_ptr_storage;
//#elif(8 == YX_PTR_SIZE)
//typedef yx_ulong64 yx_ptr_storage;
//#else
//#error "unknown pointer size"
//#endif


#pragma mark - pointer

typedef void* yx_ptr;

/*address offset*/
#if(8 == YX_PTR_SIZE)
    typedef yx_long64  yx_offset;
#else
    typedef yx_long32 yx_offset;
#endif

/*handle value*/
typedef void* yx_handle;
#define YX_HANDLE_NULL ((yx_handle)0)
#define YX_HANDLE_INVALIDATE ((yx_handle)-1)


#pragma mark - yx_value

#if(4 == YX_VALUE_SIZE)
    //a yx_value must large enough to storage a pointer !!!
    typedef yx_ulong32 yx_value;
#elif(8 == YX_VALUE_SIZE)
    typedef yx_ulong64 yx_value;
#else
#error "unknown value size"
#endif




/*size type*/
#pragma mark - size type
typedef yx_uint32 yx_strSize;
typedef size_t yx_size;
#define YX_SIZE_INVALIDATE ((yx_size)-1)

//#pragma point
//#if 4 == YX_PTR_SIZE
//typedef yx_uint32 yx_offset
//#elif 8 == YX_PTR_SIZE
//typedef size_t yx_size_t;
//#elif
//# error "unknown yx_size_t size"
//#endif



/*byte*/
#pragma mark - byte
typedef yx_uchar8 yx_byte;
#define YX_BYTE_BIT_LEN CHAR_BIT


/*boolean*/
#pragma mark - boolean
typedef yx_char8 yx_bool;
#define yx_false (0)
#define yx_true (!(yx_false))


/*result*/
#pragma mark - result
typedef yx_int32 yx_result;

/*the following 3 items can directly compare with "==" */
#define yx_result_success (0)
#define yx_result_failed  (-1)//(!(yx_result_success))
#define yx_result_unknown (1) //? to be continue...



/*compare result*/
#pragma mark - compare result
typedef yx_int32 yx_compare_result;
#define yx_compare_larger  (1)
#define yx_compare_equal   (0)
#define yx_compare_smaller (-1)


/*state*/
#pragma mark - state
typedef yx_int32 yx_state;

#define yx_state_no      (0<<0)
#define yx_state_yes     (1<<0)
#define yx_state_unknown (1<<1)







#endif
