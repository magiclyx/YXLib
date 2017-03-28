//
//  yx_core_file_scaner.c
//  YXLib
//
//  Created by yuxi on 16/2/11.
//  Copyright © 2016年 Yuxi Liu. All rights reserved.
//

#include "yx_core_file_scaner.h"


static yx_core_fileScaner_Error _dirWalker(yx_core_fileScaner_ref scaner, yx_char* pszPath);


yx_bool yx_core_fileScaner_init(yx_core_fileScaner_ref fileScaner,
                                yx_core_fileScaner_typeMask FileTypeMask,
                                yx_core_fileScaner_callback_directory_start callback_directory_start,
                                yx_core_fileScaner_callback_directory_end callback_directory_end,
                                yx_core_fileScaner_callback_file callback_file,
                                yx_core_fileScaner_callback_error callback_error)
{
    
    assert(NULL != fileScaner);
    
    
    fileScaner->type_mask = FileTypeMask;
    fileScaner->isRunning = yx_false;
    
    fileScaner->callback_directory_start = callback_directory_start;
    fileScaner->callback_directory_end = callback_directory_end;
    fileScaner->callback_file = callback_file;
    fileScaner->callback_error = callback_error;
    
    return yx_true;
}


yx_bool yx_core_fileScaner_scan(yx_core_fileScaner_ref scaner, const yx_char* pszPath, yx_value userData)
{
    yx_char path[YX_OS_MAXPATH_LEN + 1];
    
    yx_os_strncpy(path, pszPath, YX_OS_MAXPATH_LEN);
    
    
    yx_core_fileScaner_Error err = _dirWalker(scaner, path);
    
    scaner->isRunning = yx_true;
    if((NULL != scaner->callback_error) && (err == yx_core_fileScaner_err_Canceled) )
        scaner->callback_error(NULL, yx_core_fileScaner_err_Canceled, userData);
    scaner->isRunning = yx_false;
    
    return err;

}


static yx_core_fileScaner_Error _dirWalker(yx_core_fileScaner_ref scaner, yx_char* pszPath)
{
    yx_os_fileinfo file_info;
    
    //:~ 做到这里
    return NULL;
}
















































