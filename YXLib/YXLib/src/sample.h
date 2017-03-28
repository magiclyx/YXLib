//
//  sample.h
//  YXLib
//
//  Created by Yuxi Liu on 11/8/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _SAMPLE_H_
#define _SAMPLE_H_

#include "yx_core_global.h"



/**
 *  两种封装方式
 *  这个例子更适合方式一，所以方式二看起来会很奇怪
 *  实际使用中，要根据情况来定
 *  方式一对于复杂的操作，更适合(封装了上下文句柄)
 *  方式二会使代码效率很高，更适合我们服务端core层的需求(在实际是用中，方式二会有大半的函数用宏实现，还有很多强制内联)
 */


/**
 *  注意两种实现的区别
 *  1. 方式一 声明了 yx_sample句柄
 *  2. 两种方式对于上下文结构体的命名方式有区别
 */


/**
 *  其他注意
 *  ref 和 ptr 后缀都用来表示指针。
 *      ref 常用语对类型声明，表明这是一个引用(指针)
 *      ptr 常用语函数参数，表明这是一个指针类型的参数
 */


/**
 *  例子中是用2中方式的封装， 是用  “use_type_1” 来切换实例
 */
#define use_type_1 1
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////



#if use_type_1

/*方式一*/

typedef yx_handle yx_sample;

yx_sample yx_sample_create(yx_allocator allocator);
void yx_sample_destroy(yx_sample* sample_ptr);



////////////////////////////////////////////////////////////////////////////////////////////////////
#else
////////////////////////////////////////////////////////////////////////////////////////////////////


/*方式二*/

typedef struct _yx_sample{
    yx_handle allocator;
    
    //others ...
}yx_sample, *yx_smple_ref;


yx_smple_ref yx_sample_create(yx_allocator allocator);
void yx_sample_destroy(yx_smple_ref* sampleRef_ptr);


#endif //use_type_1

#endif







