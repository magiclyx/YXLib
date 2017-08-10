//
//  yx_clang_def.h
//  YXLib
//
//  Created by Yuxi Liu on 9/5/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CLANG_DEF_H_
#define _YX_CLANG_DEF_H_


#pragma mark - clang def
////////////////////////////////////////////////////////////////////////////////////////////////////
/*def the macro*/
//not consider using clang in windows with minGW
////////////////////////////////////////////////////////////////////////////////////////////////////

/*compiler*/
#define YX_COMPILER_NAME "clang"
#define YX_COMPILER_DESCRIPTION YX_COMPILER_NAME " " __clang_version__

/*attribute*/
#define yx_attribute(x)       __attribute__((__##x##__))
#define yx_attribute2(x,y)    __attribute__((__##x##__(y)))
#define yx_attribute3(x,y,z)  __attribute__((__##x##__(y,z)))

/*keywords*/
#define yx_deprecated   yx_attribute(deprecated)
#define yx_inline       inline
#define yx_forceinline  static yx_inline yx_attribute(always_inline)
#define yx_noinline     yx_attribute(noinline)
#define yx_purecall     yx_attribute(pure)
#define yx_constcall    yx_attribute(const)
#define yx_align(x)     yx_attribute2(aligned,x)
#define yx_restrict     restrict
#define yx_likely(x)    __builtin_expect(!!(x), 1)
#define yx_unlikely(x)  __builtin_expect(!!(x), 0)


#define yx_printf_fmtcheck(fmtarg, firstvararg)  __attribute__((__format__ (__printf__, fmtarg, firstvararg)))
#define yx_scanf_fmtcheck(fmtarg, firstvararg) __attribute__((__format__ (__scanf__, fmtarg, firstvararg)))

#define yx_unused(param) ((void)param)

//void unavailable(void) __attribute__((availability(macosx,unavailable)));
//void deprecated(void) __attribute__((availability(macosx,deprecated=10.6)));
//void obsoleted(void) __attribute__((availability(macosx,obsoleted=10.6)));
//void tooyoung(void) __attribute__((availability(macosx,introduced=10.8)));


#if YX_PLATFORM_WINDOWS
#    define threadlocal
#else
#    define threadlocal __thread
#endif


/*assembly snippet*/

#define yx_asm_break __asm("int3")

#define yx_asm_cycle(_u64ValPt) do{yx_uint32 lo,hi;__asm__ __volatile__("rdtsc":"=a"(lo),"=d"(hi));(*_u64ValPt)=(yx_uint64)hi<<32|lo;}while(0)



#pragma mark - verify the compiler
////////////////////////////////////////////////////////////////////////////////////////////////////
/*verify the compiler*/
////////////////////////////////////////////////////////////////////////////////////////////////////
#if YX_COMPILER_VERIFY  //compiler verify macro is def

#  if ! defined( __clang__ )
#      error "the compiler is not clang"
#  endif

#endif

#endif

