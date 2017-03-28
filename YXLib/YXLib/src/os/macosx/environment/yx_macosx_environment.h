//
//  yx_macosx_account.h
//  YXLib
//
//  Created by Yuxi Liu on 9/28/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_ENVIRONMENT_H_
#define _YX_MACOSX_ENVIRONMENT_H_

#pragma mark - user info

#include "../basictypes/yx_macosx_types.h"


#pragma mark - environment
////////////////////////////////////////////////////////////////////////////////////////////////////
/*system environment*/
////////////////////////////////////////////////////////////////////////////////////////////////////
#define yx_os_get_environment(env_name)  getenv(env_name) //NULL if not exist


#pragma mark - account
////////////////////////////////////////////////////////////////////////////////////////////////////
/*account*/
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef yx_uint32 yx_acccount_id;

#define yx_os_environment_userid     getuid
#define yx_os_environment_groupid    getgid

#define yx_os_environment_token_userid    geteuid
#define yx_os_environment_token_groupid   getegid



#pragma mark - path
////////////////////////////////////////////////////////////////////////////////////////////////////
/*path*/
////////////////////////////////////////////////////////////////////////////////////////////////////

const yx_char* _yx_os_environment_get_home_path(void);
const yx_char* _yx_os_environment_get_temp_path(void);
const yx_char* _yx_os_environment_get_document_path(void);
const yx_char* _yx_os_environment_get_working_path(void);



void yx_os_environment_set_app_path(const yx_char* path);
void yx_os_environment_set_document_path(const yx_char* path);
void yx_os_environment_set_working_path(const yx_char* path);


//YX_ENV_NAME_TEMPDIR
#define YX_OS_PATH_ROOT  "/"
#define YX_OS_PATH_HOME  _yx_os_environment_get_home_path()
#define YX_OS_PATH_TEMP  _yx_os_environment_get_temp_path()
#define YX_OS_PATH_DOC   _yx_os_environment_get_document_path()
#define YX_OS_PATH_WORK  _yx_os_environment_get_working_path()


#pragma mark - launch parameter
////////////////////////////////////////////////////////////////////////////////////////////////////
/*launch parameter*/
////////////////////////////////////////////////////////////////////////////////////////////////////

extern yx_int    yx_os_argc;
extern yx_char** yx_os_argv;
extern yx_char** yx_os_environment;

void yx_os_environment_save_launch_parameter(int argc, const char** argv);

#define yx_os_environment_laun_parameter_validate  \
    ( (-1 != yx_os_argc)  &&  (NULL != yx_os_argv)  &&  (NULL != yx_os_environment) )

#endif





