//
//  yx_macosx_file.h
//  YXLib
//
//  Created by Yuxi Liu on 9/25/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_FILE_READWRITE_H_
#define _YX_MACOSX_FILE_READWRITE_H_


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark - read & write file
/*read & write file*/
//return the size be read. 0 means the end of file. -1 means an error occured
#define yx_os_file_read(file, buff, size)  read(file, buff, size)
#define yx_os_file_write(file, buff, size) write(file, buff, size)

//diff from the read/write
//1.seek and read/write is an atomic operation
//2.do not modify the file pointer
//3.high performance---> all searchread function can working at the same time.
#define yx_os_file_offsetread(file, buf, size, offset)   pread(file, buf, size, offset)
#define yx_os_file_offsetwrite(file, buf, size, offset)  pwrete(file, buf, size, offset)


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark - flush

/*flush the file*/
#define yx_os_file_flush(file)             fsync(file)
//#define yx_os_file_flush_dataonly(file)    fdatasync(file) ??



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark - seek
/*seek the file pointer*/
#define YX_OS_FILE_POS_START           SEEK_SET
#define YX_OS_FILE_POS_CURRENT         SEEK_CUR
#define YX_OS_FILE_POS_END             SEEK_END


#define yx_os_file_seek(file, offset, pos)      lseek(file, offset, pos)


#endif



