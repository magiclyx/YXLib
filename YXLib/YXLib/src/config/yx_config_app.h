//
//  yx_config_app.h
//  YXLib
//
//  Created by Yuxi Liu on 9/29/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CONFIG_APP_H_
#define _YX_CONFIG_APP_H_


#include "yx_config_global.h"


#pragma mark - app info
////////////////////////////////////////////////////////////////////////////////////////////////////
/* set the app information */
////////////////////////////////////////////////////////////////////////////////////////////////////

#define YX_LIB_APP_NAME "yxlib"
#define YX_LIB_APP_NAME_INITIALS "yxlib"




#pragma mark - tmp file
////////////////////////////////////////////////////////////////////////////////////////////////////
/*tmp file*/
////////////////////////////////////////////////////////////////////////////////////////////////////


/*whether use the suffix in tmp file*/
#define YX_LIB_TMPFILE_USE_SUFFIX 0

/*prefix in tmp file*/
#define __YX_LIB_TMPFILE_PREFIX       yx_
#define YX_LIB_TMPFILE_PREFIX         YX_STRINGIZE(__YX_LIB_TMPFILE_PREFIX)

/*suffix in tmp file*/
#if YX_LIB_TMPFILE_USE_SUFFIX
  #define __YX_LIB_TMPFILE_SUFFIX     .tmp
  #define YX_LIB_TMPFILE_SUFFIX       YX_STRINGIZE(__YX_LIB_TMPFILE_SUFFIX)
#endif

#endif
