//
//  yx_core_auto_interface.h
//  YXLib
//
//  Created by LiuYuxi on 15/10/9.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_AUTO_INTERFACE_H_
#define _YX_CORE_AUTO_INTERFACE_H_

#include "../yx_core_global.h"

/*C 语言不要支持 auto release pool*/


/*这东西，每种类型生成一个就行*/
typedef struct yx_core_auto_info{
    yx_rtti rtti;
    yx_rtti auto_rtti;
    
    yx_bool (*_autotype_release)(void* ins_ref);
    yx_bool (*_autotype_isEqual)(void* handle1, void* handle2);
}yx_core_auto_info, *yx_core_auto_info_ref;


typedef struct yx_core_auto_struct{
    yx_int retain_count;
    yx_core_auto_info_ref auto_info;
}yx_core_auto_struct, *yx_core_auto_struct_ref;


yx_compare_result yx_core_auto_type_isEqual(yx_core_auto_struct_ref autostruct1, yx_core_auto_struct_ref autostruct2);



#define YX_AUTOTYPE_STRUCT_DECLARE  const yx_core_auto_struct __self__autostruct__


/*retain*/
#define yx_auto_retain(instance_ref) \
    ((((yx_core_auto_struct_ref)instance_ref)->retain_count)++)

/*release*/
#define yx_auto_release(instance_ref_pt) \
do{ \
    yx_core_auto_struct_ref autostruct = ((yx_core_auto_struct_ref)(*instance_ref_pt)); \
    if(--(autostruct->retain_count) <= 0) \
    { \
        autostruct->auto_info->_autotype_release(*instance_ref_pt); \
        *instance_ref_pt = NULL; \
    } \
}while(0)

/*isEqual*/
#define yx_auto_isEqual(instance_ref1, instane_ref2) \
    yx_core_auto_type_isEqual((yx_core_auto_struct_ref)instance_ref1, (yx_core_auto_struct_ref)instane_ref2)




#endif /* _YX_CORE_AUTO_INTERFACE_H_ */








