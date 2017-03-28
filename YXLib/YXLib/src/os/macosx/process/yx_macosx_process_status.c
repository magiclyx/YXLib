//
//  yx_macosx_process_status.c
//  YXLib
//
//  Created by LiuYuxi on 15/8/13.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_macosx_process_status.h"


yx_os_procexit_status yx_os_process_exitstatus(yx_os_process_ref process)
{
    assert(NULL != process);
    
    if (yx_os_process_isValid(process))
    {
        yx_os_procexit_status status = 0;
        switch (waitpid(yx_os_process_handle(process), &status, WNOHANG))
        {
            case -1:
                return yx_os_procestatus_error;
                break;
            case 0:
                return yx_os_procstatus_running;
            default:
                return status;
        }
    }
    
    return yx_os_procstatus_invald;
}


yx_bool yx_os_process_isPendding(yx_os_process_ref process)
{
    assert(NULL != process);
    
    if (yx_os_process_isValid(process))
    {
        yx_os_procexit_status status = 0;
        if(yx_unlikely( -1 == waitpid(yx_os_process_handle(process), &status, WNOHANG|WUNTRACED) ))
            return yx_false;
        
        return WIFSTOPPED(status);
    }
    
    return yx_false;
}



