//
//  yx_macosx_global.h
//  YXLib
//
//  Created by Yuxi Liu on 9/4/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_GLOBAL_H_
#define _YX_MACOSX_GLOBAL_H_ 

#include "../yx_os_global.h"

/*stdC*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <dirent.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include <stdatomic.h>

/*posix*/
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <execinfo.h>
#include <sys/event.h>
#include <sys/mman.h>
#include <sys/sysctl.h>
#include <sys/mount.h>
#include <sys/socket.h>
#include <signal.h>


/*MACOSX*/
#include <TargetConditionals.h> //platform info
#include <mach/mach.h>
#include <mach/mach_time.h>
//#include <libkern/OSAtomic.h>   //atomic deprecated, 使用 stdatomic.h


/*cpu crc*/
#ifdef __SSE4_2__
#define YX_OS_CPUCRC_SUPPORT 1
    #include <citycrc.h>
    #include <nmmintrin.h>
#endif //__SSE4_2__

#endif











