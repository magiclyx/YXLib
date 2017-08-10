//
//  yx_config_global.h
//  YXLib
//
//  Created by Yuxi Liu on 9/4/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CONFIG_GLOBAL_H_
#define _YX_CONFIG_GLOBAL_H_


/**
 *  some macro utility
 */

#pragma mark - contact
#define _YX_CONCAT_HELPER(text, line) text ## line
#define YX_CONCAT(text, line)          _YX_CONCAT_HELPER(text, line)
#define YX_CONCAT3(x1, x2, x3)         YX_CONCAT(YX_CONCAT(x1, x2), x3)
#define YX_CONCAT4(x1, x2, x3, x4)     YX_CONCAT(YX_CONCAT3(x1, x2, x3), x4)
#define YX_CONCAT5(x1, x2, x3, x4, x5) YX_CONCAT(YX_CONCAT4(x1, x2, x3, x4), x5)


#pragma mark - string
#define _YX_STRINGIZE_HELPER(x) #x
#define YX_STRINGIZE(x)        _YX_STRINGIZE_HELPER(x)


#pragma mark -pointer
/* linux-2.6.38.8/include/linux/stddef.h */
//#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#define YX_OFFSETOF(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

/* linux-2.6.38.8/include/linux/kernel.h */
//#define container_of(ptr, type, member) ({			\
//const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
//(type *)( (char *)__mptr - offsetof(type,member) );})

#define YX_CONTAINER_OF(ptr, type, member) ({			\
const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
(type *)( (char *)__mptr - YX_OFFSETOF(type,member) );})


#pragma mark -min, max
#define YX_MIN(a, b) ( ((a) < (b))? (a) : (b) )
#define YX_MAX(a, b) ( ((a) > (b))? (a) : (b) )


#pragma mark - swap
#define YX_SWAP(a, b) \
do{ \
typeof(a) _yx_lib_swap_var_c = a; \
a = b; \
b = _yx_lib_swap_var_c; \
}while(0)

#endif


















































