//
//  yx_macosx_file_access.c
//  YXLib
//
//  Created by Yuxi Liu on 9/28/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#include "yx_macosx_file_access.h"

#include "yx_macosx_file_def.h"
#include "../environment/yx_macosx_environment.h"

yx_bool yx_os_fileinfo_isreadable(yx_os_fileinfo info)
{
    
    yx_bool readable = yx_false;
    
    do {
        /*verify the euid*/
        yx_acccount_id id = yx_os_environment_token_userid();
        
        if (0 == id) { //root account
            readable = yx_true;
            break;
        }
        
        if (info.st_uid == id) { //euid matched
            readable = (( (info.st_mode & S_IRUSR) != 0)? yx_true : yx_false);
            break;
        }
        
        
        /*verify the eguid*/
        id = yx_os_environment_token_groupid();
        if(info.st_gid == id) { //eguid matched
            readable = (( (info.st_mode & S_IRGRP) != 0)? yx_true : yx_false);
            break;
        }
        
        /*verify the other user*/
        readable = (( (info.st_mode & S_IROTH) != 0)? yx_true : yx_false);
        break;
        
        
    } while (0);
    
    
    return readable;
    
}


yx_bool yx_os_fileinfo_iswritable(yx_os_fileinfo info)
{
    yx_bool readable = yx_false;
    
    do {
        /*verify the euid*/
        yx_acccount_id id = yx_os_environment_token_userid();
        
        if (0 == id) { //root account
            readable = yx_true;
            break;
        }
        
        if (info.st_uid == id) { //euid matched
            readable = (( (info.st_mode & S_IWUSR) != 0)? yx_true : yx_false);
            break;
        }
        
        
        /*verify the eguid*/
        id = yx_os_environment_token_groupid();
        if(info.st_gid == id) { //eguid matched
            readable = (( (info.st_mode & S_IWGRP) != 0)? yx_true : yx_false);
            break;
        }
        
        /*verify the other user*/
        readable = (( (info.st_mode & S_IWOTH) != 0)? yx_true : yx_false);
        break;
        
        
    } while (0);
    
    
    return readable;
}
yx_bool yx_os_fileinfo_isexeable(yx_os_fileinfo info)
{
    yx_bool readable = yx_false;
    
    do {
        /*verify the euid*/
        yx_acccount_id id = yx_os_environment_token_userid();
        
        if (0 == id) { //root account
            readable = yx_true;
            break;
        }
        
        if (info.st_uid == id) { //euid matched
            readable = (( (info.st_mode & S_IXUSR) != 0)? yx_true : yx_false);
            break;
        }
        
        
        /*verify the eguid*/
        id = yx_os_environment_token_groupid();
        if(info.st_gid == id) { //eguid matched
            readable = (( (info.st_mode & S_IXGRP) != 0)? yx_true : yx_false);
            break;
        }
        
        /*verify the other user*/
        readable = (( (info.st_mode & S_IXOTH) != 0)? yx_true : yx_false);
        break;
        
        
    } while (0);
    
    
    return readable;
}

