/* 
 *  Copyright 2007
 *  Texas Instruments Incorporated
 *
 *  All rights reserved.  Property of Texas Instruments Incorporated
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 * 
 */
/*
 *  ======== rmm.c ========
 *  Description:
 *
 *  This memory manager provides general heap management one memory segment.
 *  
 *  Notes:
 *
 *  Memory blocks are allocated from the end of the first free memory
 *  block large enough to satisfy the request.
 */

#include <std.h>
#include <dbc.h>

#include <mem.h>
#include <rmm.h>

#include <tsk.h>


/*
 *  ======== RMM_Header ========
 *  This header is used to maintain a list of free memory blocks.
 */
typedef struct RMM_Header {
    struct RMM_Header  *next;
    Uns                 size;   /* size of the free memory */
    LgUns               addr;   /* DSP address of memory block */
} RMM_Header;

/*
 *  ======== RMM_Obj ========
 */
typedef struct RMM_Obj {
    LgUns           base;                       /* base of memory segment */
    LgUns           length;         /* length of memory segment */
    RMM_Header      freeList;       /* list of free memory blocks */
    RMM_Header      headerPool;     /* Pool of RMM headers so we don't
                                     * have to allocate any in RMM_free().
                                     */
    Void           *buf;             /* Buffer allocated for RMM_Headers */
    Int             segid;           /* Memory segment to allocate from */
    Int             nHeaders;        /* Number of RMM headers allocated */
} RMM_Obj;

static Bool allocBlock(RMM_Handle rmm, LgUns size, LgUns *addr);
static Bool freeBlock(RMM_Handle rmm, LgUns addr, LgUns size);
    
/*
 *  ======== RMM_alloc ========
 */
Bool RMM_alloc(RMM_Handle rmmHandle, LgUns size, LgUns *addr)
{
    Bool                status;

    DBC_require(rmmHandle != NULL);
    DBC_require(addr != NULL);
    DBC_require(size > 0);

    /*
     *  TO DO: Check size of free memory header pool, and allocate another
     *  chunk if necessary.
     */
    status = allocBlock(rmmHandle, size, addr);

    return (status);
}

/*
 *  ======== RMM_check ========
 */
Void RMM_check(RMM_Handle rmmHandle, RMM_Stat *stat)
{
    RMM_Header *head;
    RMM_Header *prev;
    RMM_Header *curr;
    Int         nFreeBlocks;
    Int         nFreeHeaders;
    LgUns       totalFree = 0;
    LgUns       maxBlockSize = 0;

    head = &(rmmHandle->freeList);
    curr = prev = head;
    nFreeBlocks = 0;

    /* Check the free blocks */
    while ((curr = prev->next) != head) {
        nFreeBlocks++;
        totalFree += curr->size;

        if (maxBlockSize < curr->size) {
            maxBlockSize = curr->size;
        }

        if (curr->next != head) {
            DBC_assert(curr->addr < curr->next->addr);
        }

        prev = curr;
    }

    head = &(rmmHandle->headerPool);
    curr = prev = head;

    /* Count number of headers in the free pool */
    nFreeHeaders = 0;
    while ((curr = prev->next) != head) {
        nFreeHeaders++;
        prev = curr;
    }

    DBC_assert((nFreeHeaders + nFreeBlocks) == rmmHandle->nHeaders);

    stat->nBlocks = nFreeBlocks;
    stat->size = rmmHandle->length;
    stat->used = rmmHandle->length - totalFree;
    stat->length = maxBlockSize;
}

/*
 *  ======== RMM_create ========
 */
RMM_Handle RMM_create(RMM_Attrs *attrs)
{
    RMM_Obj    *handle;
    Int         i;
    RMM_Header *headers;
    RMM_Header *curr;
    RMM_Header *prev;
    RMM_Header *first;
    Int         segid;
    Uns         size;


    DBC_require(attrs != NULL);
    DBC_require(attrs->maxFreeListBlocks > 0);
    DBC_require(attrs->length > 0);

    segid = attrs->segid;

    /* Allocate RMM object */
    handle = (RMM_Obj *)MEM_calloc(segid, sizeof(RMM_Obj), sizeof(LgUns));

    if (handle != MEM_ILLEGAL) {
        handle->segid = segid;
        handle->length = attrs->length;
        handle->nHeaders = attrs->maxFreeListBlocks;

        /*
         *  Allocate pool of RMM_Header objects. This is so we will
         *  not have to do any allocation in RMM_free(), and should help
         *  prevent memory from fragmenting.
         */
        size = attrs->maxFreeListBlocks * sizeof(RMM_Header);
        headers = (RMM_Header *)MEM_calloc(segid, size, sizeof(LgUns));

        if (headers == MEM_ILLEGAL) {
            MEM_free(segid, handle, sizeof(RMM_Obj));
            handle = NULL;
        }
        else {
            /* Save the headers buffer for freeing later. */
            handle->buf = (Void *)headers;

            /*
             *  Initialize the free list with the first free block.
             *  Note: the head of the list is not actually used as a header
             *  for a free block, but points to the first header.
             */
            (handle->freeList).next = headers;
            headers->next = &(handle->freeList);
            headers->addr = attrs->base;
            headers->size = attrs->length;

            /*
             *  Queue up the remaining RMM headers. The head of the list
             *  is not used as an RMM_Header. The list is empty when
             *  headerPool.next == &headerPool
             */
            handle->headerPool.next = &(handle->headerPool);
            prev = first = &(handle->headerPool);

            for (i = 1; i < attrs->maxFreeListBlocks; i++) {
                curr = headers + i;

                /* Circular list, so connect the beginning to the end */
                curr->next = first;

                prev->next = curr;
                prev = curr;
            }
        }
    }

    return (handle);
}

/*
 *  ======== RMM_delete ========
 */
Void RMM_delete(RMM_Handle rmmHandle)
{
    Uns         size;

    DBC_require(rmmHandle != NULL);

    /* Free the buffer of RMM_Headers */
    if (rmmHandle->buf) {
        size = rmmHandle->nHeaders * sizeof(RMM_Header);
        MEM_free(rmmHandle->segid, rmmHandle->buf, size);
    }
        
    MEM_free(rmmHandle->segid, rmmHandle, sizeof(RMM_Obj));
}

/*
 *  ======== RMM_exit ========
 */
Void RMM_exit(Void)
{
}

/*
 *  ======== RMM_free ========
 */
Bool RMM_free(RMM_Handle rmmHandle, LgUns addr, LgUns size)
{
    Bool status = TRUE;

    DBC_require(rmmHandle != NULL);
    DBC_require(size > 0);

    /* Free memory */
    status = freeBlock(rmmHandle, addr, size);

    return (status);
}

/*
 *  ======== RMM_init ========
 */
Void RMM_init(Void)
{
}

/*
 *  ======== allocBlock ========
 *  This allocation function allocates memory from the lowest addresses
 *  first.
 */
static Bool allocBlock(RMM_Handle rmmHandle, LgUns size, LgUns *addr)
{
    RMM_Header *head;
    RMM_Header *poolHead;
    RMM_Header *prev;
    RMM_Header *curr;
    RMM_Header *next;
    LgUns       blockSize;
    Bool        retVal = FALSE;


    head = &(rmmHandle->freeList);
    prev = head;

    /* Protect queues in case of context switch */
    TSK_disable();

    while ((curr = prev->next) != head) {
        next = curr->next;
        blockSize = curr->size;        

        if (blockSize >= size) {       /* big enough */
            if (blockSize == size) {
                /*
                 *  Remove current header from the list of free blocks
                 *  and put it back in the pool of free headers, at the
                 *  beginning of the list.
                 */
                prev->next = next;

                poolHead = &(rmmHandle->headerPool);
                curr->next = poolHead->next;
                poolHead->next = curr;
                
            }
            *addr = curr->addr;
            curr->size -= size;
            curr->addr += size;

            retVal = TRUE;
            break;
        }
        prev = curr;
    }

    TSK_enable();

    return (retVal);
}

/*
 *  ======== freeBlock ========
 */
static Bool freeBlock(RMM_Handle rmmHandle, LgUns addr, LgUns size)
{
    RMM_Header *head;
    RMM_Header *poolHead;
    RMM_Header *curr;
    RMM_Header *next;
    RMM_Header *rhead;
    Bool        joined = FALSE;
    Bool        retVal = TRUE;


    DBC_assert(size > 0);

    /* Get the head of the free list */
    head = &(rmmHandle->freeList);

    /* Protect queues in case of context switch */
    TSK_disable();

    /*
     *  Search down the list of free blocks for the right place to put a
     *  header with address "addr". List is sorted by address, with lowest
     *  address first in the list.
     */
    curr = head;
    while ((curr->next != head) && (addr > curr->next->addr)) {
        curr = curr->next;
    }

    next = curr->next;

    /*
     *  The freed block should be put in between curr and next, and the
     *  it should not extend into the next block.
     */
    DBC_assert((curr == head) || (addr > curr->addr));
    DBC_assert((next == head) || (addr + size <= next->addr));

    /* Join with upper block, if possible */
    if ((next != head) && ((addr + size) == next->addr)) {
        next->size += size;
        next->addr = addr;
        joined = TRUE;
    }

    /* Join with the lower block, if possible */
    if ((curr != head) && ((curr->addr + curr->size) == addr)) {
        if (joined) {
            /*
             *  We need to join the upper block and lower blocks, returning
             *  one of the headers back to the free pool.
             */
            curr->size += next->size;

            /* Take "next" out of the free block list */
            curr->next = next->next;

            /* Put "next" in the pool of free headers */
            poolHead = &(rmmHandle->headerPool);
            next->next = poolHead->next;
            poolHead->next = next;
        }
        else {
            curr->size += size;
            joined = TRUE;
        }
    }

    /*
     *  If we can't join the block we're freeing to an existing free block,
     *  we need to get an RMM_Header from the free pool, initialize it, and
     *  insert it in the free list.
     */
    if (!joined) {
        poolHead = &(rmmHandle->headerPool);

        /* If the pool of headers is empty, we're stuck */
        if (poolHead->next == poolHead) {
            /* List is empty */
            DBC_assert(FALSE);
            retVal = FALSE;
        }
        else {
            /* Take the first header in the free pool. */
            rhead = poolHead->next;
            DBC_assert(rhead != NULL);

            poolHead->next = rhead->next;

            /*
             *  Now insert rhead into the free block list:
             *      curr --> rhead --> next
             */
            curr->next = rhead;
            rhead->next = next;
            rhead->addr = addr;
            rhead->size = size;
        }
    }

    TSK_enable();

    return (retVal);
}

/*
 *  @(#) ti.sdo.fc.dman3; 1, 0, 1,66; 5-10-2007 10:49:43; /db/atree/library/trees/fc-b12x/src/
 */

