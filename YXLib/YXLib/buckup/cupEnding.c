//
//  cupEnding.c
//  encryptionOnTable
//
//  Created by Yuxi Liu on 12/23/12.
//  Copyright (c) 2012 Yuxi Liu. All rights reserved.
//

#include <stdio.h>
#include <assert.h>

#include "cupEnding.h"




/*verify the above data type*/
#define cpu_ending_data_type_verify(e, detail) typedef char cpu_ending_tool_(data_type_size_error, __##detail) [(e)?1:-1];
#define cpu_ending_tool_2_(X,Y) X##Y
#define cpu_ending_tool_( X , Y ) cpu_ending_tool_2_( X, Y )



static int g_cpuendingtype_flag = -1;


int cpuEndingCheck(){
    
    if(-1 == g_cpuendingtype_flag){
    
        cpu_ending_data_type_verify(sizeof(CM_UINT32) == 4, CM_UINT32_must_a_32_bit_datatype);
        cpu_ending_data_type_verify(sizeof(CM_UCHAR) == 1, CM_UCHAR_must_a_8_bit_datatype);
    
        union{
             CM_UINT32 a;
             CM_UCHAR b;
        }c;
    
        c.a = 1;
        g_cpuendingtype_flag = (c.b == 1);
        
    }
    
    return g_cpuendingtype_flag;
}


CM_UINT16 cpuEnding16bitConvert(CM_UINT16 val){
    
    cpu_ending_data_type_verify(sizeof(CM_UINT16) == 2, CM_UINT16_must_a_16_bit_datatype);
    
    if(1 == cpuEndingCheck()){
//        val = ((((CM_UINT16)(val) & 0xff00) >> 8) | \
//               (((CM_UINT16)(val) & 0x00ff) << 8));

        val = ((val<<8)|(val>>8));
    }
    
    return val;
}

CM_UINT32 cpuEnding32bitConvert(CM_UINT32 val){
    
    cpu_ending_data_type_verify(sizeof(CM_UINT32) == 4, CM_UINT32_must_a_32_bit_datatype);
    
    if(1 == cpuEndingCheck()){
//        val = ((((CM_UINT32)(val) & 0xff000000) >> 24) | \
//               (((CM_UINT32)(val) & 0x00ff0000) >> 8) | \
//               (((CM_UINT32)(val) & 0x0000ff00) << 8) | \
//               (((CM_UINT32)(val) & 0x000000ff) << 24));

        val = ((val<<24)|((val<<8)&0x00FF0000)|((val>>8)&0x0000FF00)|(val>>24));
    }
    
    return val;
}



CM_UINT64 cpuEnding64bitConvert(CM_UINT64 val){
    
    
    cpu_ending_data_type_verify(sizeof(CM_UINT64) == 8, CM_UINT64_must_a_64_bit_datatype);
    cpu_ending_data_type_verify(sizeof(CM_UINT32) == 4, CM_UINT32_must_a_32_bit_datatype);
    
    
    if(1 == cpuEndingCheck()){
        CM_UINT32 hi, lo;
        
        //separate into high and low 32-bit values and swap them.
        
        lo = (CM_UINT32)(val & 0xFFFFFFFF); //store the low 32bit.
        val >>= 32;
        hi = (CM_UINT32)(val & 0XFFFFFFFF); //store the high 32 bit.s
        
        val = cpuEnding32bitConvert(lo);  //set the low 32bit.
        val <<= 32;
        val |= cpuEnding32bitConvert(hi); //set the high 32bit.
    }
    
    return val;
}












