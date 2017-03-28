//
//  hash.c
//  TestHas
//
//  Created by Yuxi Liu on 4/15/12.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

#include "PrimeTable.h"
#include "hash.h"


const unsigned long max_hash_size = 0X100000; //1M


typedef enum {Occupied, Empty, Deleted} EntryStatus;

typedef struct _Hsh_Entry
{
    int key;
    EntryStatus state;
    unsigned long data;
}Hsh_Entry, * Hsh_EntryRef;

typedef Hsh_Entry* HshEntryArray;
typedef struct Hsh_Tble_impl
{
    size_t max_size;
    size_t used;
    HshEntryArray array;
#ifdef DEBUG
    CM_BOOL fDebug;
    FILE* pFile; //will output the debug msg to this file stream.
#endif //DEBUG
}Hsh_Tble_impl, * Hsh_Tbl_implRef;



#ifdef DEBUG
Hsh_Tble HSH_InitTable(size_t maxSize, CM_BOOL fDebug, FILE* pFile)
#else
Hsh_Tble HSH_InitTable(size_t maxSize)
#endif //DEBUG
{
    if(maxSize > max_hash_size)
        return NULL;

    
    Hsh_Tbl_implRef pHshTbl = NULL;
    CM_BOOL fSuccess = CM_FALSE;
    size_t tableSize = getPrimeSize(maxSize);
    
    do {
        
        if( NULL == (pHshTbl = (Hsh_Tbl_implRef)malloc( sizeof(Hsh_Tble_impl) )) )
            break;
        pHshTbl->max_size = tableSize;
        pHshTbl->used = 0;
        
#ifdef DEBUG
        pHshTbl->fDebug = fDebug;
        pHshTbl->pFile = (NULL == pFile? stdout : pFile);
#endif //DEBUG
        
        
        if( NULL == (pHshTbl->array = (Hsh_EntryRef)malloc( sizeof(Hsh_Entry) * tableSize )) )
            break;
        
        for(unsigned long i=0; i<tableSize; i++)
            pHshTbl->array[i].state = Empty;
        
        
        fSuccess = CM_TRUE;
    } while (0);
    
    
    if(fSuccess == CM_FALSE && NULL != pHshTbl)
        HSH_FreeTable((Hsh_Tble*)&pHshTbl);

    
    return (fSuccess? pHshTbl : NULL);
}


void HSH_FreeTable(Hsh_Tble* ptTable)
{
    if(NULL == ptTable)
        return;
    
    Hsh_Tbl_implRef pHshTbl = (Hsh_Tbl_implRef)(*ptTable);
    
    if(NULL != pHshTbl->array)
        free(pHshTbl->array);
    
    free(pHshTbl);
    
    *ptTable = NULL;
}


CM_BOOL HSH_Insert(Hsh_Tble table, int key, unsigned long data)
//CM_BOOL HSH_Insert(Hsh_Tble table, int val)
{
    Hsh_Tbl_implRef pHshTbl = (Hsh_Tbl_implRef)table;
    CM_BOOL fSuccess = CM_FALSE;
    
    
    if(NULL == pHshTbl)
        return CM_FALSE;
    
    if(pHshTbl->used >= pHshTbl->max_size)
        return CM_FALSE;
    

    unsigned long pos = (unsigned int)(abs(key) % pHshTbl->max_size);
    unsigned long firstDetPos = pos;
    
    for(unsigned int i=0;;i++)
    {
        if(pHshTbl->array[pos].state != Occupied) //empty or deleted
        {
            pHshTbl->array[pos].state = Occupied;
            pHshTbl->array[pos].key = key;
            pHshTbl->array[pos].data = data;
            pHshTbl->used += 1;
            fSuccess = CM_TRUE;
            break;
        }
        
#if DEBUG
        if(pHshTbl->fDebug)
        {
            fprintf(pHshTbl->pFile, "\tCollision occurred saving item with key-val(%d-%ld) at hash table location %ld\n", key, (unsigned long)data, pos);
            //fflush(stdout);
        }
#endif //DEBUG

        
        pos = pos + (i * i) - 1;
        if(pos >= (pHshTbl->max_size - 1))
           pos = pos % (pHshTbl->max_size);
        
        if(firstDetPos == pos)
            break;

    }
    
    
    
    return fSuccess;
}


static int _findByKey(Hsh_Tbl_implRef pHshTbl, int key, unsigned long* rtVal, CM_BOOL fRemove)
{
    CM_BOOL fSuccess = CM_FALSE;
    
    
    if(NULL == pHshTbl)
        return CM_FALSE;
    
    unsigned long pos = (unsigned long)(abs(key) % pHshTbl->max_size);
    
    unsigned long firstDetPos = pos;
    for(int i=0;;i++)
    {
        if(pHshTbl->array[pos].state == Empty)
            break;
        
        if(i > pHshTbl->max_size)
            break;
        
        if(pHshTbl->array[pos].state == Occupied && pHshTbl->array[pos].key == key)
        {
            
            if(NULL != rtVal)
                *rtVal = pHshTbl->array[pos].data;
            
            if(fRemove){
                pHshTbl->array[pos].state = Deleted;
                pHshTbl->used--;
            }
            
            fSuccess = CM_TRUE;
            break;
        }
        
        pos = pos + (i * i) - 1;
        if(pos >= (pHshTbl->max_size - 1))
            pos = pos % (pHshTbl->max_size);
        
        if(firstDetPos == pos)
            break;
    }
    
    return fSuccess == CM_TRUE ? (int)pos : -1;
}


CM_BOOL HSH_Find(Hsh_Tble table, int key, unsigned long* val)
{
    Hsh_Tbl_implRef pHshTbl = (Hsh_Tbl_implRef)table;
    
    if(NULL == pHshTbl)
        return CM_FALSE;
    
    int pos =  _findByKey(pHshTbl, key, val, CM_FALSE);
    
#if DEBUG
    if(pHshTbl->fDebug)
    {
        if(pos >= 0)
            fprintf(pHshTbl->pFile, "\tKey %d found in table at location %d\n", key, pos);
        else
            fprintf(pHshTbl->pFile, "\tKey %d not found in table\n", key);
        //fflush(stdout);
    }
#endif //DEBUG
    
    return pos!=-1 ? CM_TRUE : CM_FALSE;
}


//:~?
CM_BOOL HSH_IsExist(Hsh_Tble table, int key)
{
    Hsh_Tbl_implRef pHshTbl = (Hsh_Tbl_implRef)table;
    
    if(NULL == pHshTbl)
        return CM_FALSE;
    
    
    int pos = _findByKey(pHshTbl, key, NULL, CM_FALSE);
    
#if DEBUG
    if(pHshTbl->fDebug)
    {
        if(pos >= 0)
            fprintf(pHshTbl->pFile, "\tKey %d found in table at location %d\n", key, pos);
        else
            fprintf(pHshTbl->pFile, "\tKey %d not found in table\n", key);
        //fflush(stdout);
    }
#endif //DEBUG
    
    return (pos >= 0);
}

//:~?
CM_BOOL HSH_Remove(Hsh_Tble table, int key)
{
    Hsh_Tbl_implRef pHshTbl = (Hsh_Tbl_implRef)table;
    
    if(NULL == pHshTbl)
        return CM_FALSE;
    
    return ( _findByKey(pHshTbl, key, NULL, CM_TRUE) >= 0 );
}


size_t HSH_Size(Hsh_Tble table)
{
    Hsh_Tbl_implRef pHshTbl = (Hsh_Tbl_implRef)table;
    
    if(NULL == pHshTbl)
        return 0;
    
    return pHshTbl->used;
}

size_t HSH_MAXSIZE(Hsh_Tble table)
{
    Hsh_Tbl_implRef pHshTbl = (Hsh_Tbl_implRef)table;
    
    if(NULL == pHshTbl)
        return 0;
    
    return pHshTbl->max_size;
}


void HSH_Clear(Hsh_Tble table)
{
    Hsh_Tbl_implRef pHshTbl = (Hsh_Tbl_implRef)table;
    if(NULL == pHshTbl)
        return;
    
    pHshTbl->used = 0;
    
    unsigned int len = (unsigned int)(pHshTbl->max_size);
    for(unsigned int i=0; i<len; i++)
        pHshTbl->array[i].state = Empty;
    
    
}


Hsh_TbleIter HSH_Iterator(Hsh_Tble table, Hsh_TbleIter it, Hsh_Pair_Ref pair){
    //!!!! hi, I use a int value as the iterator handle. it's not a pointer
    unsigned int pos = (NULL!=it ? (unsigned int)(unsigned long)it : 0);
    
    assert(NULL != pair);
    assert(NULL != table);
    
    Hsh_Tbl_implRef pHshTbl = (Hsh_Tbl_implRef)table;
    
    Hsh_TbleIter nexIt = NULL;
    
    int max_size = (int)(pHshTbl->max_size);
    for(int i=pos; i<max_size; i++)
    {
        if(pHshTbl->array[i].state == Occupied)
        {
            if(NULL != pair)
            {
                pair->key = pHshTbl->array[i].key;
                pair->data = pHshTbl->array[i].data;
            }
            
            
            //calculat the next pos.
            //if there is no next pos, return NULL;
            for(int j=i+1; j<max_size; j++)
                if(pHshTbl->array[j].state == Occupied){
                    nexIt = (void*)j;
                    break;
                }
            
            break;
        }
    }
    
    return nexIt;
}



#ifdef DEBUG
void HSH_Walk(Hsh_Tble table)
{

    Hsh_Tbl_implRef pHshTbl = (Hsh_Tbl_implRef)table;
    if(NULL == pHshTbl)
    {
        printf("Error :Invalid table\n");
        return;
    }
    
//    if(pHshTbl->fDebug == CM_FALSE)
//        return;
//    
    unsigned int len = (unsigned int)(pHshTbl->max_size);
    for(unsigned int i=0; i<len; i++)
    {
        switch (pHshTbl->array[i].state)
        {
            case Empty:
                printf("position: %4d \t|state :  Empty    \t|               |\n", i);
                break;
            case Deleted:
                printf("position: %4d \t|state :  Delete   \t|oldkey :%4d \t|\n", i, pHshTbl->array[i].key);
                break;
            case Occupied:
                printf("position: %4d \t|state :  Occupied \t|key  :%4d \t|FirstDetPos :%4d\n", i, pHshTbl->array[i].key, (unsigned int)(abs(pHshTbl->array[i].key) % pHshTbl->max_size));
                break;
        }
    }
}
#endif //DEBUG





