//
//  yx_config_environment.h
//  YXLib
//
//  Created by Yuxi Liu on 9/28/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CONFIG_ENVIRONMENT_H_
#define _YX_CONFIG_ENVIRONMENT_H_





#define YX_MAX_PATH_LEN 1024

//#define YX_HOME_PATH  "~/"



////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
/*use the system environment seting*/

#define YX_USE_SYS_ENVIRONMENT 1



#if YX_USE_SYS_ENVIRONMENT
    /*home path*/
    #define YX_ENV_NAME_HOME "HOME"
    #define YX_ENV_NAME_TEMPDIR "TMPDIR"
#endif


//whether the file system is case-sensitive.
#if YX_USE_SYS_ENVIRONMENT
    #define YX_ENV_CASELES_FILESYSTEM 0
#endif


//big-endian or little-endian

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
 significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
#  if defined __BIG_ENDIAN__
#    define YX_ENV_WORDS_BIGENDIAN 1
#  endif
#endif


#endif






