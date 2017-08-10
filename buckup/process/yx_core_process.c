//
//  yx_core_process.c
//  YXLib
//
//  Created by LiuYuxi on 15/8/12.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_process.h"
#include "../debug/yx_core_assert.h"
#include "../communicator/yx_core_communicator.h"



yx_bool yx_core_process_init(yx_core_process_ref process)
{
    YX_ASSERT(NULL != process);
    yx_os_process_initnull( &(process->process) );
    yx_os_strempty(process->name);
    
    process->status = yx_os_procstatus_unknown;
    
    return yx_true;
}


yx_bool yx_core_process_recycle(yx_core_process_ref process, yx_bool wait)
{
    YX_ASSERT(NULL != process);
    
    return yx_os_process_closeHandle(&(process->process), wait);
}






yx_bool yx_core_process_fork(yx_core_process_ref process, const yx_char* name, yx_bool* isChild)
{
    
    YX_ASSERT(NULL != process);
    
    
    const char* parentTitle = yx_os_process_title();
    
    if (yx_likely( yx_os_process_fork(&(process->process), name, isChild) ))
    {
        if (isChild)
        {
            /*set the parent process's name*/
            if (NULL != parentTitle)
                yx_os_strncpy(process->name, parentTitle, yx_os_process_maxname_len);
            
            process->status = yx_os_procstatus_running;
        }
        else
        {
            /*set the child process's name*/
            if (NULL != name)
                yx_os_strncpy(process->name, name, yx_os_process_maxname_len);
            
            process->status = yx_os_procstatus_running;
        }
        
        
        return yx_true;
    }
    
    return yx_false;
}


yx_os_procexit_status yx_core_process_status(yx_core_process_ref process)
{
    YX_ASSERT(NULL != process);
    YX_ASSERT(yx_os_process_isValid(&(process->process)));
    
    if (!yx_os_process_isExit(process->status))
    {
        process->status = yx_os_process_exitstatus(&(process->process));
    }
    
    return process->status;
}





