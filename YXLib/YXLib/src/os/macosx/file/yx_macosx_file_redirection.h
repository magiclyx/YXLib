//
//  yx_macosx_file_redirection.h
//  YXLib
//
//  Created by Yuxi Liu on 9/28/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_FILE_REDIRECTION_H_
#define _YX_MACOSX_FILE_REDIRECTION_H_

#include "yx_macosx_file_def.h"

#pragma mark - duplicate
/*duplicate file*/
#define yx_os_file_duplicate(file)                                    dup(file)
#define yx_os_file_duplicate_by_targetfile(file, target_file)         dup2(file, target_file)

#define yx_os_file_redirect_stdin(file)                       dup2(file, yx_os_file_stdin)
#define yx_os_file_redirect_stout(file)                       dup2(file, yx_os_file_stdout)
#define yx_os_file_redirect_stderr(file)                      dup2(file, yx_os_file_stderr)


#endif


