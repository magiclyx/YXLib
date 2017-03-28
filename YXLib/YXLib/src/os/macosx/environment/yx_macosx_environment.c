//
//  yx_macosx_environment.c
//  YXLib
//
//  Created by Yuxi Liu on 9/28/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#include "yx_macosx_environment.h"
#include "../err/yx_macosx_err.h"
#include "../system/yx_macosx_system.h"
#include "../string/yx_macosx_string.h"

#pragma mark - path
////////////////////////////////////////////////////////////////////////////////////////////////////
/*path*/
////////////////////////////////////////////////////////////////////////////////////////////////////



static yx_char _g_yx_macosx_environment_path_app[YX_OS_MAXPATH_LEN + 1] = ""; //app
static yx_char _g_yx_macosx_environment_path_document[YX_OS_MAXPATH_LEN + 1] = ""; //document
static yx_char* _g_yx_macosx_environment_path_home = NULL; //home
static yx_char* _g_yx_macosx_environment_path_temp = NULL; //temp



/*
 app
 */
void yx_os_environment_set_app_path(const yx_char* path)
{
    yx_os_strncpy(_g_yx_macosx_environment_path_app, path, YX_OS_MAXPATH_LEN);
}



/*
 home
 */
const yx_char* _yx_os_environment_get_home_path(void)
{
    

#if YX_USE_SYS_ENVIRONMENT
    if (NULL == _g_yx_macosx_environment_path_home) {
        _g_yx_macosx_environment_path_home = yx_os_get_environment(YX_ENV_NAME_HOME);
    }
#else
    if (NULL == _g_yx_macosx_environment_path_home) {
        struct passwd* pw = getpwuid( yx_os_environment_userid() );
        home_path = pw->pw_dir;
    }
#endif
    
    return _g_yx_macosx_environment_path_home;
}


/*
 temp
 */
const yx_char* _yx_os_environment_get_temp_path(void)
{
    
#if YX_USE_SYS_ENVIRONMENT
    if (NULL == _g_yx_macosx_environment_path_temp) {
        _g_yx_macosx_environment_path_temp = yx_os_get_environment(YX_ENV_NAME_TEMPDIR);
    }
#else
    if (NULL == _g_yx_macosx_environment_path_temp) {
        _g_yx_macosx_environment_path_temp = "/var/tmp";
    }
#endif
    
    return _g_yx_macosx_environment_path_temp;
}


/*
 document
 */

void yx_os_environment_set_document_path(const yx_char* path)
{
    strncpy(_g_yx_macosx_environment_path_document, path, YX_OS_MAXPATH_LEN);
}


const yx_char* _yx_os_environment_get_document_path(void)
{
    if (_g_yx_macosx_environment_path_document[0] == '\0')
    {
        if (_g_yx_macosx_environment_path_app[0] != '\0')
        {
            strncpy(_g_yx_macosx_environment_path_document, _g_yx_macosx_environment_path_app, YX_OS_MAXPATH_LEN);
        }
        else
        {
            strncpy(_g_yx_macosx_environment_path_document, YX_OS_PATH_HOME, YX_OS_MAXPATH_LEN);
        }
    }
    
    return _g_yx_macosx_environment_path_document;
}


/*
 working
 */
const yx_char* _yx_os_environment_get_working_path(void)
{
    static yx_char _g_yx_macosx_environment_path_working[YX_OS_MAXPATH_LEN + 1];
    
    getcwd(_g_yx_macosx_environment_path_working, YX_OS_MAXPATH_LEN);
    
    return _g_yx_macosx_environment_path_working;
}

void yx_os_environment_set_working_path(const yx_char* path)
{
    assert(NULL != path);
    chdir(path);
}


#pragma mark - launch parameter
////////////////////////////////////////////////////////////////////////////////////////////////////
/*launch parameter*/
////////////////////////////////////////////////////////////////////////////////////////////////////
extern char **environ;
yx_int    yx_os_argc = -1;
yx_char** yx_os_argv = NULL;
yx_char** yx_os_environment = NULL;

void yx_os_environment_save_launch_parameter(int argc, const char** argv)
{
    yx_os_argc = argc;
    yx_os_argv = (yx_char**)argv;
    yx_os_environment = environ;
}






