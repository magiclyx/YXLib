//
//  yx_macosx_varargs.h
//  YXLib
//
//  Created by Yuxi Liu on 10/11/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_VARARGS_H_
#define _YX_MACOSX_VARARGS_H_

#include "yx_macosx_global.h"

/***********************************************************************************
 *  yx_va
 ***********************************************************************************
 *
 *  yx_vafun_declare
 *        define a function suffix
 *        e.g.
 *        void printf(yx_vaparam(const char* fmt)) yx_vafun_declare
 *
 *  yx_vaparam(arg0_declare)
 *        define the functions variable paramters
 *        e.g.
 *        void printf(const char* fmt, ...)
 *        is equal to
 *        void printf(yx_vaparam(const char* fmt))
 *
 * yx_valist
 *        a value type indicate the variable paramters list
 *
 * yx_vastart((valist, arg0_name)
 *        start fetch the variable paramters
 *        valist -> a yx_valist type paramters
 *        arg0_name -> the arguments name which declare in yx_vaparam(arg0_declare)
 *
 * yx_vaend(valist)
 *        valist -> a yx_valist type paramters
 *
 * yx_vaarg_first(valist, type, arg0_name)
 *        fetch the first argument
 *        valist      -> a yx_valist type paramters
 *        arg0_type   -> data type declare in yx_vaparam(arg0_declare)
 *        arg0_name   -> data name declare in yx_vaparam(arg0_declare)
 *
 */





#if YX_LIB_VARARGS

    // use <varargs.h>

    #define yx_vafun_declare                              va_dcl
    #define yx_vaparam(arg0_declare)                      va_alist

    #define yx_valist                                     va_list

    #define yx_vastart(valist, arg0_name)                 va_start(valist)
    #define yx_vaend(valist)                              va_end(valist)

    #define yx_vaarg_first(valist, type, arg0_name)       va_arg(valist, type)
    #define yx_vaarg(valist, type)                        va_arg(valist, type)


#else

    // use <stdarg.h>

    #define yx_vafun_declare
    #define yx_vaparam(arg0_declare)                      arg0_declare, ...

    #define yx_valist                                     va_list

    #define yx_vastart(valist, arg0_name)                 va_start(valist, arg0_name)
    #define yx_vaend(valist)                              va_end(valist)

    #define yx_vaarg_first(valist, arg0_type, arg0_name)  arg0_name
    #define yx_vaarg(valist, type)                        va_arg(valist, type)




#endif


#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
    #define yx_vamacro_declare  ...
    #define yx_vamacro_args     __VA_ARGS__
#elif defined __GNUC__
    #define yx_vamacro_declare  format, args...
    #define yx_vamacro_args     format, ##args

#endif


/*
 sample
 */

//void error_final(yx_vaparam(char* format)) yx_vafun_declare
//{
//    yx_valist ap;
//    char* fmt;
//
//    yx_vastart(ap, format);
//    fmt = yx_vaarg_first(ap, char*, format);
//    vprintf(fmt, ap);
//    yx_vaend(ap);
//}
//
//
//void print_num(const char* title, yx_vaparam(int num_count)) yx_vafun_declare
//{
//    yx_valist ap;
//    int num; //the name must be different from num_count
//
//    /*start fetch the argument list*/
//    yx_vastart(ap, num_count);
//
//    /*fetch the first argument*/
//    num = yx_vaarg_first(ap, int, num_count);
//
//    printf("%s(%d)", title, num);
//    for(int i=0; i<num; i++)
//    {
//        /*fetch other argument*/
//        int val = yx_vaarg(ap, int);
//        printf(" %d", val);
//    }
//    printf("\n");
//
//    /*end fetch the argument*/
//    yx_vaend(ap);
//}




#endif //_YX_MACOSX_VARARGS_H_
