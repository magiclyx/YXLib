//
//  yx_macosx_file_authorization.h
//  YXLib
//
//  Created by Yuxi Liu on 9/28/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_FILE_AUTHORIZATION_H_
#define _YX_MACOSX_FILE_AUTHORIZATION_H_


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark - mode

/*mode*/
#define YX_FILE_AU_None   0

#define YX_OS_FILE_AU_USRREAD          S_IRUSR    //usr read
#define YX_OS_FILE_AU_USRWRITE         S_IWUSR    //usr write
#define YX_OS_FILE_AU_USREXECUTE       S_IXUSR    //usr execute
#define YX_OS_FILE_AU_USRALL           S_IRWXU    //S_IRUSR | S_IWUSR | S_IXUSR

#define YX_OS_FILE_AU_GRPREAD          S_IRGRP    //group read
#define YX_OS_FILE_AU_GRPWRITE         S_IWGRP    //group write
#define YX_OS_FILE_AU_GRPEXECUTE       S_IXGRP    //gorup execute
#define YX_OS_FILE_AU_GRPALL           S_IRWXG    //S_IRGRP | S_IWGRP | S_IXGRP

#define YX_OS_FILE_AU_OTHERREAD        S_IROTH    //other read
#define YX_OS_FILE_AU_OTHERWRITE       S_IWOTH    //other write
#define YX_OS_FILE_AU_OTHEREXECUTE     S_IXOTH    //other execute
#define YX_OS_FILE_AU_OTHERALL         S_IRWXO    //S_IROTH | S_IWOTH | S_IXOTH

//some convenient defined
#define YX_OS_FILE_AU_USERDEFAULT      S_IRUSR | S_IWUSR //user read write
#define YX_OS_FILE_AU_GRPDEFAULT       S_IRGRP | S_IWGRP //group read write
#define YX_OS_FILE_AU_OTHERDEFAULT     S_IROTH | S_IWOTH //other read write

#define YX_OS_FILE_AU_DEFAULT          YX_OS_FILE_AU_USERDEFAULT | YX_OS_FILE_AU_GRPDEFAULT | YX_OS_FILE_AU_OTHERDEFAULT
#define YX_OS_FILE_AU_EXECUTE          YX_OS_FILE_AU_USRALL | YX_OS_FILE_AU_GRPALL | YX_OS_FILE_AU_OTHERALL



#define YX_OS_FILE_AU_CHANGEOWNER  S_ISUID    //when the file is running. using the owner's uid as it uid
#define YX_OS_FILE_AU_CHANGEGROUP  S_ISGID    //when the file is running. using the owner's gid as it gid

//other process can not del this file even it has the write authorization
//other process must not the file's owner, must not be the root usr, must not has the parent folder's owner
#define YX_OS_FILE_AU_STICKY   S_ISVTX    //sticky bit



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark - modify authorization
/*set file access authorization*/

//set the process default authborization
//if the yx_os_file_open does not indicate an autorization. will using the default authorization
#define yx_os_set_default_authorization(authoriazation)    umask(authorization)

//set authorization by path
#define yx_os_set_path_authorization(path, authorizaiton)   chmod(file, authorization)

//set authorizaton by file
#define yx_os_set_file_authorization(file, authorization)   fchmod(file, authorization)




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark owner
/*set file owner*/

//set owner by file
#define yx_os_file_setowner(file, uid, gid)        fchown(file, uid, gid)

//set owner by path
#define yx_os_path_setowner(path, uid, gid)        lchown(path, uid, gid)
//set the owner of the original file on the link to the file
#define yx_os_path_setowner_jmp(path, uid, gid)    chown(path, uid, gid)

//set owner by fileinfo
#define yx_os_fileinfo_owner(info)              (info.st_uid)
#define yx_os_fileinfo_grpowner(info)           (info.st_gid)



#endif












