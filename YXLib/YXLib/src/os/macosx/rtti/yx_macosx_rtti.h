//
//  yx_core_rtti.h
//  YXLib
//
//  Created by LiuYuxi on 15/4/17.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_RTTI_H
#define _YX_MACOSX_RTTI_H

#include "../basictypes/yx_macosx_types.h"
#include "yx_macosx_rttidef.h"

typedef yx_value yx_rtti, *yx_rtti_ref;


/*get the rttiv value from a handle*/
#define yx_os_rtti_val(handle) \
                ( *((yx_rtti_ref)handle) )

/*test a handle is equal to a rtti value*/
#define yx_os_rtti_equal(handle, rtti) \
                ( rtti == yx_os_rtti_val(handle) )

#define yx_os_rtti_notequal(handle, rtti) \
                ( rtti != yx_os_rtti_val(handle) )


/* convenient method  if( xxx ) */
#define yx_os_rtti_if(handle, rtti) \
                if( rtti == yx_os_rtti_val(handle) )

/* convenient method  if(! xx ) */
#define yx_os_rtti_notif(handle, rtti) \
                if( rtti != yx_os_rtti_val(handle) )


/* setup rtti value */
#define yx_os_rtti_setup(handle, rtti) \
                ((*((yx_rtti_ref)handle)) = rtti)


#endif /* defined(_YX_MACOSX_RTTI_H) */

