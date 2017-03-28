//
//  yx_macosx_process.c
//  YXLib
//
//  Created by LiuYuxi on 15/8/7.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_macosx_process.h"
#include "../environment/yx_macosx_environment.h"
#include "../string/yx_macosx_string.h"
#include "../allocator/yx_macosx_allocator.h"
#include "../allocator/yx_macosx_mem.h"





/**
 *  set the process title
 *
 *  1. must call the yx_os_environment_save_launch_parameter to set the argc and argv.
 *  2. use std-c alloator a space in this function and will not release it until the end of the process
 */


yx_bool yx_os_process_set_title(const yx_char* title)
{
    
    static char* _argv_last = NULL;
    
    
    assert(NULL != title);
    
    if ( ! yx_os_environment_laun_parameter_validate )
    {
        /*
         have not init the launch parameter
         You must call the "yx_os_environment_save_launch_parameter" to set the launch parameter first
         */
        assert(0);
        return yx_false;
    }
    
    
    if (NULL == _argv_last)
    {
        yx_size env_size = 0;
        for (yx_int i=0; yx_os_environment[i]; i++)
        {
            env_size += yx_os_strlen(yx_os_environment[i]) + 1;
        }
        
        
        //use std-c allocator to alloc a new environment space
        yx_allocator c_allocator = yx_stdC_allocator();
        yx_char* p = yx_allocator_alloc(c_allocator, env_size);
        if (yx_unlikely( NULL == p ))
            return yx_false;
        
        //init the _argv_last pt
        _argv_last = yx_os_argv[0];
        for (yx_int i=0; yx_os_argv[i]; i++)
        {
            if (_argv_last == yx_os_argv[i])
            {
                _argv_last = yx_os_argv[i] + yx_os_strlen(yx_os_argv[i]) + 1;
            }
        }
        
        
        //copy the environ to the new space
        for (yx_int i=0; yx_os_environment[i]; i++)
        {
            if (_argv_last == yx_os_environment[i])
            {
                yx_size size = yx_os_strlen(yx_os_environment[i]) + 1;
                _argv_last = yx_os_environment[i] + size;
                
                yx_os_strfillstr(p, (yx_char*)yx_os_environment[i], (yx_strSize)size);
                yx_os_strncpy(p, (yx_char*)yx_os_environment[i], (yx_strSize)size);
                yx_os_environment[i] = p;
                
                p += size;
            }
        }
        
        
        yx_os_argv[1] = NULL;
        
        _argv_last--;

    }
    
    
    /*set the new title*/
    yx_char* end = yx_os_strfillstr(yx_os_argv[0], (yx_char*)title, (yx_strSize)(_argv_last - yx_os_argv[0]));
    
    
    //clear the remain space
    if (_argv_last - end)
        yx_os_memset(end, '\0', _argv_last - end);
    
    
    return yx_true;
}


const yx_char* yx_os_process_title(void)
{
    if ( ! yx_os_environment_laun_parameter_validate )
    {
        /*
         have not init the launch parameter
         You must call the "yx_os_environment_save_launch_parameter" to set the launch parameter first
         */
        return NULL;
    }
    
    return yx_os_argv[0];

}








/*
 create the process
 */
yx_bool yx_os_process_fork(yx_os_process_ref process, const yx_char* name, yx_bool* isChild)
{
    assert(NULL != process);
    
    /*init process with the parent process info*/
    yx_os_process_current_init(process);
    
    pid_t pid;
    if (yx_likely( -1 != (pid = fork()) ))
    {
        if (0 == pid)
        {
            *isChild = yx_true;
            
            if (NULL != name)
                yx_os_process_set_title(name);
            
            process->need_close_handle = yx_false;
        }
        else
        {
            *isChild = yx_false;
            process->process_id = pid;
            process->need_close_handle = yx_true;
        }
        
        
        return yx_true;
    }
    else
    {
        /*error occur, set the process_id to -1*/
        process->process_id = -1;
        
        return yx_false;
    }
}


yx_bool yx_os_process_closeHandle(yx_os_process_ref process, yx_bool wait)
{
    assert(NULL != process);
    
    if (yx_os_process_isValid(process))
    {
        if (process->need_close_handle)
        {
            int flag = 0;
            if (wait)
                flag |= WNOHANG;
            
            if (0 != waitpid(process->process_id, NULL, flag))
            {
                process->need_close_handle = yx_false;
                
                return yx_true;
            }
            else
            {
                return yx_false;
            }
        }
    }
    
    
    return yx_false;
}







































