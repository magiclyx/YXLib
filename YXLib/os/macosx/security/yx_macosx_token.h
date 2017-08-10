//
//  yx_macosx_token.h
//  YXLib
//
//  Created by Yuxi Liu on 9/25/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_TOKEN_H_
#define _YX_MACOSX_TOKEN_H_

typedef yx_uint64 yx_token_rule;  //define an object's access rule
typedef yx_uint64 yx_token; //define an object's permissions


typedef struct _rule{
    uint16_t owner_rule;
    uint16_t other_rule;
    uint16_t group_rule;
}rule;

yx_token_rule yx_init_rule()

#endif




