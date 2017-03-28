//
//  cupEnding.h
//  encryptionOnTable
//
//  Created by Yuxi Liu on 12/23/12.
//  Copyright (c) 2012 Yuxi Liu. All rights reserved.
//

#ifndef encryptionOnTable_cupEnding_h
#define encryptionOnTable_cupEnding_h


#include "cmBasicTypes.h"

#ifdef __cplusplus
extern "C"{
#endif



//return value
//1 - little ending
//0 - big ending
int cpuEndingCheck();


CM_UINT16 cpuEnding16bitConvert(CM_UINT16 val);
CM_UINT32 cpuEnding32bitConvert(CM_UINT32 val);
CM_UINT64 cpuEnding64bitConvert(CM_UINT64 val);




#define cpuEnding16BitConvertMacro(val_address)  _cpuEnding16BitConvertMacro_impl_(val_address)
#define cpuEnding32BitConvertMacro(val_address)  _cpuEnding32BitConvertMacro_impl_(val_address)
#define cpuEnding64bitConvertmacro(val_address)  _cpuEnding64bitConvertmacro_impl_(val_address)




#define _cpuEnding16BitConvertMacro_impl_(val_address) \
do{ \
*val_address = ((((CM_UINT16)(*val_address) & 0xff00) >> 8) | \
(((CM_UINT16)(*val_address) & 0x00ff) << 8)); \
}while(0)


#define _cpuEnding32BitConvertMacro_impl_(val_address) \
do{ \
if(1 == cpuEndingCheck()){ \
*val_address = ((((CM_UINT32)(*val_address) & 0xff000000) >> 24) | \
(((CM_UINT32)(*val_address) & 0x00ff0000) >> 8) | \
(((CM_UINT32)(*val_address) & 0x0000ff00) << 8) | \
(((CM_UINT32)(*val_address) & 0x000000ff) << 24)); \
} \
}while(0)


#define _cpuEnding64bitConvertmacro_impl_(val_address) \
do{ \
if(1 == cpuEndingCheck()){ \
CM_UINT32 hi, lo; \
lo = (CM_UINT32)((*val_address) & 0xFFFFFFFF); \
val >>= 32; \
hi = (CM_UINT32)((*val_address) & 0XFFFFFFFF); \
(*val_address) = cpuEnding32BitConvertMacro(lo); \
(*val_address) <<= 32; \
(*val_address) |= cpuEnding32BitConvertMacro(hi); \
} \
}while(0)

    
#ifdef __cplusplus
}
#endif


#endif //encryptionOnTable_cupEnding_h


