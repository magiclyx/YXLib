//
//  yx_macosx_file_link.h
//  YXLib
//
//  Created by Yuxi Liu on 9/30/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_FILE_LINK_H_
#define _YX_MACOSX_FILE_LINK_H_

#define yx_os_file_link(path, linkPath)           link(path, linkPath)
#define yx_os_file_symbol_link(path, linkPath)    link(path, linkPath)
#define yx_os_file_unlink(path)                   unlink(path)

#endif
