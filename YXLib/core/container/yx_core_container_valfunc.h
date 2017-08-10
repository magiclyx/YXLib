//
//  yx_core_container_baseValue.h
//  YXLib
//
//  Created by LiuYuxi on 15/10/30.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_CONTAINER_VALFUNC_H_
#define _YX_CORE_CONTAINER_VALFUNC_H_

#include "../yx_core_global.h"
#include "../hash/yx_core_hash.h"



////////////////////////////////////////////////////////////////////////////////////////////////////
/*hash*/
////////////////////////////////////////////////////////////////////////////////////////////////////

/*string*/
yx_core_hash YX_CORE_HASHFUN_STRING(yx_value key);

/*char*/
yx_core_hash YX_CORE_HASHFUN_CHAR(yx_value key);
yx_core_hash YX_CORE_HASHFUN_UCHAR(yx_value key);

/*short*/
yx_core_hash YX_CORE_HASHFUN_SHORT(yx_value key);

/*int*/
yx_core_hash YX_CORE_HASHFUN_INT(yx_value key);
yx_core_hash YX_CORE_HASHFUN_UINT(yx_value key);


/*long*/
yx_core_hash YX_CORE_HASHFUN_LONG(yx_value key);
yx_core_hash YX_CORE_HASHFUN_ULONG(yx_value key);


/*float & double & real*/
yx_core_hash YX_CORE_HASHFUN_FLOAT(yx_value key);
yx_core_hash YX_CORE_HASHFUN_DOUBLE(yx_value key);
yx_core_hash YX_CORE_HASHFUN_REAL(yx_value key);



////////////////////////////////////////////////////////////////////////////////////////////////////
/*compare*/
////////////////////////////////////////////////////////////////////////////////////////////////////


/*compare*/
yx_compare_result YX_CORE_COMFUN_DEFAULT(yx_value key1, yx_value key2);

/*string*/
yx_compare_result YX_CORE_COMFUN_STRING(yx_value key1, yx_value key2);



#endif /* _YX_CORE_CONTAINER_VALFUNC_H_ */






