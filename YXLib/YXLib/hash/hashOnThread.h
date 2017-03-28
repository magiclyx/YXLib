//
//  hashOnThread.h
//  hash
//
//  Created by Yuxi Liu on 10/22/12.
//  Copyright (c) 2012 Yuxi Liu. All rights reserved.
//

//This is a wrapping of the hash funs on thread safe.


#ifndef hash_hashOnThread_h
#define hash_hashOnThread_h

#include "hash.h"


#ifdef __cplusplus
extern "C"{
#endif


typedef void* HTSH_Tble;
typedef void* HTSH_TbleIter;

    
//:~ [v] there is one bug in this module


#ifdef DEBUG
HTSH_Tble HTSH_InitTable(size_t maxSize, CM_BOOL fDebug, FILE* pFile);
#else
HTSH_Tble HTSH_InitTable(size_t maxSize);
#endif //DEBUG

void HTSH_FreeTable(HTSH_Tble* ptTable);



CM_BOOL HTSH_Find(HTSH_Tble table, int key, unsigned long* val);
CM_BOOL HTSH_Insert(HTSH_Tble table, int key, unsigned long data);
CM_BOOL HTSH_IsExist(HTSH_Tble table, int key);
size_t HTSH_Size(HTSH_Tble table);
size_t HTSH_MAXSIZE(HTSH_Tble table);
void HTSH_Clear(HTSH_Tble table);
CM_BOOL HTSH_Remove(HTSH_Tble table, int key);


void HTSH_BeginIteratorForRead(HTSH_Tble table);
void HTSH_BeginIteratorForWrite(HTSH_Tble table);
void HTSH_EndIterator(HTSH_Tble table);
HTSH_TbleIter HTSH_Iterator(HTSH_Tble table, HTSH_TbleIter it, Hsh_Pair_Ref pair);


#ifdef DEBUG
void HTSH_Walk(HTSH_Tble table);
#endif //DEBUG

#ifdef __cplusplus
}
#endif


#endif
