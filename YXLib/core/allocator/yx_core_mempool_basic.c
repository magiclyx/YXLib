//
//  yx_mempool_basic.c
//  YXLib
//
//  Created by Yuxi Liu on 8/29/13.
//  Copyright (c) 2013 Yuxi Liu. All rights reserved.
//



#include "yx_core_mempool_basic.h"

#include "yx_core_mem.h"
#include "../rttidef/yx_core_rttidef.h"
#include "../debug/yx_core_assert.h"
#include "../yx_core_bit.h"



/*mem header*/
#pragma mark - struct mem header
////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 mem type flag (16 bit)
 */
#define YX_MEMPOOL_BASICMEM_TYPE_LARGE    1
#define YX_MEMPOOL_BASICMEM_TYPE_ALIGN    2

/*large slot*/
#pragma mark - struct larg slot
////////////////////////////////////////////////////////////////////////////////////////////////////
struct __yx_memPool_basic_largeBlock_s{
    struct __yx_memPool_basic_largeBlock_s* next;
    struct __yx_memPool_basic_largeBlock_s* prev;
    void* alloc;
};


struct __yx_memPool_largeSlot_s{
    struct __yx_memPool_basic_largeBlock_s* block;
};




/*slot*/
#pragma mark - struct slot
////////////////////////////////////////////////////////////////////////////////////////////////////
struct __yx_memPool_basic_slotFreeList_node_s{
    struct __yx_memPool_basic_slotFreeList_node_s* next;
};

YX_STATIC_ASSERT((sizeof(struct __yx_memPool_basic_slotFreeList_node_s) <= YX_ALIGNMENT)); //limited the note's size


struct __yx_memPool_basic_slotBlock_s{
    struct __yx_memPool_basic_slotBlock_s* next;
    YX_STRUCT_ALIGNMENT_INFILLING(sizeof(struct __yx_memPool_basic_slotBlock_s*));
};

YX_STATIC_ASSERT_ALIGMENT(struct __yx_memPool_basic_slotBlock_s);



struct __yx_memPool_slot_s{
    struct __yx_memPool_basic_slotBlock_s* block;
    struct __yx_memPool_basic_slotFreeList_node_s* freeList;
};



#pragma mark - headers
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

//every mem has this header
struct __yx_memPool_basic_memHeader_s{
    yx_uint32 flag;
    YX_STRUCT_ALIGNMENT_INFILLING(sizeof(yx_uint32));
};

YX_STATIC_ASSERT_ALIGMENT(struct __yx_memPool_basic_memHeader_s);


//appened when alloc the memory from small slut and assigned a custom alignment
struct __yx_memPool_basic_memHeader_alignment_s{
    void* pt;
    YX_STRUCT_ALIGNMENT_INFILLING(sizeof(void*));
};

YX_STATIC_ASSERT_ALIGMENT(struct __yx_memPool_basic_memHeader_alignment_s);

//appened when alloc the memory from the large slut. 
struct __yx_memPool_basic_memHeader_large_s{
    struct __yx_memPool_basic_largeBlock_s* onwer_block;
    YX_STRUCT_ALIGNMENT_INFILLING(sizeof(struct __yx_memPool_basic_largeBlock_s*));
};

YX_STATIC_ASSERT_ALIGMENT(struct __yx_memPool_basic_memHeader_large_s);

/*pool*/
#pragma mark struct pool
////////////////////////////////////////////////////////////////////////////////////////////////////
struct __yx_memPool_basicPool_s{
    YX_ALLOCATOR_STRUCT_DECLARE;
    yx_allocator allocator;
    
    struct __yx_memPool_largeSlot_s largeSlot[YX_MEMPOOL_BASIC_LARGERMEMORY_BITSUPPORT]; //这里可以优化，因为对于这张表来说，前面几位总是空的。因为这是大内存池
    struct __yx_memPool_slot_s slot[YX_MEMPOOL_BASIC_SMALLMEMORY_SLOTNUM];
};



/*macro*/
#pragma mark - macro
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

#define HANDLE2BASICMEMPOOL(h)  ((struct __yx_memPool_basicPool_s*)h)  //convert handle to pool

//pack 16bit index and 16bit type into a 32bit flag
#define __YX_MEMPOOL_BASIC_PACK_(index, type) (index << 16 | type)
#define __YX_MEMPOOL_BASIC_INDEX_FROM_PACK_(flag) (flag >> 16)
#define __YX_MEMPOOL_BASIC_TYPE_FROM_PACK_(flag) (flag & 65535)

// the size of every slot is 2 * YX_ALIGNMENT. so, the index is ....
#if (4 == YX_ALIGNMENT)
#define YX_MEMPOOL_BASIC_SLOT_INDEX(size) (size >> 3)
#define YX_MEMPOOL_BASIC_SLOT_SIZE(index) ((index+1) << 3) //index start from 0, so need to add 1
#define YX_MEMPOOL_BASIC_LARGSLOG_INDEX(size) yx_value32_max_bit_len(size)
#elif (8 == YX_ALIGNMENT)
//#define YX_MEMPOOL_BASIC_SLOT_INDEX(size) (size >> 4) + ((size & 15)? 0 : -1)
#define YX_MEMPOOL_BASIC_SLOT_INDEX(size) (size >> 4)
#define YX_MEMPOOL_BASIC_SLOT_SIZE(index) ((index+1) << 4) //index start from 0, so need to add 1
#define YX_MEMPOOL_BASIC_LARGSLOG_INDEX(size) yx_value64_max_bit_len(size)
#endif



#pragma mark private fun declare
/*declare private function*/
static yx_int _yx_memPool_basic_slotExtend_num(yx_int slotIndex);


static void* _yx_memPool_basic_slotAlloc(struct __yx_memPool_basicPool_s* poolContext, struct __yx_memPool_slot_s* slot, yx_int slotIndex);
static void _yx_memPool_basic_slotFree(struct __yx_memPool_slot_s* slot, void* address);
static void _yx_memPool_basic_slotDestory(struct __yx_memPool_basicPool_s* poolContext, struct __yx_memPool_slot_s* slot);
static struct __yx_memPool_basic_slotBlock_s* _yx_memPool_basic_slotBlockCreate(struct __yx_memPool_basicPool_s* poolContext, yx_size cellSize, yx_size cellNum);



static struct __yx_memPool_basic_largeBlock_s* _yx_memPool_basic_largeBlockAlloc(struct __yx_memPool_basicPool_s* poolContext, yx_size  size);
static void _yx_memPool_basic_largeBlockFree(struct __yx_memPool_basicPool_s* poolContext, struct __yx_memPool_basic_largeBlock_s* largeBlock);
static void _yx_memPool_basic_largeSlotDestory(struct __yx_memPool_basicPool_s* poolContext, struct __yx_memPool_largeSlot_s* largeSlot);


/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////



yx_allocator yx_basicMempool_create(yx_allocator allocator){
    struct __yx_memPool_basicPool_s  *context;
    
    if(NULL == allocator)
        allocator = yx_get_allocator();
    
    /*alloc the mem of the structure*/
    context = yx_allocator_alloc(allocator, sizeof(struct __yx_memPool_basicPool_s));
    if (yx_unlikely( context == NULL )) {
        return NULL;
    }
    
    for(yx_int i=0; i<YX_MEMPOOL_BASIC_SMALLMEMORY_SLOTNUM; i++){
        context->slot[i].block = NULL;
        context->slot[i].freeList = NULL;
    }
    
    for(yx_int i=0; i<YX_MEMPOOL_BASIC_LARGERMEMORY_BITSUPPORT; i++){
        context->largeSlot[i].block = NULL;
    }
    
    
    /*setup the alocator*/
    context->allocator = allocator;
    yx_allocator_setup(yx_rtti_allocator_basic, context, yx_basicMempool_alloc, yx_basicMempool_memalign, yx_basicMempool_free);
    
    return (yx_allocator)context;
}


void yx_basicMempool_destroy(yx_allocator* allocator_ptr){
    
    YX_ASSERT(NULL != allocator_ptr);
    YX_ASSERT(NULL != *allocator_ptr);
    
    
    struct __yx_memPool_basicPool_s* poolContext = HANDLE2BASICMEMPOOL(*allocator_ptr);
    *allocator_ptr = NULL;
    
    
    //check the memory leak
#if YX_BASICALLOC_DEBUG
    YX_ASSERT(yx_true == isSmallSlutEmpty(poolContext));
    YX_ASSERT(yx_true == isLargeSlutEmpty(poolContext));
#endif
    
    
    /*
     the large block mantain struct "__yx_memPool_basic_largeBlock_s" is also allocked from the pool.
     so, the large block must be released before the pool released!!!!!
     */
    
    /*release the large slot*/
    for(yx_int i=0; i<YX_MEMPOOL_BASIC_LARGERMEMORY_BITSUPPORT; i++){
        //actually, the largeSlotList should be empty, otherwise, it's memleak.
        _yx_memPool_basic_largeSlotDestory(poolContext, &(poolContext->largeSlot[i]));
    }
    
    /*release all slot*/
    for(yx_int i=0; i<YX_MEMPOOL_BASIC_SMALLMEMORY_SLOTNUM; i++){
        _yx_memPool_basic_slotDestory(poolContext, &(poolContext->slot[i]));
    }
    
    
    /*release the pool maintain structre*/
    yx_allocator_free(poolContext->allocator, poolContext);
}


void* yx_basicMempool_alloc(yx_allocator allocator, yx_size size)
{
    
    YX_ASSERT(NULL != allocator);
    
    
    void* mem;
    
    struct __yx_memPool_basicPool_s* poolContext = HANDLE2BASICMEMPOOL(allocator);
    
    
    const yx_size guassSlotTotalSize = (sizeof(struct __yx_memPool_basic_memHeader_s) + size);
    yx_int slotIndex = (yx_int)YX_MEMPOOL_BASIC_SLOT_INDEX(guassSlotTotalSize);
    if(yx_likely( slotIndex < YX_MEMPOOL_BASIC_SMALLMEMORY_SLOTNUM ))
    {
        
        
        struct __yx_memPool_basic_memHeader_s* header = _yx_memPool_basic_slotAlloc(poolContext, &(poolContext->slot[slotIndex]), slotIndex);
        if(yx_unlikely( !header ))
            return NULL;
        
        /*set the header info*/
        header->flag = __YX_MEMPOOL_BASIC_PACK_((yx_uint32)slotIndex, 0);

        
        /*get the mem*/
        mem = ((yx_byte*)header) + sizeof(struct __yx_memPool_basic_memHeader_s);
    }
    else
    {
        const yx_size largeSlotTotalSize = (sizeof(struct __yx_memPool_basic_largeBlock_s) + sizeof(struct __yx_memPool_basic_memHeader_s) + size);
        const yx_int largeBlockIndex = YX_MEMPOOL_BASIC_LARGSLOG_INDEX(largeSlotTotalSize);
        
        
        //large mem
        struct __yx_memPool_basic_largeBlock_s* newLargerBlock = _yx_memPool_basic_largeBlockAlloc(allocator, largeSlotTotalSize);
        if(yx_unlikely( !newLargerBlock ))
            return NULL;
        
        mem = newLargerBlock->alloc;
        
        
        /*setup large block header*/
        struct __yx_memPool_basic_memHeader_large_s* largeblock_header = mem;
        largeblock_header->onwer_block = newLargerBlock;
        
        /*setup the mem header*/
        /*set the header's index to -1 .*/
        mem += sizeof(struct __yx_memPool_basic_memHeader_large_s);
        struct __yx_memPool_basic_memHeader_s* heade = mem;
        heade->flag = __YX_MEMPOOL_BASIC_PACK_((yx_uint32)largeBlockIndex, YX_MEMPOOL_BASICMEM_TYPE_LARGE);
        
        
        /*set the memory*/
        mem += sizeof(struct __yx_memPool_basic_memHeader_s);
        
        
        
        /*link the newLargeMem to the large memory linked list*/
        struct __yx_memPool_basic_largeBlock_s* firstLargeBlock = poolContext->largeSlot[largeBlockIndex].block;
        newLargerBlock->prev = NULL;
        if (NULL != firstLargeBlock)
            firstLargeBlock->prev = newLargerBlock;
        newLargerBlock->next = firstLargeBlock;
        poolContext->largeSlot[largeBlockIndex].block = newLargerBlock;
        
        
        
    }
    
    return mem;
}

void* yx_basicMempool_memalign(yx_allocator allocator, yx_size alignment, yx_size size)
{
    
    YX_ASSERT(NULL != allocator);
    
    
    /*if alignment is equal to the YX_ALIGNMENT, allocate the memory from yx_basicMempool_alloc*/
    if (YX_ALIGNMENT == alignment)
        return yx_basicMempool_alloc(allocator, size);
    
    
    
    
    void* mem;
    struct __yx_memPool_basicPool_s* poolContext = HANDLE2BASICMEMPOOL(allocator);
    
    // we need to allocate enough storage for the requested bytes, some
    // book-keeping (to store the location returned by malloc) and some extra
    // padding to allow us to find an aligned byte. im not entirely sure if
    // 2 * alignment is enough here, its just a guess.
    const yx_size totalSize = sizeof(struct __yx_memPool_basic_memHeader_alignment_s) + sizeof(struct __yx_memPool_basic_memHeader_s) + 2*alignment + size;
    
    const yx_int slotIndex = (yx_int)YX_MEMPOOL_BASIC_SLOT_INDEX(totalSize);
    if(slotIndex < YX_MEMPOOL_BASIC_SMALLMEMORY_SLOTNUM)
    {
        
        // store the original start of the malloc'd data.
        void* mem_start = _yx_memPool_basic_slotAlloc(poolContext, &(poolContext->slot[slotIndex]), slotIndex);
        if (yx_unlikely( !mem_start ))
            return NULL;
        
        mem = mem_start;
        
        
        // dedicate enough space to the book-keeping.
        mem += sizeof(struct __yx_memPool_basic_memHeader_alignment_s) + sizeof(struct __yx_memPool_basic_memHeader_s);
        
        // find a memory location with correct alignment. the alignment minus
        // the remainder of this mod operation is how many bytes forward we need
        // to move to find an aligned byte.
        const size_t offset = alignment - (((size_t)mem) % alignment);
        
        
        // set data to the aligned memory.
        mem += offset;
        
        
        
        //setup the common header
        struct __yx_memPool_basic_memHeader_s* header = mem - sizeof(struct __yx_memPool_basic_memHeader_s);
        //set the header info
        header->flag = __YX_MEMPOOL_BASIC_PACK_((yx_uint32)slotIndex, YX_MEMPOOL_BASICMEM_TYPE_ALIGN);
        
        
        /*setup the alignment header*/
        struct __yx_memPool_basic_memHeader_alignment_s* alignment_header = (yx_ptr)header - sizeof(struct __yx_memPool_basic_memHeader_alignment_s);
        alignment_header->pt = mem_start;
        
    }
    else{
        //large mem
        
        struct __yx_memPool_basic_largeBlock_s* newLargerBlock = _yx_memPool_basic_largeBlockAlloc(allocator, totalSize);
        if(yx_unlikely( !newLargerBlock ))
            return NULL;
        
        mem = newLargerBlock->alloc;
        
        
        // dedicate enough space to the book-keeping.
        mem += sizeof(struct __yx_memPool_basic_memHeader_large_s) + sizeof(struct __yx_memPool_basic_memHeader_s);
        
        
        // find a memory location with correct alignment. the alignment minus
        // the remainder of this mod operation is how many bytes forward we need
        // to move to find an aligned byte.
//        const size_t offset = alignment - (((size_t)mem) % alignment);
        // set data to the aligned memory.
//        mem += offset;
        mem = YX_POINT_ALIGNMENT(mem);
        
        
        
        //setup the common header
        struct __yx_memPool_basic_memHeader_s* header = mem - sizeof(struct __yx_memPool_basic_memHeader_s);
        //set the header info
        header->flag = __YX_MEMPOOL_BASIC_PACK_((yx_uint32)slotIndex, YX_MEMPOOL_BASICMEM_TYPE_LARGE | YX_MEMPOOL_BASICMEM_TYPE_ALIGN);
        
        
        /*setup the large block header*/
        struct __yx_memPool_basic_memHeader_large_s* largeblock_header = (yx_ptr)header - sizeof(struct __yx_memPool_basic_memHeader_large_s);
        largeblock_header->onwer_block = newLargerBlock;
        
        
        
        /*link the newLargeMem to the large memory linked list*/
        const yx_int largeBlockIndex = YX_MEMPOOL_BASIC_LARGSLOG_INDEX(totalSize);
        struct __yx_memPool_basic_largeBlock_s* firstLargeBlock = poolContext->largeSlot[largeBlockIndex].block;
        
        firstLargeBlock->prev = newLargerBlock;
        newLargerBlock->next = firstLargeBlock;
        newLargerBlock->prev = NULL;
        poolContext->largeSlot[largeBlockIndex].block = newLargerBlock;
        
    }
    
    return mem;
}


void yx_basicMempool_free(yx_allocator allocator, yx_ptr address){
    
    YX_ASSERT(NULL != allocator);
    YX_ASSERT(NULL != address);
    
    //poolContext
    struct __yx_memPool_basicPool_s* poolContext = HANDLE2BASICMEMPOOL(allocator);
    
    //header
    struct __yx_memPool_basic_memHeader_s* const header = address - sizeof(struct __yx_memPool_basic_memHeader_s);
    
    //type flag
    const yx_uint16 typeFlag = __YX_MEMPOOL_BASIC_TYPE_FROM_PACK_(header->flag);
    
    //index
    const yx_uint32 slot_index = __YX_MEMPOOL_BASIC_INDEX_FROM_PACK_(header->flag);
    
    
    
    if (YX_MEMPOOL_BASICMEM_TYPE_LARGE & typeFlag)
    {
        
        struct __yx_memPool_basic_memHeader_large_s* largeblock_header = (yx_ptr)header - sizeof(struct __yx_memPool_basic_memHeader_large_s);
        struct __yx_memPool_basic_largeBlock_s* largeBlock = largeblock_header->onwer_block;
        
        if (NULL != largeBlock->prev) {
            largeBlock->prev->next = largeBlock->next;
            
            if (NULL != largeBlock->next)
            {
                largeBlock->next->prev = largeBlock->prev;
            }
        }
        else
        {
            struct __yx_memPool_largeSlot_s* largeSlot = &(poolContext->largeSlot[slot_index]);
            largeSlot->block = largeBlock->next;
            
            if (NULL != largeBlock->next) {
                largeBlock->next->prev = NULL;
            }
        }
        
        _yx_memPool_basic_largeBlockFree(poolContext, largeBlock);
        
    }
    else
    {
        
        void* mem;
        if (YX_MEMPOOL_BASICMEM_TYPE_ALIGN & typeFlag)
        {
            //alignment header
            const struct __yx_memPool_basic_memHeader_alignment_s* alignment_header = (struct __yx_memPool_basic_memHeader_alignment_s*)(header - sizeof(struct __yx_memPool_basic_memHeader_alignment_s));
            
            mem = alignment_header->pt;
        }
        else
        {
            mem = header;
        }
        
        
        _yx_memPool_basic_slotFree(&(poolContext->slot[slot_index]), mem);
    }
    
    
}




/**************************************************************************************************/
/*private*/
#pragma mark private
/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
static yx_int _yx_memPool_basic_slotExtend_num(yx_int slotIndex){
    return slotIndex < 64 ? 30 : (slotIndex < 512 ? 10 : 5);
}


static void* _yx_memPool_basic_slotAlloc(struct __yx_memPool_basicPool_s* poolContext, struct __yx_memPool_slot_s* slot, yx_int slotIndex){
    
    const struct __yx_memPool_basic_slotFreeList_node_s* mem;
    
    /*if no new __yx_memPool_free_cell_nodes_s in freeList, create a new block*/
    if(! (slot->freeList) ){
        
        /*get the slot Extend num*/
        const yx_int slotExtendNum = _yx_memPool_basic_slotExtend_num(slotIndex);
        
        /*create new block*/
        struct __yx_memPool_basic_slotBlock_s* newBolck = _yx_memPool_basic_slotBlockCreate(poolContext, (yx_size)(YX_MEMPOOL_BASIC_SLOT_SIZE(slotIndex)), (yx_size)slotExtendNum);
        if(yx_unlikely( !newBolck ))
            return NULL;
        
        /*link the bolck to slot*/
        newBolck->next = slot->block;
        slot->block = newBolck;
        
        /*link the new freeList to slot->freeList*/
        slot->freeList = (struct __yx_memPool_basic_slotFreeList_node_s*)( ((yx_byte*)newBolck) + sizeof(struct __yx_memPool_basic_slotBlock_s) );
    }
    
    /*the first node in freeList is the new memory*/
    mem = slot->freeList;
    /*remove the first node from the freeList*/
    slot->freeList = mem->next;
    
    
    return (void*)mem;
}


static void _yx_memPool_basic_slotFree(struct __yx_memPool_slot_s* slot, void* address){
    struct __yx_memPool_basic_slotFreeList_node_s* cell = address;
    
    cell->next = slot->freeList;
    slot->freeList = cell;
}

static void _yx_memPool_basic_slotDestory(struct __yx_memPool_basicPool_s* poolContext, struct __yx_memPool_slot_s* slot){
    struct __yx_memPool_basic_slotBlock_s* block;
    
    block = slot->block;
    while (NULL != block) {
        struct __yx_memPool_basic_slotBlock_s* tmpBlock = block;
        block = block->next;
        yx_allocator_free(poolContext->allocator, tmpBlock);
    }
}

static struct __yx_memPool_basic_slotBlock_s* _yx_memPool_basic_slotBlockCreate(struct __yx_memPool_basicPool_s* poolContext, yx_size cellSize, yx_size cellNum){
    const void* buff;
    const yx_byte* mem;
    struct __yx_memPool_basic_slotBlock_s* slotBlock;
    
    //cellSize must larger or equal to YX_BASICMEMPOOL_MIN_SIZE
    YX_ASSERT(cellSize >= YX_ALIGNMENT);
    //cellSize must alignment on NGX_POOL_ALIGNMENT
    YX_ASSERT(0 == cellSize % YX_ALIGNMENT);
    
    
    buff = yx_allocator_alloc(poolContext->allocator, sizeof(struct __yx_memPool_basic_slotBlock_s) + cellSize*cellNum);
    if(yx_unlikely( !buff ))
        return NULL;
    
    slotBlock = (struct __yx_memPool_basic_slotBlock_s*)buff;
    mem = buff + sizeof(struct __yx_memPool_basic_slotBlock_s);
    
    
    /*init the slotBlock*/
    slotBlock->next = NULL;
    
    /*init the cell freeCell in blockBuff*/
    struct __yx_memPool_basic_slotFreeList_node_s* currentCell = (struct __yx_memPool_basic_slotFreeList_node_s*)mem;
    //    currentCell->pre = NULL;
    //i start from 1. because the first cell is the first currentCell, we just need to find another (cellNum - 1) cell.
    for(yx_size i=1; i<cellNum; i++){
        struct __yx_memPool_basic_slotFreeList_node_s* newCell = (struct __yx_memPool_basic_slotFreeList_node_s*)(((yx_byte*)currentCell) + cellSize);
        
        currentCell->next = newCell;
        //        newCell->pre = currentCell;
        newCell->next = NULL;
        
        currentCell = newCell;
    }
    
    return slotBlock;
}

static struct __yx_memPool_basic_largeBlock_s* _yx_memPool_basic_largeBlockAlloc(struct __yx_memPool_basicPool_s* poolContext, yx_size  size){
    /*alloc the large memory directly*/
    void* mem = yx_allocator_alloc(poolContext->allocator, size);
    if(yx_unlikely( !mem ))
        return NULL;
    
    /*alloc the __yx_mempool_largeMem_s from the pool*/
    struct __yx_memPool_basic_largeBlock_s* newLargeBlock = yx_basicMempool_alloc(poolContext, sizeof(struct __yx_memPool_basic_largeBlock_s));
    if(yx_unlikely( !newLargeBlock )){
        yx_allocator_free(poolContext->allocator, mem);
        return NULL;
    }
    
    /*set new buff to large->alloc*/
    newLargeBlock->alloc = mem;
    
    return newLargeBlock;
}

static void _yx_memPool_basic_largeBlockFree(struct __yx_memPool_basicPool_s* poolContext, struct __yx_memPool_basic_largeBlock_s* largeBlock){
    
    yx_allocator_free(poolContext->allocator, largeBlock->alloc);
    
    /*free the largeBlock structure*/
    yx_basicMempool_free(poolContext, largeBlock);
}

static void _yx_memPool_basic_largeSlotDestory(struct __yx_memPool_basicPool_s* poolContext, struct __yx_memPool_largeSlot_s* largeSlot){
    
    struct __yx_memPool_basic_largeBlock_s* block;
    
    block = largeSlot->block;
    while (NULL != block) {
        yx_allocator_free(poolContext->allocator, block->alloc);
        
        struct __yx_memPool_basic_largeBlock_s* tmpBlock = block;
        block = block->next;
        yx_basicMempool_free(poolContext, tmpBlock);
    }
}






/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
//debug method
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/


#if YX_BASICALLOC_DEBUG

yx_bool isSmallSlutEmpty(yx_handle pool){
    const struct __yx_memPool_basicPool_s* poolContext = HANDLE2BASICMEMPOOL(pool);
    
    yx_bool isEmpty = yx_true;
    
    for(yx_int i=0; i<YX_MEMPOOL_BASIC_SMALLMEMORY_SLOTNUM; i++){
        struct __yx_memPool_slot_s slot = poolContext->slot[i];
        //yx_int slotCellSize = YX_MEMPOOL_BASIC_SLOT_SIZE(i);
        yx_int allMemInSlot;
        yx_int allMemInFreeList;
        
        
        /*count the slot num*/
        yx_int slotblockNum = 0;
        struct __yx_memPool_basic_slotBlock_s* block = slot.block;
        while (block) {
            slotblockNum++;
            block = block->next;
        }
        
        //all mem num in slot is slotExtendNum * slotBlocNum;
        allMemInSlot = _yx_memPool_basic_slotExtend_num(i) * slotblockNum;
        
        
        /*count the mem in free list*/
        struct __yx_memPool_basic_slotFreeList_node_s* node = slot.freeList;
        allMemInFreeList = 0;
        while (NULL != node) {
            allMemInFreeList++;
            node = node->next;
        }
        
        
        
        //verify
        if(allMemInSlot != allMemInFreeList){
            isEmpty = yx_false;
            break;
        }
        
    }
    
    return isEmpty;
}


yx_bool isLargeSlutEmpty(yx_handle pool){
    
    yx_bool isEmpty = yx_true;
    
    const struct __yx_memPool_basicPool_s* poolContext = HANDLE2BASICMEMPOOL(pool);
    for (yx_int i=0; i<YX_MEMPOOL_BASIC_LARGERMEMORY_BITSUPPORT; i++) {
        if(NULL != poolContext->largeSlot[i].block){
            isEmpty = yx_false;
            break;
        }
    }
    
    return isEmpty;
}


#endif
