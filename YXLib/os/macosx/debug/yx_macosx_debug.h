//
//  yx_macosx_debug.h
//  YXLib
//
//  Created by LiuYuxi on 15/10/5.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#ifndef yx_macosx_debug_h
#define yx_macosx_debug_h



#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   if defined __GNUC__ && __GNUC__ >= 2
#       define __YXFUNC__ __FUNCTION__
#   else
#      define __YXFUNC__ "<unknown>"
#   endif
#else
#   define   __YXFUNC__ __FUNCTION__
#endif


#define __YXFILE__  __FILE__

#define __YXLINE__  __LINE__






#endif /* yx_macosx_debug_h */
