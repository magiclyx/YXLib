//
//  yx_core_range.h
//  YXLib
//
//  Created by Yuxi Liu on 2/11/15.
//  Copyright (c) 2015 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_RANGE_H_
#define _YX_CORE_RANGE_H_

#include "../yx_core_global.h"

typedef struct yx_irange
{
    yx_int location;
    yx_uint length;
}yx_irange;



#define YXNotFound YX_MAX_INT


//#define YXRange_make(location, length) {location, length}
yx_forceinline yx_irange YXRange_make(yx_int location, yx_uint length){
    yx_irange r;
    r.location = location;
    r.length = length;
    
    return r;
}

#define YXRange_MaxValue(range) (range.location+(yx_int)range.length - 1)
#define YXRange_MinValue(range) (range.location)

#define YXRange_isValid(range) ((YXNotFound != range.location)? yx_true : yx_false)
#define YXRange_isEqual(range1, range2) (((range1.location == range2.location)  &&  (range1.length == range2.length)) ? yx_true : yx_false)
#define YXRange_isIncludeLocation(range, location) (((location < YXRange_MinValue(range)) || (location > YXRange_MaxValue(range))) ? yx_true : yx_false)


//FOUNDATION_EXPORT NSRange NSUnionRange(NSRange range1, NSRange range2);
//FOUNDATION_EXPORT NSRange NSIntersectionRange(NSRange range1, NSRange range2);
//FOUNDATION_EXPORT NSString *NSStringFromRange(NSRange range);
//FOUNDATION_EXPORT NSRange NSRangeFromString(NSString *aString);

#endif /* defined(_YX_CORE_RANGE_H_) */


