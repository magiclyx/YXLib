//
//  encryByRandomDict.h
//  encryptionOnTable
//
//  Created by Yuxi Liu on 12/20/12.
//  Copyright (c) 2012 Yuxi Liu. All rights reserved.
//

#ifndef cmlib_encryByRandomDict_h
#define cmlib_encryByRandomDict_h

#include "../yx_core_global.h"

#ifdef __cplusplus
extern "C"{
#endif

/************************/
//:~ low perform without my mempool->localPool
/************************/


typedef void* yx_core_encrypt_randomdict;

extern const yx_uint32 encryByRandomDict_max_key_base;


//if key is NULL, use current time as it's key.
yx_core_encrypt_randomdict yx_core_encrypt_randomdict_create(yx_allocator allocator, yx_uint32 seedKey, yx_uint32 dictNum);
void yx_core_encrypt_randomdict_destroy(yx_core_encrypt_randomdict* pHandle);

yx_uint32 yx_core_encrypt_randomdict_seedKeyFromString(const yx_char* key, yx_uint32 keyBase);
yx_uint32 yx_core_encrypt_random_currentSeedKey(yx_core_encrypt_randomdict handle);


//the src and the dist can be the same buff
void yx_core_encrypt_randomdict_encrypt(yx_core_encrypt_randomdict handle, yx_uchar8* pszDst, const yx_uchar8* pszSrc, yx_size len);
void yx_core_encrypt_randomdict_decrypt(yx_core_encrypt_randomdict handle, yx_uchar8* pszDst, const yx_uchar8* pszSrc, yx_size len);

#ifdef __cplusplus
}
#endif


#endif
