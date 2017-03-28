//
//  yx_macosx_file_create.h
//  YXLib
//
//  Created by Yuxi Liu on 9/28/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_FILE_CREATE_H_
#define _YX_MACOSX_FILE_CREATE_H_

#include "../string/yx_macosx_string.h"
#include "yx_macosx_file_link.h"
#include "yx_macosx_file_authorization.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark - create mode
/*createMode*/
#define YX_OS_FILE_CREATEMOD_OPEN        0              //open the file. failed if file not exist
#define YX_OS_FILE_CREATEMOD_CREATE      O_CREAT       //create file if it does not exist
#define YX_OS_FILE_CREATEMOD_NOTEXIST    O_EXCL         //error if the file is already exist
#define YX_OS_FILE_CREATEMOD_RESET       O_TRUNC        //truncate size to 0(file exist and open flag is R/RW)

#define YX_OS_FILE_CREATEMOD_APPENDING   O_APPEND       //append on each write
#define YX_OS_FILE_CREATEMOD_NONBLOCK    O_NONBLOCK     //do not block on open or for data to become available(do not use O_NDELAY flag)



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark - test create mode
//test the create mode
#define yx_os_file_isappend(file)     ((fcntl(file, F_GETFL, 0)) & O_APPEND)
#define yx_os_file_isnonblock(file)   ((fcntl(file, F_GETFL, 0)) & O_NONBLOCK)



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark - modify create mode
//set or remove the create mode
static yx_inline
int yx_os_file_set_create_mode(yx_os_file file, yx_int mode){
    int val;
    
    if((val = fcntl(file, F_GETFL, 0)) < 0)
        return val;
    
    val |= mode;
    
    return fcntl(file, F_SETFL, val);
}

static yx_inline
int yx_os_file_remove_create_mode(yx_os_file file, yx_int mode){
    int val;
    
    if((val = fcntl(file, F_GETFL, 0)) < 0)
        return val;
    
    val &= ~mode;
    
    return fcntl(file, F_SETFL, val);
}


#define yx_os_file_set_appendmode(file)     yx_os_file_set_create_mode(file, O_APPEND)
#define yx_os_file_remove_appendmode(file)   yx_os_file_remove_create_mode(file, O_APPEND)

#define yx_os_file_set_blockmode(fle)       yx_os_file_set_create_mode(file, O_NONBLOCK)
#define yx_os_file_remove_blockmode(fle)       yx_os_file_remove_create_mode(file, O_NONBLOCK)



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark - open,create,close,delete
/*open & create file*/

/*
 //:! the O_CLOEXEC flag always set here
 //:! if authorization not be set in yx_os_fileopen.
 using the default authorization. (using yx_os_filedefault_authorization set the default authorization)
 */

/*
 //:! the following create mode can use for flag param
 
YX_OS_FILE_CREATEMOD_NOTEXIST  //error if the file is already exist
YX_OS_FILE_CREATEMOD_RESET     //truncate size to 0(file exist and open flag is R/RW)
YX_OS_FILE_CREATEMOD_APPENDING //append on each write
YX_OS_FILE_CREATEMOD_NONBLOCK  //do not block on open or for data to become available(do
 */

#define yx_os_file_openForRead(path, flag)      open(path, YX_OS_FILE_ACCESSMOD_R | YX_OS_FILE_CREATEMOD_OPEN | flag | O_CLOEXEC)
#define yx_os_file_openForWrite(path, flag)     open(path, YX_OS_FILE_ACCESSMOD_W | YX_OS_FILE_CREATEMOD_OPEN | flag | O_CLOEXEC)
#define yx_os_file_openForUpdate(path, flag)    open(path, YX_OS_FILE_ACCESSMOD_RW | YX_OS_FILE_CREATEMOD_OPEN | flag |  O_CLOEXEC)

#define yx_os_file_createForRead(path, flag)    open(path, YX_OS_FILE_ACCESSMOD_R | YX_OS_FILE_CREATEMOD_CREATE | flag | O_CLOEXEC, YX_OS_FILE_AU_DEFAULT)
#define yx_os_file_createForWrite(path, flag)   open(path, YX_OS_FILE_ACCESSMOD_W | YX_OS_FILE_CREATEMOD_CREATE | flag | O_CLOEXEC, YX_OS_FILE_AU_DEFAULT)
#define yx_os_file_createForUpdate(path, flag)  open(path, YX_OS_FILE_ACCESSMOD_RW | YX_OS_FILE_CREATEMOD_CREATE | flag | O_CLOEXEC, YX_OS_FILE_AU_DEFAULT)

static yx_inline
yx_os_file yx_os_file_create_temp(yx_char * path, yx_strSize buff_len){
    
#if YX_LIB_TMPFILE_USE_SUFFIX
    yx_os_strncpy(path, YX_CONTACT3(__YX_LIB_TMPFILE_PREFIX, _XXXXXX, __YX_LIB_TMPFILE_SUFFIX), buff_len);
    int file = mkstemps(path, strlen(YX_LIB_TMPFILE_SUFFIX));
#else
    yx_os_strncpy(path, YX_STRINGIZE(YX_CONTACT(__YX_LIB_TMPFILE_PREFIX, _XXXXXX)), buff_len);
    int file = mkstemp(path);
#endif
    
    yx_os_file_unlink(path);
    
    return file;
}


/*the origional file open*/
#define yx_os_file_open(path, accessMode, createMode, authorizationMode) \
open(path, accessMode|createMode|O_CLOEXEC, authorizationMode)

#define yx_os_file_close(file)  close(file)
#define yx_os_path_delete(path) unlink(path)




#pragma mark - file handle convert
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#define yx_os_file_to_CFile(file, mode)    fdopen(file, mode)    //file descriptor -> File*
#define yx_os_create_CFile(file, mode)  fdopen(file, mode)  //file descriptior -> File* (need to fclose())
#define yx_os_file_from_CFile(cfile)  fileno(cfile); //File* - > file descriptor


//#define yx_os_file_open(path, accessMode) \
//open(path, accessMode|O_CLOEXEC)
//
//#define yx_os_file_open(path, accessMode, createMode) \
//open(path, accessMode|createMode|O_CLOEXEC)


#endif


