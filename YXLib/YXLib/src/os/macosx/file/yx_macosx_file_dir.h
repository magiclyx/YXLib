//
//  yx_macosx_file_dir.h
//  YXLib
//
//  Created by Yuxi Liu on 9/30/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_FILE_DIR_H_
#define _YX_MACOSX_FILE_DIR_H_

#include "yx_macosx_file_def.h"
#include "../yx_macosx_global.h"
#include "../basictypes/yx_macosx_types.h"


#pragma mark - dir check

static yx_inline
yx_result yx_os_dir_isdir(yx_char* path){
    DIR* dir = opendir(path);
    if (NULL != dir) {
        closedir(dir);
        return yx_result_success;
    }
    
    return yx_result_failed;
}

#pragma mark - dir crate & remvoe
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
/*dir create & remove*/

#define yx_os_dir_create(name, authorization)  mkdir(name, authorization)
#define yx_os_dir_remove(name)  rmdir(name)


#pragma mark - dir operation
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
/*dir operation*/


static yx_inline
yx_bool yx_os_dir_open(yx_char* name, yx_os_dir* dir){
    dir->dir = opendir(name);
    return (NULL != dir->dir)? yx_true : yx_false;
}


static yx_inline
yx_int yx_os_dir_close(yx_os_dir* dir){
    return closedir(dir->dir);
}

//return NULL means no sub files
static yx_inline
yx_os_dirIterator yx_os_dir_iterate(yx_os_dir* dir){
    return (yx_os_dirIterator)readdir(dir->dir);
}

#pragma mark walk
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
/*diriterator walk*/
//typedef void (*yx_os_dirIterator_walk_callback)(yx_os_dirIterator iterator, yx_value value, yx_bool* stepIn, yx_bool* stop);
//
//void yx_os_dir_walk(yx_char* path, yx_value value, yx_os_dirIterator_walk_callback callback);

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
/*diriterator about*/

#pragma mark - dir iterator info

#define yx_os_dirIterator_name(dirIterator)         (dirIterator->d_name)
#define yx_os_dirIterator_nameLen(dirIterator)      (dirIterator->d_namelen)
#define yx_os_dirIterator_type(dirIterator)         (dirIterator->d_type)
#define yx_os_dirIterator_file(dirIterator)         (dirIterator->d_ino)
#define yx_os_dirIterator_position(dirIterator)     (dirIterator->d_seekoff)


#define YX_OS_DIRITERATOR_TYPE_UNKNOWN   DT_UNKNOWN   //unknown file
#define YX_OS_DIRITERATOR_TYPE_FIFO      DT_FIFO      //fifo file (named pipe or other fifo)
#define YX_OS_DIRITERATOR_TYPE_CHA       DT_CHR       //character device file
#define YX_OS_DIRITERATOR_TYPE_DIR       DT_DIR       //directory file
#define YX_OS_DIRITERATOR_TYPE_BLK       DT_BLK       //block device file
#define YX_OS_DIRITERATOR_TYPE_REG       DT_REG       //regular file
#define YX_OS_DIRITERATOR_TYPE_LNK       DT_LNK       //link file
#define YX_OS_DIRITERATOR_TYPE_SOCK      DT_SOCK      //socket file
#define YX_OS_DIRITERATOR_TYPE_WHT       DT_WHT       //???????



#endif



