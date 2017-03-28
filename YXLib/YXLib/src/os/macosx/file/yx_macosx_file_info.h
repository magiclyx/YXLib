//
//  yx_macosx_file_attribute.h
//  YXLib
//
//  Created by Yuxi Liu on 9/28/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_FILE_INFO_H_
#define _YX_MACOSX_FILE_INFO_H_

#include "yx_macosx_file_def.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark - get info
/*get info*/
#define yx_os_file_info(file, info)             fstat(file, &info)
#define yx_os_path_info(path, info)             lstat(path, &info)  //trade the symbol link as a link file
#define yx_os_path_info_jmp(path, info)         stat(path, &info)   //jump the real file when using on a link file


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark - file type

typedef enum{
    yx_os_filetype_regular = S_IFREG,
    yx_os_filetype_pipe = S_IFIFO,
    yx_os_filetype_characterSpecial = S_IFCHR,
    yx_os_filetype_directory = S_IFDIR,
    yx_os_filetype_blockSpecial = S_IFBLK,
    yx_os_filetype_symbolLink = S_IFLNK,
    yx_os_filetype_socket = S_IFSOCK,
    yx_os_filetype_other = S_IFWHT,
}yx_os_filetype;


/*tfile type*/
#define yx_os_filetype_id(info)                 (info.st_mode)
#define yx_os_fileinfo_type_isnormal(info)      S_ISREG(info.st_mode)      //regular file
#define yx_os_fileinfo_type_isdir(info)         S_ISDIR(info.st_mode)      //directory
#define yx_os_fileinfo_type_ischar(info)        S_ISCHR(info.st_mode)      //character special
#define yx_os_fileinfo_type_isblk(info)         S_ISBLK(info.st_mode)      //block special
#define yx_os_fileinfo_type_isfifo(info)        S_ISFIFO(info.st_mode)     //piple or fifo file
#define yx_os_fileinfo_type_islnk(info)         S_ISLNK(info.st_mode)      //symbolic link
#define yx_os_fileinfo_type_issock(info)        S_ISSOCK(info.st_mode)     //socket

//ipc about
#define yx_os_fileinfo_type_ismsgqueue(info)    S_TYPEISMQ(info.st_mode)   //message queue
#define yx_os_fileinfo_type_issemaphore(info)   S_TYPEISSEM(info.st_mode)  //semaphore
#define yx_os_fileinfo_type_isshared(info)      S_TYPEISSHM(info.st_mode)  //shared object


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark - file size
/*get file size*/
#define yx_os_fileinfo_size(info)               (info.st_size)
#define yx_os_fileinfo_blocksnum(info)          (info.st_blocks)
#define yx_os_fileinfo_blocksize(info)          ((info.st_blocks)*512) //i'm not sure the block unit is 512
#define yx_os_fileinfo_fitbuffsize(info)        (info.st_blksize) /*file buffer size*/


/*set file size*/
//the two functions will truncate the file.
#define yx_os_file_setSize(file, size)          ftruncate(file, size)
#define yx_os_path_setSize(path, size)          truncate(path, size)



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark - time
/*time about*/
#define yx_os_fileinfo_readtime(info)           (info.st_atime)
#define yx_os_fileinfo_accesstime(info)         (info.st_mtime)
#define yx_os_fileinfo_modifytime(info)         (info.st_ctime)



#endif










