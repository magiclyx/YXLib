//
//  yx_macosx_file_path.h
//  YXLib
//
//  Created by Yuxi Liu on 9/30/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_FILE_PATH_H_
#define _YX_MACOSX_FILE_PATH_H_ 

#define yx_os_path_name(path)   rename(path)
#define yx_os_real_path(path, realPath) realpath(path, realPath)

#endif
