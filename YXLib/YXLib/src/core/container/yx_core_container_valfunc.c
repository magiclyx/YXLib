//
//  yx_core_container_baseValue.c
//  YXLib
//
//  Created by LiuYuxi on 15/10/30.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_container_valfunc.h"



////////////////////////////////////////////////////////////////////////////////////////////////////
/*hash*/
////////////////////////////////////////////////////////////////////////////////////////////////////


yx_core_hash YX_CORE_HASHFUN_STRING(yx_value key)
{
    yx_char* str = (yx_char*)key;
    
    return yx_core_hash_string(str, 2);
}

yx_core_hash YX_CORE_HASHFUN_CHAR(yx_value key)
{
    return yx_core_hash_char(key);
}
yx_core_hash YX_CORE_HASHFUN_UCHAR(yx_value key)
{
    return yx_core_hash_uchar(key);
}


yx_core_hash YX_CORE_HASHFUN_SHORT(yx_value key)
{
    return yx_core_hash_short(key);
}


yx_core_hash YX_CORE_HASHFUN_INT(yx_value key)
{
    return yx_core_hash_int(key);
}
yx_core_hash YX_CORE_HASHFUN_UINT(yx_value key)
{
    return yx_core_hash_uint(key);
}


yx_core_hash YX_CORE_HASHFUN_LONG(yx_value key)
{
    return yx_core_hash_long(key);
}
yx_core_hash YX_CORE_HASHFUN_ULONG(yx_value key)
{
    return yx_core_hash_ulong(key);
}


yx_core_hash YX_CORE_HASHFUN_FLOAT(yx_value key)
{
    return yx_core_hash_float(key);
}
yx_core_hash YX_CORE_HASHFUN_DOUBLE(yx_value key)
{
    return yx_core_hash_double(key);
}
yx_core_hash YX_CORE_HASHFUN_REAL(yx_value key)
{
    return yx_core_hash_real(key);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/*compare*/
////////////////////////////////////////////////////////////////////////////////////////////////////


yx_compare_result YX_CORE_COMFUN_DEFAULT(yx_value key1, yx_value key2)
{
    if (key1 > key2)
    {
        return yx_compare_larger;
    }
    else if(key1 < key2)
    {
        return yx_compare_smaller;
    }
    else
    {
        return yx_compare_equal;
    }
}


/*string*/
yx_compare_result YX_CORE_COMFUN_STRING(yx_value key1, yx_value key2)
{
    yx_int res = yx_os_strncmp(key1, key2, yx_os_strlen(key1));
    
    if (res > 0)
    {
        return yx_compare_larger;
    }
    else if(res < 0)
    {
        return yx_compare_smaller;
    }
    else
    {
        return yx_compare_equal;
    }
}

