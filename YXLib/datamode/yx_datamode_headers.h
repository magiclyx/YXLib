//
//  yx_auto_datamode.h
//  YXLib
//
//  Created by Yuxi Liu on 8/16/13.
//  Copyright (c) 2013 Yuxi Liu. All rights reserved.
//

#ifndef _YX_DATAMODE_HEADERS_
#define _YX_DATAMODE_HEADERS_

#include "yx_datamode_global.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/*data mode*/
////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 TYPE             LP32    ILP32    LP64    ILP64    LLP64
 ----------------------------------------------------------
 CHAR             8       8        8       8        8
 ----------------------------------------------------------
 SHORT            16      16       16      16       16
 ----------------------------------------------------------
 INT              16      32       32      64       32
 ----------------------------------------------------------
 LONG             32      32       64      64       32
 ----------------------------------------------------------
 LONG LONG        64      64       64      64       64
 ----------------------------------------------------------
 POINTER          32      32       64      64       64
 ---------------------------------------------------------- 
 DOUBLE           64      64       64      64       64
 ---------------------------------------------------------- 
 FLOAT            32      32       32      32       32  
 */





#pragma mark - data mode
////////////////////////////////////////////////////////////////////////////////////////////////////
/*define the data size*/
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef YX_LP32

#define YX_CHAR_SIZE 1
#define YX_SHORT_SIZE 2
#define YX_INT_SIZE  2
#define YX_LONG_SIZE 4
#define YX_LONGLONG_SIZE 8
#define YX_PTR_SIZE  4
#define YX_DOUBLE_SIZE 8
#define YX_FLOAT_SIZE 4
#define YX_VALUE_SIZE 4

#define YX_ALIGNMENT 4  //alignment

#define YX_DATAMODE_32 1

#elif YX_ILP32

#define YX_CHAR_SIZE 1
#define YX_SHORT_SIZE 2
#define YX_INT_SIZE  4
#define YX_LONG_SIZE 4
#define YX_LONGLONG_SIZE 8
#define YX_PTR_SIZE  4
#define YX_DOUBLE_SIZE 8
#define YX_FLOAT_SIZE 4
#define YX_VALUE_SIZE 4

#define YX_ALIGNMENT 4 //alignment

#define YX_DATAMODE_32 1

#elif YX_LP64

#define YX_CHAR_SIZE 1
#define YX_SHORT_SIZE 2
#define YX_INT_SIZE  4
#define YX_LONG_SIZE 8
#define YX_LONGLONG_SIZE 8
#define YX_PTR_SIZE  8
#define YX_DOUBLE_SIZE 8
#define YX_FLOAT_SIZE 4
#define YX_VALUE_SIZE 8

#define YX_ALIGNMENT 8 //alignment

#define YX_DATAMODE_64 1


#elif YX_ILP64

#define YX_CHAR_SIZE 1
#define YX_SHORT_SIZE 2
#define YX_INT_SIZE  8
#define YX_LONG_SIZE 8
#define YX_LONGLONG_SIZE 8
#define YX_PTR_SIZE  8
#define YX_DOUBLE_SIZE 8
#define YX_FLOAT_SIZE 4
#define YX_VALUE_SIZE 8

#define YX_ALIGNMENT 8 //alignment

#define YX_DATAMODE_64 1


#elif YX_LLP64

#define YX_CHAR_SIZE 1
#define YX_SHORT_SIZE 2
#define YX_INT_SIZE  4
#define YX_LONG_SIZE 4
#define YX_LONGLONG_SIZE 8
#define YX_PTR_SIZE  8
#define YX_DOUBLE_SIZE 8
#define YX_FLOAT_SIZE 4
#define YX_VALUE_SIZE 8

#define YX_DATAMODE_64 1

#define YX_ALIGNMENT 8  //alignment

#else

#error "unknown data mode"

#endif //endof data mode







#pragma mark - verify data mode
////////////////////////////////////////////////////////////////////////////////////////////////////
/*verify the data mode*/
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef YX_DATAMODE_VERIFY

//here, define a local macro to verify the datatype
#define __YX_DATAMODE_VERIFHY_JOIN_(X, Y) __YX_DATAMODE_VERIFHY_JOIN_AGIN(X, Y)
#define __YX_DATAMODE_VERIFHY_JOIN_AGIN(X,Y) X##Y

#define __YX_DATAMODE_VERIFY_(datatype,size,msg) typedef char __YX_DATAMODE_VERIFHY_JOIN_(data_mode_err_, msg) [(sizeof(datatype)==size)? 1:-1]
#define __YX_DATAMODE_CONDITION_VERIFY_(condition, msg) typedef char __YX_DATAMODE_VERIFHY_JOIN_(data_mode_err_, msg) [(condition)? 1:-1]


__YX_DATAMODE_VERIFY_(char, YX_CHAR_SIZE, CHAR_size_not_match_data_mode_defined_in_yx_auto_headers);
__YX_DATAMODE_VERIFY_(short, YX_SHORT_SIZE, SHORT_size_not_match_data_mode_defined_in_yx_auto_headers);
__YX_DATAMODE_VERIFY_(int, YX_INT_SIZE, INT_size_not_match_data_mode_defined_in_yx_auto_headers);
//__YX_DATAMODE_VERIFY_(long, YX_LONG_SIZE, LONG_size_not_match_data_mode_defined_in_yx_auto_headers);
__YX_DATAMODE_VERIFY_(long long, YX_LONGLONG_SIZE, LONGLONG_size_not_match_data_mode_defined_in_yx_auto_headers);
__YX_DATAMODE_VERIFY_(void*, YX_PTR_SIZE, POINTER_size_not_match_data_mode_defined_in_yx_auto_headers);
__YX_DATAMODE_VERIFY_(double, YX_DOUBLE_SIZE, DOUBLE_size_not_match_data_mode_defined_in_yx_auto_headers);
__YX_DATAMODE_VERIFY_(float, YX_FLOAT_SIZE, FLOAT_size_not_match_data_mode_defined_in_yx_auto_headers);

__YX_DATAMODE_CONDITION_VERIFY_(YX_VALUE_SIZE >= YX_PTR_SIZE, YX_VALUE_SIZE_must_larger_or_equal_to_ptr_size);

__YX_DATAMODE_CONDITION_VERIFY_(YX_LONG_SIZE == YX_ALIGNMENT, YX_ALIGNMENT_may_have_error_value);

#endif




#endif











