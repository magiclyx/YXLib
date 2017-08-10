//
//  yx_core_auto_interface.c
//  YXLib
//
//  Created by LiuYuxi on 15/10/9.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_auto_interface.h"


yx_compare_result yx_core_auto_type_isEqual(yx_core_auto_struct_ref autostruct1, yx_core_auto_struct_ref autostruct2)
{
    yx_bool isEqual = yx_false;
    
    
    yx_core_auto_info_ref autoinfo1 = autostruct1->auto_info;
    yx_core_auto_info_ref autoinfo2 = autostruct2->auto_info;
    
    do{
        if (autoinfo1->rtti  !=  autoinfo2->rtti)
            break;
        
        if (autoinfo1->auto_rtti != autoinfo2->auto_rtti)
            break;
        
        isEqual = autoinfo1->_autotype_isEqual(autostruct1, autostruct2);
        
    }while (0);
    
    return isEqual;
}

