//
//  yx_unittest_rttidef.h
//  YXLib
//
//  Created by LiuYuxi on 15/4/17.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_UNITTEST_RTTIDEF_H_
#define _YX_UNITTEST_RTTIDEF_H_ 

#define yx_rtti_unittest_offset(offset)   (yx_rtti_core_base + offset)


////////////////////////////////////////////////////
/*reserved 0 - 50 offset value from core rtti base*/
////////////////////////////////////////////////////

/*special (offset 0 - 4)*/

/*mem pool (offset 5 - 7)*/
#define yx_rtti_allocator_unittest  yx_rtti_unittest_offset(5)

#endif
