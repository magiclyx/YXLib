//
//  yx_macosx_headers
//  YXLib
//
//  Created by Yuxi Liu on 9/4/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_HEADERS_H_
#define _YX_MACOSX_HEADERS_H_


#include "yx_macosx_global.h"

#ifndef YX_PLATFORM_MACOSX
#error "Incorrect platform configuration"
#endif

/*types*/
#include "basictypes/yx_macosx_basictypes.h"
#include "basictypes/yx_macosx_types.h"

/*platform*/
#include "platform/yx_macosx_platform.h"

/*system*/
#include "system/yx_macosx_system.h"

/*environment*/
#include "environment/yx_macosx_environment.h"

/*err*/
#include "err/yx_macosx_err.h"

/*rtti*/
#include "rtti/yx_macosx_rtti.h"
#include "rtti/yx_macosx_rttidef.h"


/*alloc*/
#include "allocator/yx_macosx_mem.h"
#include "allocator/yx_macosx_allocator.h"

/*string*/
#include "string/yx_macosx_char.h"
#include "string/yx_macosx_string.h"
#include "string/yx_macosx_string_ext.h"
#include "string/yx_macosx_string_base64.h"
#include "string/yx_macosx_string_http.h"

/*memory*/ //deprecated
//#include "mem/yx_macosx_memory.h"
#include "mem/yx_macosx_memory.h"
#include "mem/yx_macosx_shmem.h"

/*variable arguments*/
#include "yx_macosx_varargs.h"


/*time*/
#include "time/yx_macosx_time.h"
#include "time/yx_macosx_critime.h"

/*file*/
#include "file/yx_macosx_file_def.h"           //define
#include "file/yx_macosx_file_create.h"        //open and close file
#include "file/yx_macosx_file_readwrite.h"     //read, write, seek file
#include "file/yx_macosx_file_redirection.h"   //redirection a file
#include "file/yx_macosx_file_info.h"          //file information
#include "file/yx_macosx_file_access.h"        //file access
#include "file/yx_macosx_file_link.h"          //file link
#include "file/yx_macosx_file_authorization.h" //file authorization, owner
#include "file/yx_macosx_file_path.h"          //file path operation
#include "file/yx_macosx_file_dir.h"           //dir operation


/*atomic*/
#include "atomic/yx_macosx_atomic.h"

/*thread*/
#include "thread/yx_macosx_thread.h"
#include "thread/yx_macosx_thread_lock.h"

/*event*/
#include "event/yx_macosx_event.h"

/*process*/
#include "process/yx_macosx_process.h"
#include "process/yx_macosx_process_status.h"
#include "process/yx_macosx_process_info.h"

/*pipe*/
#include "pipe/yx_macosx_pipe.h"


/*debug*/
#include "debug/yx_macosx_debug.h"


#endif

