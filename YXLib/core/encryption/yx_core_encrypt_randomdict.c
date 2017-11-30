//
//  encryByRandomDict.c
//  encryptionOnTable
//
//  Created by Yuxi Liu on 12/20/12.
//  Copyright (c) 2012 Yuxi Liu. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <math.h>

#include <limits.h>


#include "../encryption/yx_core_encrypt_randomdict.h"


const yx_uint32 encryByRandomDict_max_key_base = 100;




// Small prime number used as a multiplier in the supplied hash functions
const yx_uint32 RANDOM_DICT_HASH_MULTIPLIER = 101;

#define RANDOM_DICT_HASH_SHIFT_MULTIPLY  //force to use MULTIPLY

#ifdef RANDOM_DICT_HASH_SHIFT_MULTIPLY
# define RANDOM_DICT_HASH_MULTIPLY(dw)   (((dw) << 7) - (dw))
#else
# define RANDOM_DICT_HASH_MULTIPLY(dw)   ((dw) * RANDOM_DICT_HASH_MULTIPLIER)
#endif


#define random_dict_setEncrypt(context_ptr, tableNum, index, code) ((((context_ptr->dict)[tableNum]).encry_table)[index] = code)
#define random_dict_setDecrypt(context_ptr, tableNum, index, code) ((((context_ptr->dict)[tableNum]).decrypt_table)[index] = code)
#define random_dict_encrypt(contex_ptr, char, tableNum) ((((contex_ptr->dict)[tableNum]).encry_table)[char])
#define random_dict_decrypt(contex_ptr, char, tableNum) ((((contex_ptr->dict)[tableNum]).decrypt_table)[char])

#define random_dict_handle2contextRef(handle)  ((_random_dict_encry_context_ref)handle)

typedef struct{
    yx_uchar8 encry_table[256];
    yx_uchar8 decrypt_table[256];
}_random_dict_, *_random_dict_ref_;

typedef struct{
    yx_allocator allocator;

    yx_uint32 seed;
    yx_uint32 currentKey;
    yx_uint32 dictNum;
    _random_dict_ref_ dict;
}_random_dict_encry_context, *_random_dict_encry_context_ref;




static yx_uint32 _nativeKeyIterator (yx_uint32 *key);
static void _initDict(_random_dict_encry_context_ref context, yx_int tableNum);



yx_core_encrypt_randomdict yx_core_encrypt_randomdict_create(yx_allocator allocator, yx_uint32 seedKey, yx_uint32 dictNum){
    
    _random_dict_ref_ table = NULL;
    _random_dict_encry_context_ref context = NULL;
    
    
    if (NULL == allocator)
        allocator = yx_get_allocator();
    
    
    context = (_random_dict_encry_context_ref)yx_allocator_alloc(allocator, sizeof(_random_dict_encry_context));
    if(NULL == context)
        goto errout;
    
    
    table = (_random_dict_ref_)yx_allocator_alloc(allocator, sizeof(_random_dict_)*dictNum);
    if(NULL == table)
        goto errout;
    
    
    context->allocator = allocator;
    context->dictNum = dictNum;
    context->seed = seedKey;
    context->currentKey = seedKey;
    context->dict = table;
    
    
    for(yx_int i=0; i<dictNum; i++)
        _initDict(context, i);
    
    
    
    return (yx_core_encrypt_randomdict)context;
    
errout:
    
    if(NULL != table)
        yx_allocator_free(allocator, table);

    if (NULL != context)
        yx_allocator_free(allocator, context);
    
    
    return NULL;
}


void yx_core_encrypt_randomdict_destroy(yx_core_encrypt_randomdict* pHandle){

    if(NULL == pHandle || NULL == *pHandle)
        return;
    
    
    _random_dict_encry_context_ref context = random_dict_handle2contextRef(*pHandle);
    *pHandle = NULL;
    
    
    yx_allocator_free(context->allocator, context->dict);
    yx_allocator_free(context->allocator, context);
    
}

yx_uint32 yx_core_encrypt_randomdict_seedKeyFromString(const yx_char* key, yx_uint32 keyBase){
    
    if(NULL == key)
        return (yx_uint32)(time(NULL));
    
    
    keyBase = (0==keyBase? strlen(key)+17 : keyBase) % encryByRandomDict_max_key_base;
    
    
    // force compiler to use unsigned arithmetic
    const yx_uchar8* upsz = (const yx_uchar8*) key;
    
    for ( ; *upsz; ++upsz)
        keyBase = RANDOM_DICT_HASH_MULTIPLY(keyBase) + *upsz;
    
    return keyBase;
}


yx_uint32 yx_core_encrypt_random_currentSeedKey(yx_core_encrypt_randomdict handle){
    
    assert(NULL != handle);
    
    _random_dict_encry_context_ref context = random_dict_handle2contextRef(handle);
    return context->seed;
}

void yx_core_encrypt_randomdict_encrypt(yx_core_encrypt_randomdict handle, yx_uchar8* pszDst, const yx_uchar8* pszSrc, yx_size len){
    
    assert(NULL != pszDst);
    assert(NULL != pszSrc);
    assert(NULL != handle);
    
    _random_dict_encry_context_ref context = random_dict_handle2contextRef(handle);
    yx_int dictNum = context->dictNum;
    
    
    yx_int index = 0;
    while (index<len){
        for(yx_int currentDict = 0; index<len && currentDict<dictNum; currentDict++){
            pszDst[index] = random_dict_encrypt(context, pszSrc[index], currentDict);
            index++;
        }
    }
    
}
void yx_core_encrypt_randomdict_decrypt(yx_core_encrypt_randomdict handle, yx_uchar8* pszDst, const yx_uchar8* pszSrc, yx_size len){
    assert(NULL != pszDst);
    assert(NULL != pszSrc);
    assert(NULL != handle);
    
    _random_dict_encry_context_ref context = random_dict_handle2contextRef(handle);
    yx_int dictNum = context->dictNum;
    
    yx_int index = 0;
    while (index<len){
        for(yx_int currentDict = 0; index<len && currentDict<dictNum; currentDict++){
            pszDst[index] = random_dict_decrypt(context, pszSrc[index], currentDict);
            index++;
        }
    }
    
}






static yx_uint32 _nativeKeyIterator (yx_uint32 *key)
{
    
    assert(sizeof(yx_uint32) == 4);
    
    yx_uint32 next = *key;
    yx_uint32 nexKey;
    
    next *= 1103515245; //10 bit
    next += 12345; //5 bit Aha, ha~~ha~~~~
    nexKey = (yx_uint32) (next / 65536) % 2048;
    
    next *= 1103515245;
    next += 12345;
    nexKey <<= 10;
    nexKey ^= (yx_uint32) (next / 65536) % 1024;
    
    next *= 1103515245;
    next += 12345;
    nexKey <<= 10;
    nexKey ^= (yx_uint32) (next / 65536) % 1024;
    
    *key = next;
    
    return nexKey;
}



static void _initDict(_random_dict_encry_context_ref context, yx_int tableNum){
    
    yx_char8 flag[256];
    memset(flag, 0, 256);
    
    for(yx_int i=0; i<256; i++){
        
        _nativeKeyIterator(&(context->currentKey));
        yx_uchar8 code = (yx_uchar8)(context->currentKey % 256);
        while (0 != flag[code])
            code = abs((code+1) % 256);
        
        flag[code] = 1;
        random_dict_setEncrypt(context, tableNum, i, code);
        random_dict_setDecrypt(context, tableNum, code, i);
    }
    
}













