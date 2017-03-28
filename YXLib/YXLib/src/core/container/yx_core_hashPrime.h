//
//  yx_core_hashPrime.h
//  YXLib
//
//  Created by LiuYuxi on 15/7/21.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_HASHPRIME__
#define _YX_CORE_HASHPRIME__

#include "../yx_core_global.h"


/*!
 Under the suggested value to calculate the size of the hash value
 
 if the suggested value larger than the 65534. This function will run for a long time.
 */

yx_size yx_core_hash_primeSize(yx_size size);



#endif /* defined(_YX_CORE_HASHPRIME__) */
