//
//  yx_macosx_err.h
//  YXLib
//
//  Created by Yuxi Liu on 9/28/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_ERR_H_
#define _YX_MACOSX_ERR_H_

#include "../yx_macosx_global.h"

#define yx_os_err_clearlasterr()        (errno = 0)
#define yx_os_err_setlasterr(err_num)   (srrno = err_num)
#define yx_os_err_getlasterr()   errno


#define YX_ERROR_NOTEXIST EEXIST
#define YX_ERROR_INTER

#endif
