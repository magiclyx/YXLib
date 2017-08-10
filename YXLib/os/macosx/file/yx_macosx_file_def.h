//
//  yx_macosx_file_def.h
//  YXLib
//
//  Created by Yuxi Liu on 9/25/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_FILE_DEF_H_
#define _YX_MACOSX_FILE_DEF_H_

#include "../basictypes/yx_macosx_types.h"

#pragma mark - file
typedef int yx_os_file;
typedef struct stat yx_os_fileinfo;

typedef struct {
    DIR* dir;
    struct dirent* de;
}yx_os_dir;

typedef struct dirent* yx_os_dirIterator;





#define yx_os_file_invalidate  (-1)


#pragma mark - std file
/*stdin, stdout, stderr*/
#define yx_os_file_stdin   ((STDIN_FILENO))
#define yx_os_file_stdout  ((STDOUT_FILENO))
#define yx_os_file_stderr  ((STDERR_FILENO))







#endif





