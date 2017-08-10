//
//  yx_macosx_file_access.h
//  YXLib
//
//  Created by Yuxi Liu on 9/28/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef __YX_MACOSX_FILE_ACCESS_H_
#define __YX_MACOSX_FILE_ACCESS_H_


#include "yx_macosx_file_def.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark - mode

/*accessMode*/
#define YX_OS_FILE_ACCESSMOD_R   O_RDONLY        //open for reading only
#define YX_OS_FILE_ACCESSMOD_W   O_WRONLY        //open for writing only
#define YX_OS_FILE_ACCESSMOD_RW  O_RDWR          //open for reading and writing



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark - test

//test the access by file
#define yx_os_file_isreadonly(file)   (O_RDONLY == ((fcntl(file, F_GETFL, 0)) & O_ACCMODE))
#define yx_os_file_iswriteonly(file)  (O_WRONLY == ((fcntl(file, F_GETFL, 0)) & O_ACCMODE))
#define yx_os_file_isreadwrite(file)  (O_RDWR == ((fcntl(file, F_GETFL, 0)) & O_ACCMODE))

//test the access by fileinfo
yx_bool yx_os_fileinfo_isreadable(yx_os_fileinfo info);
yx_bool yx_os_fileinfo_iswritable(yx_os_fileinfo info);
yx_bool yx_os_fileinfo_isexeable(yx_os_fileinfo info);

//test the access by path
#define yx_os_path_realusr_isReadable(path)  access(path, R_OK)
#define yx_os_path_realusr_isWritable(path)  access(path, W_OK)
#define yx_os_path_realusr_isExeable(path)   access(path, X_OK)

#define yx_os_path_fileExist(path)           access(path, F_OK)

#endif /* defined(__YXLib__yx_macosx_file_access__) */

















