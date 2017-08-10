//
//  yx_core_file_scaner.h
//  YXLib
//
//  Created by yuxi on 16/2/11.
//  Copyright © 2016年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_FILE_SCANER_H_
#define _YX_CORE_FILE_SCANER_H_

#include "../yx_core_global.h"

typedef enum {
    yx_core_fileScaner_typeMask_Regular=1, /* regular */
    yx_core_fileScaner_typeMask_Pipe=2, /* named pipe (fifo) */
    yx_core_fileScaner_typeMask_CharacterSpecial=4, /* character special */
    yx_core_fileScaner_typeMask_Directory=8, /* directory */
    yx_core_fileScaner_typeMask_BlockSpecial=16, /* block special */
    yx_core_fileScaner_typeMask_SymbolLink=32, /* symbolck link */
    yx_core_fileScaner_typeMask_Socket=64, /* socket */
    yx_core_fileScaner_typeMask_Other=128, /* whitout */
    
    yx_core_fileScaner_typeMask_ALL=65535,
}yx_core_fileScaner_typeMask;


typedef enum {
    yx_core_fileScaner_err_Success=0,
    yx_core_fileScaner_err_Stat,
    yx_core_fileScaner_err_FileUnreadable,
    yx_core_fileScaner_err_DirectoryUnreadable,
    yx_core_fileScaner_err_Canceled
}yx_core_fileScaner_Error;



typedef struct yx_core_fileScaner_cursor{
    int b;
}yx_core_fileScaner_cursor, *yx_core_fileScaner_cursor_ref;


typedef void (*yx_core_fileScaner_callback_directory_start)(const yx_char* pszPath, const struct stat* statptr, void* userData); //found a directory
typedef void (*yx_core_fileScaner_callback_directory_end)(const yx_char* pszPath, const struct stat* statptr, void* userData);  //leave a directory
typedef void (*yx_core_fileScaner_callback_file)(yx_core_fileScaner_cursor_ref cursor, void* userData); //found a file
typedef void (*yx_core_fileScaner_callback_error)(const yx_char* pszPath, yx_core_fileScaner_Error erryrType, yx_value userData); //error!



typedef struct yx_core_fileScaner{
    
    yx_core_fileScaner_typeMask type_mask;
    yx_bool isRunning;
    
    yx_core_fileScaner_callback_directory_start callback_directory_start;
    yx_core_fileScaner_callback_directory_end callback_directory_end;
    yx_core_fileScaner_callback_file callback_file;
    yx_core_fileScaner_callback_error callback_error;
}yx_core_fileScaner, *yx_core_fileScaner_ref;






yx_bool yx_core_fileScaner_init(yx_core_fileScaner_ref fileScaner,
                                yx_core_fileScaner_typeMask FileTypeMask,
                                yx_core_fileScaner_callback_directory_start callback_directory_start,
                                yx_core_fileScaner_callback_directory_end callback_directory_end,
                                yx_core_fileScaner_callback_file callback_file,
                                yx_core_fileScaner_callback_error callback_error);



yx_bool yx_core_fileScaner_scan(yx_core_fileScaner_ref scaner, const yx_char* pszPath, yx_value userData);


#endif /* _YX_CORE_FILE_SCANER_H_ */
