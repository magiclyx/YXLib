/*!
@header hash.h 
@abstract This file implement the storage and retrieval using hashing.
@author Yuxi Liu
@version 1.00 January 5, 2012 Creation
 
 
change: april 2, 2013
 made it more stable and change the structure for new arithmetic
*/


#ifndef TestHas_hash_h
#define TestHas_hash_h


#include "../cm/cmBasicTypes.h"
#include <stdio.h>


#ifdef __cplusplus
extern "C"{
#endif
    
    
//:~ [v] there is two bug in this module
//:~ [v] not implemented


extern const unsigned long max_hash_size;

typedef void* Hsh_Tble;
typedef void* Hsh_TbleIter;

typedef struct _Hsh_Pair{
    int key;
    unsigned long data;
}Hsh_Pair, *Hsh_Pair_Ref;



#if DEBUG
#define HSH_InitTable_Debug(max_size, fDebug, pFile) HSH_InitTable(max_size, fDebug, pFile)
#define HSH_Walk_Debug(table) HSH_Walk(table)
#else
#define HSH_InitTable_Debug(max_size, fDebug, pFile) HSH_InitTable(max_size)
#define HSH_Walk_Debug(table)
#endif //DEBUG




#ifdef DEBUG
Hsh_Tble HSH_InitTable(size_t maxSize, CM_BOOL fDebug, FILE* pFile);
#else
Hsh_Tble HSH_InitTable(size_t maxSize);
#endif //DEBUG



void HSH_FreeTable(Hsh_Tble* ptTable);



CM_BOOL HSH_Find(Hsh_Tble table, int key, unsigned long* val);


//CM_BOOL HSH_Insert(Hsh_Tble table, int val);
CM_BOOL HSH_Insert(Hsh_Tble table, int key, unsigned long data);


CM_BOOL HSH_IsExist(Hsh_Tble table, int key);


size_t HSH_Size(Hsh_Tble table);


size_t HSH_MAXSIZE(Hsh_Tble table);


void HSH_Clear(Hsh_Tble table);


CM_BOOL HSH_Remove(Hsh_Tble table, int key);


Hsh_TbleIter HSH_Iterator(Hsh_Tble table, Hsh_TbleIter it, Hsh_Pair_Ref pair);


#ifdef DEBUG
void HSH_Walk(Hsh_Tble table);
#endif //DEBUG


#ifdef __cplusplus
}
#endif



#endif //include once
