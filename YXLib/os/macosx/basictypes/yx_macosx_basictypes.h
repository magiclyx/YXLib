//
//  yx_os_basictypes_mac.h
//  YXLib
//
//  Created by Yuxi Liu on 9/4/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_BASICTYPES_H_
#define _YX_MACOSX_BASICTYPES_H_



#pragma mark - int

/**
 define int
 */

typedef int8_t      yx_int8;
typedef uint8_t     yx_uint8;

typedef int16_t     yx_int16;
typedef uint16_t    yx_uint16;

typedef int32_t     yx_int32;
typedef uint32_t    yx_uint32;

typedef int64_t     yx_int64;
typedef uint64_t    yx_uint64;



#pragma mark - short

/**
 define short
 */

typedef int8_t      yx_short8;
typedef uint8_t     yx_ushort8;

typedef int16_t     yx_short16;
typedef uint16_t    yx_ushort16;

typedef int32_t     yx_short32;
typedef uint32_t    yx_ushort32;




#pragma mark - long

/**
 long
 */

typedef int32_t     yx_long32;
typedef uint32_t    yx_ulong32;

typedef int64_t     yx_long64;
typedef uint64_t    yx_ulong64;



#pragma mark - long long

/**
 long long
 */
typedef int32_t yx_longlong32;
typedef uint32_t yx_ulonglong32;

typedef int64_t yx_longlong64;
typedef uint64_t yx_ulonglong64;



#pragma mark - double & float

/**
 double & float
 */
typedef double yx_double64;
typedef float yx_float32;


#pragma mark - char

/**
 char
 */
typedef char             yx_char8;
typedef unsigned char    yx_uchar8;




#pragma mark max value

/**
 max_int & min_int
 */

#define YX_MAX_INT8      127
#define YX_MAX_INT16     32767
#define YX_MAX_INT32     2147483647
#define YX_MAX_INT64     9223372036854775807LL




#define YX_MIN_INT8      -128
#define YX_MIN_INT16     -32768
/**
 Note:  the literal "most negative int" cannot be written in C --
 the rules in the standard (section 6.4.4.1 in C99) will give it
 an unsigned type, so INT32_MIN (and the most negative member of
 any larger signed type) must be written via a constant expression.
 */
#define YX_MIN_INT32     (-INT32_MAX-1)
#define YX_MIN_INT64     (-INT64_MAX-1)

#define YX_MAX_UINT8      255
#define YX_MAX_UINT16     65535
#define YX_MAX_UINT32     4294967295U  //0xffffffff
#define YX_MAX_UINT64      18446744073709551615ULL





#pragma mark - value len

/**
 max len of int or uint
 */

#define YX_INT8_LEN    (sizeof("-128") - 1)
#define YX_INT16_LEN   (sizeof("-32768") - 1)
#define YX_INT32_LEN   (sizeof("-2147483648") - 1)
#define YX_INT64_LEN   (sizeof("-9223372036854775808") - 1)

#define YX_UINT8_LEN   (sizeof("256"))
#define YX_UINT16_LEN  (sizeof("65536"))
#define YX_UINT32_LEN  (sizeof("4294967296"))
#define YX_UINT64_LEN  (sizeof("18446744073709551616"))


#pragma mark - NULL

/**
 define NULL
 */
//#undef NULL
//#if defined(__cplusplus)
//#define NULL 0
//#else
//#define NULL ((void *)0)
//#endif


#endif
