//
//  yx_core_bit.h
//  YXLib
//
//  Created by Yuxi Liu on 9/4/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_BIT_H_
#define _YX_CORE_BIT_H_

//#include "datatype/yx_core_basictypes.h"

/*set or remove a flag from a value*/
#define yx_set_flag(val, flag)      (val |= flag)
#define yx_remove_flag(val, flag)   (val &= ~flag)



/**
 *  bit operation
 *
 *  pos buff第多少位
 *  flag 二进制buff
 */
#define _yx_bit_mask(pos) (1 << ( ((pos) < YX_BYTE_BIT_LEN) ? (pos) : ((pos) % YX_BYTE_BIT_LEN) )) //设置掩码的位置,模YX_BYTE_BIT_LEN是为了限制最多只能左移7位.
#define _yx_bit_slot(pos) ((pos) / YX_BYTE_BIT_LEN) //用于从一个二进制数组中找到掩码在第几个BIT位

#define yx_bit_set(flag, pos) ((flag)[_yx_bit_slot (pos)] |= _yx_bit_mask(pos)) //设置某位的值
#define yx_bit_clear(flag, pos) ((flag)[_yx_bit_slot (pos)] &= ~_yx_bit_mask(pos)) //用于清除某位的值
#define yx_bit_test(flag, pos) ((flag)[_yx_bit_slot (pos)] & _yx_bit_mask(pos)) //用于检验某位是否为
#define yx_bit_nslots(num) ((num + YX_BYTE_BIT_LEN - 1) / YX_BYTE_BIT_LEN) //用于求出数组的个数

/*
yx_byte flag[yx_bit_nslots(50)]; //申请能容纳50位的数组
memset(flag, 0, sizeof(yx_byte)*sizeof(flag)); //将这个区域清0
if (yx_bit_test(flag, 23))
{ //如果23位是1
    yx_bit_clear(flag, 23); //将23位清零
}
else
{ //如果23位是0
    yx_bit_set(flag, 23); //将23位设置为1
}
*/


////////////////////////////////////////////////////////////////////////////////////////////////////
/*yx_valuexx_max_bit_len*/
////////////////////////////////////////////////////////////////////////////////////////////////////
#define __YX_VAL_MAXLEN_XTAB1_(a) ((a)>=128?8:(a)>=64?7:(a)>=32?6:(a)>=16?5:(a)>=8?4:(a)>=4?3:(a)>=2?2:(a))
#define __YX_VAL_MAXLEN_XTAB4_(a) __YX_VAL_MAXLEN_XTAB1_(a+0), __YX_VAL_MAXLEN_XTAB1_(a+1), __YX_VAL_MAXLEN_XTAB1_(a+2), __YX_VAL_MAXLEN_XTAB1_(a+3)
#define __YX_VAL_MAXLEN_XTAB16_(a) __YX_VAL_MAXLEN_XTAB4_(a+0), __YX_VAL_MAXLEN_XTAB4_(a+4), __YX_VAL_MAXLEN_XTAB4_(a+8), __YX_VAL_MAXLEN_XTAB4_(a+12)

static const int __yx_val_max_bitlen_xtab_[] = {
    __YX_VAL_MAXLEN_XTAB16_(0x00), __YX_VAL_MAXLEN_XTAB16_(0x10), __YX_VAL_MAXLEN_XTAB16_(0x20), __YX_VAL_MAXLEN_XTAB16_(0x30),
    __YX_VAL_MAXLEN_XTAB16_(0x40), __YX_VAL_MAXLEN_XTAB16_(0x50), __YX_VAL_MAXLEN_XTAB16_(0x60), __YX_VAL_MAXLEN_XTAB16_(0x70),
    __YX_VAL_MAXLEN_XTAB16_(0x80), __YX_VAL_MAXLEN_XTAB16_(0x90), __YX_VAL_MAXLEN_XTAB16_(0xA0), __YX_VAL_MAXLEN_XTAB16_(0xB0),
    __YX_VAL_MAXLEN_XTAB16_(0xC0), __YX_VAL_MAXLEN_XTAB16_(0xD0), __YX_VAL_MAXLEN_XTAB16_(0xE0), __YX_VAL_MAXLEN_XTAB16_(0xF0)
};


/*max bit len in value for 32bit*/
#define yx_value32_max_bit_len(val) \
val & 0xff000000?24 + __yx_val_max_bitlen_xtab_[val>>24]: \
val & 0x00ff0000?16 + __yx_val_max_bitlen_xtab_[val>>16]: \
val & 0x0000ff00?8  + __yx_val_max_bitlen_xtab_[val>>8]: \
__yx_val_max_bitlen_xtab_[val]


/*max bit len in value for 64bit*/
#define yx_value64_max_bit_len(val) \
val & 0xFF00000000000000?56 + __yx_val_max_bitlen_xtab_[val>>56]: \
val & 0xFF000000000000?48 + __yx_val_max_bitlen_xtab_[val>>48]: \
val & 0xFF0000000000?40 + __yx_val_max_bitlen_xtab_[val>>40]: \
val & 0xff00000000?32 + __yx_val_max_bitlen_xtab_[val>>32]: \
val & 0xff000000?24 + __yx_val_max_bitlen_xtab_[val>>24]: \
val & 0x00ff0000?16 + __yx_val_max_bitlen_xtab_[val>>16]: \
val & 0x0000ff00?8  + __yx_val_max_bitlen_xtab_[val>>8]: \
__yx_val_max_bitlen_xtab_[val]

////////////////////////////////////////////////////////////////////////////////////////////////////


#endif
