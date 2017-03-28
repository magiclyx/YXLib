//
//  yx_macosx_file_dir.c
//  YXLib
//
//  Created by Yuxi Liu on 9/30/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#include "yx_macosx_file_dir.h"


//static yx_inline
//yx_bool _yx_os_dir_walk(yx_char* path, yx_value value, yx_os_dirIterator_walk_callback callback){
//    yx_bool stop = yx_false;
//    
//    yx_os_dir dir;
//    
//    if(yx_true == yx_os_dir_open(path, &dir)){
//        for (;;) {
//            
//            yx_os_dirIterator it = yx_os_dir_iterate(&dir);
//            if (NULL != it) {
//                yx_bool stepIn = yx_false;
//                callback(it, value, &stepIn, &stop);
//                
//                if (yx_true == stop)
//                    break;
//            
//                if (yx_true == stepIn) {
//                    yx_os_dirIterator_name(it);
//                }
//        }
//    }
//}
//
//
//void yx_os_dir_walk(yx_char* path, yx_value value, yx_os_dirIterator_walk_callback callback){
//    
//}
