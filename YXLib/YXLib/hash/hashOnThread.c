//
//  hashOnThread.c
//  hash
//
//  Created by Yuxi Liu on 10/22/12.
//  Copyright (c) 2012 Yuxi Liu. All rights reserved.
//

#include <stdio.h>
#include <memory.h>
#include <assert.h>

#include <pthread.h>

#include "../cm/cmMem.h"

#include "hashOnThread.h"


#define ThreadSafeHashHandle2Impl(h) ((_thread_hash_table_impl_ref)h)

typedef struct{
    Hsh_Tble hashTable;
    pthread_rwlock_t rwl;
}_thread_hash_table_impl, *_thread_hash_table_impl_ref;



#ifdef DEBUG
HTSH_Tble HTSH_InitTable(size_t maxSize, CM_BOOL fDebug, FILE* pFile){
#else
HTSH_Tble HTSH_InitTable(size_t maxSize){
#endif //DEBUG
    
    
    _thread_hash_table_impl_ref context = NULL;
    CM_BOOL isRWLInit = CM_FALSE;
    Hsh_Tble hashTable = NULL;
    
    
    if(NULL == (context = (_thread_hash_table_impl_ref) MALLOC(sizeof(_thread_hash_table_impl))))
        goto outerr;
    
    if(0 != pthread_rwlock_init(&(context->rwl), NULL))
        goto outerr;
    isRWLInit = CM_TRUE;
    
    
    
#ifdef DEBUG
    if(NULL == (hashTable = HSH_InitTable(maxSize, fDebug, pFile)))
        goto outerr;
#else
    if(NULL == (hashTable = HSH_InitTable(maxSize)))
        goto outerr;
    
#endif //DEBUG
    
    
    context->hashTable = hashTable;
    
    
    return (HTSH_Tble)context;
    
outerr:
    if(NULL != context){
        
        if(isRWLInit == CM_TRUE)
            pthread_rwlock_destroy(&(context->rwl));
        
        FREE(context);
    }
    
    if(NULL != hashTable)
        HSH_FreeTable(&hashTable);
    
    
    return NULL;
}



void HTSH_FreeTable(HTSH_Tble* ptTable){
    _thread_hash_table_impl_ref context = ThreadSafeHashHandle2Impl(*ptTable);
    *ptTable = NULL;
    
    
    //release the hash table
    pthread_rwlock_wrlock(&(context->rwl));
    HSH_FreeTable(&(context->hashTable));
    context->hashTable = NULL;
    pthread_rwlock_unlock(&(context->rwl));
    
    //destroy the rw lock
    pthread_rwlock_destroy(&(context->rwl));
    
    
    FREE(context);
}
    

CM_BOOL HTSH_Find(HTSH_Tble table, int key, unsigned long* val)
{
    
    _thread_hash_table_impl_ref context = ThreadSafeHashHandle2Impl(table);
    assert(NULL != context);
    
    int rtVal;
    pthread_rwlock_rdlock(&(context->rwl));
    rtVal = HSH_Find(context->hashTable, key, val);
    pthread_rwlock_unlock(&(context->rwl));

    
    return rtVal;
}

    
CM_BOOL HTSH_Insert(HTSH_Tble table, int key, unsigned long data)
{
    _thread_hash_table_impl_ref context = ThreadSafeHashHandle2Impl(table);
    assert(NULL != context);
    
    CM_BOOL rtVal;
    pthread_rwlock_wrlock(&(context->rwl));
    rtVal = HSH_Insert(context->hashTable, key, data);
    pthread_rwlock_unlock(&(context->rwl));
    return rtVal;
}


    
CM_BOOL HTSH_IsExist(HTSH_Tble table, int key)
{
    _thread_hash_table_impl_ref context = ThreadSafeHashHandle2Impl(table);
    assert(NULL != context);
    
    CM_BOOL rtVal;
    pthread_rwlock_rdlock(&(context->rwl));
    rtVal = HSH_IsExist(context->hashTable, key);
    pthread_rwlock_unlock(&(context->rwl));
    
    return rtVal;
}

    
size_t HTSH_Size(HTSH_Tble table)
{
    _thread_hash_table_impl_ref context = ThreadSafeHashHandle2Impl(table);
    assert(NULL != context);
    
    size_t rtVal;
    pthread_rwlock_rdlock(&(context->rwl));
    rtVal = HSH_Size(context->hashTable);
    pthread_rwlock_unlock(&(context->rwl));
    
    return rtVal;
}
    
size_t HTSH_MAXSIZE(HTSH_Tble table)
{
    _thread_hash_table_impl_ref context = ThreadSafeHashHandle2Impl(table);
    assert(NULL != context);
    
    size_t rtVal;
    pthread_rwlock_rdlock(&(context->rwl));
    rtVal = HSH_MAXSIZE(context->hashTable);
    pthread_rwlock_unlock(&(context->rwl));
    
    return rtVal;
}
    
    
void HTSH_Clear(HTSH_Tble table)
{
    _thread_hash_table_impl_ref context = ThreadSafeHashHandle2Impl(table);
    assert(NULL != context);
    
    
    pthread_rwlock_wrlock(&(context->rwl));
    HSH_Clear(context->hashTable);
    pthread_rwlock_unlock(&(context->rwl));
}


CM_BOOL HTSH_Remove(HTSH_Tble table, int key)
{
    _thread_hash_table_impl_ref context = ThreadSafeHashHandle2Impl(table);
    assert(NULL != context);
    
    CM_BOOL rtVal;
    pthread_rwlock_wrlock(&(context->rwl));
    rtVal = HSH_Remove(context->hashTable, key);
    pthread_rwlock_unlock(&(context->rwl));
    
    return rtVal;
}

    
void HTSH_BeginIteratorForRead(HTSH_Tble table)
{
    _thread_hash_table_impl_ref context = ThreadSafeHashHandle2Impl(table);
    assert(NULL != context);
    
    pthread_rwlock_rdlock(&(context->rwl));
}
void HTSH_BeginIteratorForWrite(HTSH_Tble table)
{
    _thread_hash_table_impl_ref context = ThreadSafeHashHandle2Impl(table);
    assert(NULL != context);
    
    pthread_rwlock_wrlock(&(context->rwl));
}
void HTSH_EndIterator(HTSH_Tble table)
{
    _thread_hash_table_impl_ref context = ThreadSafeHashHandle2Impl(table);
    assert(NULL != context);
    
    pthread_rwlock_unlock(&(context->rwl));
}
HTSH_TbleIter HTSH_Iterator(HTSH_Tble table, HTSH_TbleIter it, Hsh_Pair_Ref pair)
{
    _thread_hash_table_impl_ref context = ThreadSafeHashHandle2Impl(table);
    assert(NULL != context);
    
    return (HTSH_TbleIter)HSH_Iterator(context->hashTable, (Hsh_TbleIter)it, pair);
}
    
    
    

#ifdef DEBUG
void HTSH_Walk(HTSH_Tble table)
{
    _thread_hash_table_impl_ref context = ThreadSafeHashHandle2Impl(table);
    assert(NULL != context);
    
    
    pthread_rwlock_rdlock(&(context->rwl));
    HSH_Walk(context->hashTable);
    pthread_rwlock_unlock(&(context->rwl));
}
#endif //DEBUG



