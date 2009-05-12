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
 *  ======== dskt2_scratch.c ========
 *  DSKT2 scratch sharing support functions
 *
 */
#include <std.h>

#include <dbg.h>
#include <dbc.h>
#include <ti/xdais/ialg.h>
#include <mem.h>
#include <sys.h>
#include <tsk.h>
#include <_dskt2.h>
#include <dskt2.h>
#include <dskt2_permute.h>


#define USESCRATCHOPT 1  /* Set to 1 to optimize scratch usage */

/*
 *  Set to 1 to allow allocation scratch heaps in DSKT2_allocScratch(), if
 *  they have not yet been allocated.
 */
#define ALLOCSCRATCH 1

/*
 *  ALLOC_ORIGSIZE = 1:
 *    if scratch heap has not been allocated yet when DSKT2_allocScratch()
 *    is called, a scratch heap of size desc->origSize will be allocated.
 *
 *  ALLOC_ORIGSIZE = 0:
 *    if scratch heap has not been allocated yet when DSKT2_allocScratch()
 *    is called, a scratch heap of size of the requested scratch memory
 *    will be allocated.
 */
#define ALLOC_ORIGSIZE 0

#define _DSKT2_NUM_INTERNAL_HEAPS 6
#define _DSKT2_MAX(x,y) ( ( (x) > (y) ) ? (x) : (y) )
#define _DSKT2_MIN(x,y) ( ( (x) < (y) ) ? (x) : (y) )


/*
 * Global private module declarations.
 */
_DSKT2_ScratchBufferDesc *_DSKT2_scratchDescTbl[DSKT2_NUM_SHAREDHEAPS];
_DSKT2_MemTabListNode *_DSKT2_memTabList = NULL;
IALG_Handle *_DSKT2_activeAlg = NULL;
IALG_Handle *_DSKT2_lastActiveAlg = NULL;
Int *_DSKT2_scratchShareRefCount = NULL;
Int  _DSKT2_heapId;
_DSKT2_CreateCallbackFxn _DSKT2_createCallbackFxn = NULL;


/*
 * Private local functions
 */
static Bool _DSKT2_freeAllocatedMemory(Int scratchMutexId, IALG_MemRec *memTab,
    Int failedIndex, Int extHeapId);
static Void _DSKT2_incrScratchReferenceCount(Int scratchMutexId);
static Void _DSKT2_decrScratchReferenceCount(Int scratchMutexId);
static Int  _DSKT2_exitScratch(Int scratchMutexId);
Bool _DSKT2_useSharedScratch(_DSKT2_ScratchBufferDesc *scrBuffDesc,
    IALG_MemRec *memTab);
static Bool _DSKT2_isSharedScratchAddr(Int scratchMutexId, Void *base);
static Void _DSKT2_initializeAndUseSharedScratch(Int scratchMutexId, 
        IALG_MemRec *memTab, Int numRecs);
static Bool _DSKT2_allocateFromSharedScratch(Int scratchMutexId,
    IALG_MemRec *memTab);
static Bool _DSKT2_allocateInDesignatedSpace(IALG_MemRec memTab[], Int i,
    IALG_MemSpace ialgSpace, Int segId);
static Bool _DSKT2_allocateInInternal(IALG_MemRec memTab[], Int i,
        Int extSegId);
static Bool _DSKT2_usesInternalScratch(IALG_MemRec *memTab, Int numRecs);
static LgUns _DSKT2_getScratchUsed(IALG_Handle alg, Int mutexId, Int heapId);

/********* TO DO: Move to dskt2.h or _dskt2.h? *********/
Bool DSKT2_allocScratch(IALG_Handle alg,Int mutexId, IALG_MemRec *memTab,
        Int numRecs);

Void DSKT2_freeScratch(Int mutexId, Void *addr, Uns size);

/*
 *  ======== _DSKT2_init ========
 */
Void _DSKT2_init()
{
    static Int initialized = 0;
    Int i;

    if (!initialized) {
        initialized = 1;
        /*
         * Use the heapId from DSKT2 configuration for the allocation of
         * all internal DSKT2 data structures.
         */
        _DSKT2_heapId = _DSKT2_HEAP;

        /* Dynamically allocate and initialize DSKT2 internal data
         * structures in the _DSKT2 heap segment
         */
        if ((_DSKT2_activeAlg = MEM_calloc(_DSKT2_heapId,
                sizeof(IALG_Handle) * DSKT2_NUM_SCRATCH_GROUPS, 0))
                == MEM_ILLEGAL){

            _DSKT2_throwMPUMemException("_DSKT2_activeAlg");
        }
        if ((_DSKT2_lastActiveAlg = MEM_calloc(_DSKT2_heapId,
                sizeof(IALG_Handle) * DSKT2_NUM_SCRATCH_GROUPS, 0))
                == MEM_ILLEGAL){

            _DSKT2_throwMPUMemException("_DSKT2_lastActiveAlg");
        }
        if ((_DSKT2_scratchShareRefCount = MEM_calloc(_DSKT2_heapId,
                sizeof(Int) * DSKT2_NUM_SCRATCH_GROUPS, 0)) == MEM_ILLEGAL) {

            _DSKT2_throwMPUMemException("_DSKT2_scratchShareRefCount");
        }

        /*
         * Allocate scratch-group heap descriptors
         */
        for (i = 0; i < DSKT2_NUM_SHAREDHEAPS; i++) {

            if ((_DSKT2_scratchDescTbl[i] = (_DSKT2_ScratchBufferDesc *)
                    MEM_calloc(_DSKT2_heapId, sizeof(_DSKT2_ScratchBufferDesc)
                        * DSKT2_NUM_SCRATCH_GROUPS, 0)) == MEM_ILLEGAL) {

                _DSKT2_throwMPUMemException("_DSKT2_scratchDescTbl");
            }
        }

        /*
         * Initialize the scratch heap sizes for SARAM and DARAM for each
         * scratch-group using data obtained from DSKT2 CDB configuration
         */
        for (i = 0; i < DSKT2_NUM_SCRATCH_GROUPS; i++) {
            _DSKT2_scratchDescTbl[_DSKT2_DARAMHEAP][i].origSize =
                _DSKT2_DARAM_SCRATCH_SIZES[i];
            _DSKT2_scratchDescTbl[_DSKT2_SARAMHEAP][i].origSize =
                _DSKT2_SARAM_SCRATCH_SIZES[i];
        }
    }
}

/*
 *  ======== DSKT2_throwMemException ========
 */
Void _DSKT2_throwMPUMemException(Char *msg)
{
    SYS_printf("** MPU Exception <can't allocate: %s> **\n", msg);
    SYS_abort(SYS_errors[SYS_EALLOC]);
}

/*
 *  ======== DSKT2_assignInstanceMemory ========
 *
 * Allocates memory requested by the algorithm from configured IALG Memory
 * Space Heaps. For "scratch" memory requests, allocation is from a shared
 * scratch buffer for all algorithm instances belonging to the same scratch
 * group (whenever possible).
 *
 * NOTE on RE-ENTRANCY:

 * Since MEM_calloc cannot be used within a TSK_disable'd context
 * mutual exclusion needs to be enforced by the application framework
 * when executing this function. All algorithm instances created using
 * the same scratchMutexId must be guaranteed by the
 * framework/application to run mutually exclusively.  This means, we
 * are assuming create phase of XDAIS Socket Nodes at the same node
 * 'priority' cannot be scheduled concurrently.
 */
Bool _DSKT2_assignInstanceMemory(Int scratchMutexId, IALG_MemRec *memTab,
    Int numRecs, Int extHeapId)
{
    Int i;
    Int failedIndex = 0;
    Bool succeeded = TRUE;

    if ( (scratchMutexId >= 0) &&
         (scratchMutexId < DSKT2_NUM_SCRATCH_GROUPS) &&
         _DSKT2_usesInternalScratch(memTab, numRecs) ) {
        /*
         * If algo requests any internal scratch memory:
         * Prepare shared scratch area for use by current scratchMutexId
         */
        _DSKT2_initializeAndUseSharedScratch(scratchMutexId, memTab, numRecs);
    }

    for (i = 0; i < numRecs; i++) {

        /* skip allocation if this memTab rec has been pre-allocated from
         * internal shared scratch
         */
        if ((memTab[i].attrs == IALG_SCRATCH) &&
            (!(IALG_isOffChip(memTab[i].space))) && /* not requested as
                                                     * external scratch
                                                     */
                (scratchMutexId >= 0) &&
                (scratchMutexId < DSKT2_NUM_SCRATCH_GROUPS) &&
                (_DSKT2_isSharedScratchAddr(scratchMutexId, memTab[i].base))) {
            continue;
        }

        /*
         * If the requested buffer, 'i', is of "Internal Scratch" type apply
         * scratch sharing policy.
         */
        if ((memTab[i].attrs == IALG_SCRATCH) &&
                (scratchMutexId >= 0) &&
                (scratchMutexId < DSKT2_NUM_SCRATCH_GROUPS) &&
                (!(IALG_isOffChip(memTab[i].space))) ) {

            int usesDaram0Heap = 0;
            int usesSaram0Heap = 0;

            switch (memTab[i].space) {
                case IALG_DARAM1:
                case IALG_DARAM2:
                    usesDaram0Heap = (_DSKT2_spaceToSeg(memTab[i].space)
                        == _DSKT2_spaceToSeg(IALG_DARAM0));

                case IALG_SARAM1:
                case IALG_SARAM2:
                    usesSaram0Heap = (_DSKT2_spaceToSeg(memTab[i].space)
                        == _DSKT2_spaceToSeg(IALG_SARAM0));

                    /* If the requested SARAM (or DARAM) segment is mapped to
                     * the same MEM heap segment for SARAM0 (or DARAM0), then
                     * request is treated as SARAM0 (DARAM0). Otherwise it is
                     * a distinct segment and we try allocating as
                     * unshared scratch in the requested mem space.
                     * If allocation fails: we try  default 'sharing' policy
                     */
                    if (!usesSaram0Heap && !usesDaram0Heap &&
                            _DSKT2_allocateInDesignatedSpace(memTab, i,
                            memTab[i].space, extHeapId)) {

                        continue; /* succeeded */
                    }

                case IALG_DARAM0:
                case IALG_SARAM0:

                    /*
                     * Search for available shared scratch memory:
                     *    for IALG_DARAM0: first try DARAM then SARAM.
                     *    for IALG_SARAM0: first try SARAM then DARAM.
                     */
                    if (_DSKT2_allocateFromSharedScratch(scratchMutexId,
                            &memTab[i]) ) {

                        continue; /* succeeded */

                    /*
                     * if unable to create in shared scratch area,
                     * try to allocate it in the requested heap without sharing
                     * before trying in "any" internal heap.
                     */
                    } else if (_DSKT2_allocateInDesignatedSpace(memTab, i,
                             memTab[i].space, extHeapId)) {

                        continue; /* succeeded */

                    /*
                     * if unable to share existing scratch space, try to
                     * allocate as persistent buffer in Internal memory.
                     */
                    } else if (_DSKT2_allocateInInternal(memTab, i, extHeapId)) {

                        continue; /* succeeded */

                    } else if (_DSKT2_ALLOW_EXTERNAL_SCRATCH) {
                        /*
                         * If configuration allows trying in External Memory
                         * try to allocate scratch as persistent in Ext Mem
                         */
                         if (_DSKT2_allocateInDesignatedSpace(memTab, i,
                                 IALG_ESDATA, extHeapId)) {

                             continue; /* succeeded */
                          }
                    }
                    /*  -- else --
                     * If configuration does not allow, creation fails here
                     * after not being able to share an existing scratch heap
                     * *and* not being able to allocate as non-shared
                     * (i.e. persistent internal scratch memory.
                     * (Fails without trying to allocate in External memory)
                     */

                default:  /* fail */

                    failedIndex = i;
                    succeeded = FALSE;

                    break;

            } /* end switch */

        } else {/*
                 * Requested memory is not of type: "internal scratch".
                 * Dynamically allocate it as Persistent memory.
                 */

            /* First, try to allocate it in requested heap segment */
            if (_DSKT2_allocateInDesignatedSpace(memTab, i, memTab[i].space,
                        extHeapId)

                    /* Next, try allocating it in ANY Internal heap */
                    || _DSKT2_allocateInInternal(memTab, i, extHeapId)

                    /* Finally, try allocating it in External Memory Heap */
                    || _DSKT2_allocateInDesignatedSpace(memTab, i,
                            IALG_ESDATA, extHeapId)){

                continue;  /* succeeded */
            }
        }
        failedIndex = i;
        succeeded = FALSE;      /* return false on failure */

        break;
    }   /* for i ... */

    /* safe even if scratch is not in range, functions below check for range */
    if (!succeeded) {
        DBG_printf1(TRACE, "DSAM: MEM_calloc failure, index %d", failedIndex);
        _DSKT2_freeAllocatedMemory(scratchMutexId, memTab, failedIndex,
                extHeapId);
        _DSKT2_exitScratch(scratchMutexId);

        return (FALSE);
    }

    /* safe only if scratch group is in the range */
    if ( (scratchMutexId >= 0) &&
         (scratchMutexId < DSKT2_NUM_SCRATCH_GROUPS) &&
         _DSKT2_usesInternalScratch(memTab, numRecs)) {
        _DSKT2_incrScratchReferenceCount(scratchMutexId);
    }

    return (TRUE);
}

/*
 *  ======== _DSKT2_freeAllocatedMemory ========
 */
Bool _DSKT2_freeAllocatedMemory(Int scratchMutexId, IALG_MemRec *memTab, Int n,
        Int extHeapId)
{
    Int i;
    Int segId;

    /* free previous successful allocations for first 'n' buffers: */
    for (i = 0; i < n; i++) {

        if (!_DSKT2_isSharedScratchAddr(scratchMutexId, memTab[i].base)) {
            DBG_printf1(TRACE, "DSAM: freeing %x", (LgUns)memTab[i].base);

            /*
             *  Any IALG memory space that maps to the same segment as
             *  IALG_ESDATA, uses the external memory heap segment.
             */
            segId = _DSKT2_spaceToSeg(memTab[i].space);
            if (segId == _DSKT2_spaceToSeg(IALG_ESDATA)) {
                segId = extHeapId;
            }

            if (MEM_free(segId, memTab[i].base, memTab[i].size) == FALSE) {
                DBG_printf1(TRACE, "DSAM: ERROR freeing %x", memTab[i].base);
                return (FALSE);
            }
        }
    }

    return (TRUE);
}

/*
 *  ======== _DSKT2_freeInstanceMemory ========
 */
Bool _DSKT2_freeInstanceMemory(Int scratchMutexId, IALG_MemRec *memTab,
    Int numRecs, Int extHeapId)
{
    Int fStatus = TRUE;
    /*
     * Free allocated algorithm instance memory.
     * Use the scratch-group Id for determining shared scratch heap.
     */
    if (_DSKT2_freeAllocatedMemory(scratchMutexId, memTab, numRecs, extHeapId)
            == FALSE) {
        DBG_printf0(TRACE, "DSFO: DSKT2_freeInstanceMemory failed");
        fStatus = FALSE;
    }

    if ((scratchMutexId >= 0) &&
         (scratchMutexId < DSKT2_NUM_SCRATCH_GROUPS) &&
         _DSKT2_usesInternalScratch(memTab, numRecs) ) {

        _DSKT2_decrScratchReferenceCount(scratchMutexId);
        _DSKT2_exitScratch(scratchMutexId);
    }

    return (fStatus);
}

/*
 *  ======== _DSKT2_usesInternalScratch ========
 */
Bool _DSKT2_usesInternalScratch(IALG_MemRec *memTab, Int numRecs)
{
     Int i;
     Bool flag = FALSE;

     /* check memTab to see if algorithm requests internal scratch memory */
     for (i=0; i<numRecs; i++) {

         if ((memTab[i].attrs == IALG_SCRATCH) &&
                 (!(IALG_isOffChip(memTab[i].space))) ) {
             flag = TRUE;

             break;
         }
     }

     return flag;
}

/*
 *  ======== _DSKT2_getReqScratchSize ========
 */
Uns _DSKT2_getReqScratchSize(Int ialgSpace, IALG_MemRec *memTab, Int numRecs)
{
    Int i;
    LgUns sum = 0;

    /*
     * Sum up the sizes of all scratch buffers requested by this algorithm
     * that are mapped to the given 'ialgSpace' memory segment.
     */
    for (i = 0; i < numRecs; i++) {

        if ((memTab[i].attrs == IALG_SCRATCH) && (_DSKT2_spaceToSeg(ialgSpace)
                == _DSKT2_spaceToSeg(memTab[i].space)) ) {
            /*
             * Note: This has the effect of combinining  scratch requests
             * that are mapped by the configuration to the same MEM heap, but
             * might otherwise be requested in a different IALG Memory Space.
             */

            sum += memTab[i].size + (memTab[i].alignment);
        }
    }

    /* ensure requested combined size is not greater than the MAX allowed */
    return ((sum > DSKT2_MAX_MEM_ALLOCATION_SIZE)?
            DSKT2_MAX_MEM_ALLOCATION_SIZE : (Uns)sum );
}

/*
 *  ======== _DSKT2_getScratchRequestInfo  ========
 */
Void _DSKT2_getScratchRequestInfo(Int ialgSpace, IALG_MemRec *memTab,
        Int numRecs, Uns *sizeBestCase, Uns *sizeWorstCase,
        Int *scratchRequestIndexes, Int *numScratchRecs)
{
    Int i;

    *sizeBestCase =  *sizeWorstCase = 0;
    *numScratchRecs = 0;
    /*
     * Sum up the sizes of all scratch buffers requested by this algorithm
     * that are mapped to the given 'ialgSpace' memory segment.
     */
    for (i = 0; i < numRecs; i++) {

        if ((memTab[i].attrs == IALG_SCRATCH) && (_DSKT2_spaceToSeg(ialgSpace)
                == _DSKT2_spaceToSeg(memTab[i].space)) ) {
            /*
             * Note: This has the effect of combinining  scratch requests
             * that are mapped by the configuration to the same MEM heap, but
             * might otherwise be requested in a different IALG Memory Space.
             */
            *sizeBestCase  += memTab[i].size ;
            *sizeWorstCase += memTab[i].size + (memTab[i].alignment);
            scratchRequestIndexes[*numScratchRecs] = i;
            (*numScratchRecs)++;

            /* 
             * limit the number of requests to be considered for optimal
             * allocation to the maximum supported.
             */
            if (*numScratchRecs >= _DSKT2_PERMUTE_MAXSIZE) {
                break;
            }
        }
    }

    /* ensure  sizes not greater than the MAX allowed */
    *sizeBestCase  = _DSKT2_MIN(DSKT2_MAX_MEM_ALLOCATION_SIZE, *sizeBestCase);
    *sizeWorstCase = _DSKT2_MIN(DSKT2_MAX_MEM_ALLOCATION_SIZE, *sizeWorstCase);
}

/*
 *  ======== _DSKT2_initializeAndUseSharedScratch ========
 *
 * Prepares shared scratch area for use by current scratchMutexId level:
 *   -  allocates shared scratch heaps if currently heap is unallocated
 *   -  initializes scratch descriptor ptrs to this node's creation use.
 */
Void _DSKT2_initializeAndUseSharedScratch(Int scrGrpId, IALG_MemRec *memTab, 
        Int numRecs)
{
    _DSKT2_ScratchBufferDesc *scrBufDesc;
    Int heapId;
    Int ialgHeap;
    Uns thisAlgScrRequestSizeBestCase, 
        thisAlgScrRequestSizeWorstCase;
    Bool createdNewSharedScratchBuffer;
    MEM_Stat statbuf;

    typedef struct _Locals {
        Int scratchRecIdList[_DSKT2_PERMUTE_MAXSIZE]; 
        Int optimalRecIdList[_DSKT2_PERMUTE_MAXSIZE]; 
        Int maximalRecIdList[_DSKT2_PERMUTE_MAXSIZE];
        Int *permutationList[_DSKT2_PERMUTE_MAXSIZE];
        _DSKT2_PERMUTE_Enumeration enumeration;
    } _Locals;

    _Locals *_locals;
    Int numScratchRecs;
    Int * **recIdList;
    Int i, id;
    LgUns sharedAllocationSize;
    LgUns netAllocationSize;
    LgUns smallestTotalAllocationSize;
    LgUns largestNetAllocationSize;
    LgUns minSharedAllocationSize;

    DBC_require((scrGrpId >= 0) && (scrGrpId < DSKT2_NUM_SCRATCH_GROUPS));

    /* allocate locals dynamically to reduce stack requirements */
    if ((_locals = MEM_calloc(_DSKT2_heapId, sizeof(_Locals), 0))
            == MEM_ILLEGAL){
        return;
    }

    for (heapId = 0; heapId < DSKT2_NUM_SHAREDHEAPS; heapId++) {
        scrBufDesc = &_DSKT2_scratchDescTbl[heapId][scrGrpId];
        ialgHeap = (heapId == 0) ? IALG_DARAM0: IALG_SARAM0;
        createdNewSharedScratchBuffer = FALSE;

        /*
         * If configuration maps both SARAM0 and DARAM0 heaps to the same
         * BIOS MEM segment than DSKT2 uses DARAM as the single primary shared
         * heap. Everything still works since SARAM scratch sharing policy
         * uses DARAM as its alternate heap.
         */
        if ((ialgHeap == IALG_SARAM0) &&  (_DSKT2_spaceToSeg(IALG_DARAM0)
                == _DSKT2_spaceToSeg(IALG_SARAM0)) ) {

            continue; /* skip SARAM0 when both heaps are mapped to same seg */
            /*
             * Note: this test is necessary, otherwise we would allocate twice
             * the shared scratch memory due to the 'getReqScratchSize'
             * call (below), which would return the same size for both
             * IALG_DARAM0 and IALG_SARAM0 heaps.
             */
        }

        /*
         * Get aggregate scratch requirement for the current algorithm
         * for this IALG memory space.
         * 
         * 'thisAlgScrRequestSizeWorstCase' => each requested scratch buffer
         *          will be allocated with full padding to meet its alignment 
         * 'thisAlgScrRequestSizeBestCase' => each requested scratch buffer
         *          will be allocated with no padding for alignment 
         * 'scratchRecIdList' will be initialized with the indexes of the
         *          memTab entries containing "Scratch" type memory requests
         *          in memory spaces associated with the ialgHeap.
         * 'numScratchRecs' will be initialized with the number of such 
         *          memTab entries 
         */
        _DSKT2_getScratchRequestInfo(ialgHeap, memTab, numRecs,
                       &thisAlgScrRequestSizeBestCase, 
                       &thisAlgScrRequestSizeWorstCase,
                       (Int *)_locals->scratchRecIdList,
                       &numScratchRecs);

        if (scrBufDesc->base == MEM_ILLEGAL) {
            /*
             * If this request is larger than configuration provided scratch
             * buffer size for this group-id & heap AND the shared buffer
             * has not been allocated yet: bump up the configured size info
             * to satisfy current algo's requirements (for this scratch-group)
             */
            scrBufDesc->size = _DSKT2_MAX (scrBufDesc->origSize,
                                    thisAlgScrRequestSizeWorstCase);
        }

        /*
         * allocate shared scratch heap for the 'ialgHeap' only if
         * 'this algorithm' requests a shared scratch in this heap, and
         * a shared buffer for this type of heap is not already allocated.
         */
        if ((scrBufDesc->size > 0) && (scrBufDesc->base == MEM_ILLEGAL)
                && (thisAlgScrRequestSizeWorstCase > 0)) {

            /*
             * Check the size of largest free contiguous block that can be 
             * allocated in the requested heap, and cap the allocation size
             */
            MEM_stat(_DSKT2_spaceToSeg(ialgHeap), &statbuf);

            /* 
             * if not enough heap to allocate worst case size shared scratch 
             * area try an optimistic approach.
             */
            scrBufDesc->size = _DSKT2_MIN (statbuf.length, scrBufDesc->size); 
            
            if (scrBufDesc->size <= 0) {

                /* 
                 * No Shared scratch buffer can be allocated for this heap &
                 * and scratch-group, likely out of heap memory. So, skip.
                 */
                continue;

            } else {

                /* 
                 * otherwise, continue on and allocate new shared scratch buffer
                 */
                DBG_printf1(TRACE, "DSAM: allocing space %x", ialgHeap);
                DBG_printf1(TRACE, "DSAM: allocing size %x", scrBufDesc->size);
                DBG_printf1(TRACE, "DSAM: allocing align %x", 0);

                scrBufDesc->scratchPtr = scrBufDesc->base = MEM_calloc(
                        _DSKT2_spaceToSeg(ialgHeap), scrBufDesc->size, 0 );

                DBG_printf1(TRACE, "DSAM: alloc'd addr= %x",
                        (LgUns)scrBufDesc->base);

                if (scrBufDesc->base == MEM_ILLEGAL) {

                    /* do nothing special if unable to allocate scratch area
                     * nice try. BUT this is unlikely to happen, due to MEM_stat
                     * check and if there are no other tasks which can preempt
                     * this task AND do MEM_alloc/free using this heap, 
                     * then it should never happen.
                     */
                    continue;

                } else {

                    createdNewSharedScratchBuffer = TRUE;
                }
            }
        }

        /* start sharing at 'base' */
        scrBufDesc->scratchPtr = scrBufDesc->base;

        /*
         * If there is a shared scratch buffer for this 'ialgHeap', and 
         * 'this algorithm' requests a shared scratch in this heap, 
         * try to satisfy the requested scratch buffers from the shared buffer.
         *
         * Find the optimal placement by considering all permutations of 
         * memTab scratch request records, finding the order that results in 
         * the minimal size using DSKT2's sequential allocation policy.
         */
        if ((scrBufDesc->size > 0) && (scrBufDesc->base != MEM_ILLEGAL)
                && (thisAlgScrRequestSizeWorstCase > 0)) {

            /* 
             * initialize permutationList as the original ordering
             * of the scratch memTab entries stored in scratchRecIdList
             */ 
            for (i=0; i<numScratchRecs; i++) {
                _locals->permutationList[i] = &(_locals->scratchRecIdList[i]);
                _locals->optimalRecIdList[i] = _locals->scratchRecIdList[i];
            }
            _DSKT2_Permute_setInitialState(&(_locals->enumeration), 
                        (void **)&(_locals->permutationList), numScratchRecs);

            smallestTotalAllocationSize = 0; /* nothing initially */
            minSharedAllocationSize = 0;  /* nothing initially */
            largestNetAllocationSize = 0; /* nothing initially */
            
            /*
             * iterate through all permutations of the memTab scratch 
             * request records until best-case is established or all
             * permutations evaluated, noting the optimal order.
             */
            while (NULL != (recIdList = (int ***)
                        _DSKT2_Permute_iterateNext(&(_locals->enumeration))) ) {

                _DSKT2_ScratchBufferDesc tmpBuffDesc;
                Bool allocatedAll = FALSE;
                Bool allocatedSome = FALSE;
                Int  numRecsAllocated = 0;

                tmpBuffDesc.scratchPtr =  scrBufDesc->base;
                tmpBuffDesc.size =  scrBufDesc->size;
                tmpBuffDesc.base = scrBufDesc->base;

                for (i=0; i<numScratchRecs; i++) {

                    /* Reset memTab base before trying this iteration */
                    memTab[*recIdList[i][0]].base = MEM_ILLEGAL;

                    /*
                     * Try to allocate current ordering of memTab recs
                     * using the shared scratch buffer.
                     */
                    if (_DSKT2_useSharedScratch(&tmpBuffDesc, 
                            &memTab[*recIdList[i][0]]) ) {
                        allocatedSome = TRUE;
                        numRecsAllocated++;
                    }
                }

                if (numRecsAllocated == numScratchRecs) {
                        allocatedAll = TRUE; 
                }

                /*
                 * 
                 */
                if (allocatedSome) {
                    sharedAllocationSize = (LgUns)(tmpBuffDesc.scratchPtr) -
                            (LgUns)(scrBufDesc->base);

                    /*
                     * If all scratch records fit into shared scratch buffer
                     * check if 'this' permutation is better than previous best
                     * (note: smallestTotalAllocationSize==0 ensures the 
                     *        first total allocation sets it.)
                     */
                    if ( allocatedAll && 
                       ((smallestTotalAllocationSize == 0)/* first time */ ||
                        (sharedAllocationSize < smallestTotalAllocationSize))) {

                        smallestTotalAllocationSize = sharedAllocationSize;

                        /* this permutation becomes the optimalRecIdList */
                        for (i=0; i<numScratchRecs; i++) {
                            _locals->optimalRecIdList[i] = *recIdList[i][0];
                        }
                    }

                    /* 
                     * if unable to allocate 'all' scratch buffers inside the 
                     * shared buffer (but allocated 'some'), then save the 
                     * permutation that results in utilizing the  shared 
                     * with best fit of the subset of memTab entries.
                     * Record bestfit memTab permutation in maximalRecIdList
                     */
                    if ( !allocatedAll ) {
                    
                        /* sum up the 'sizes' of those sharing scratch */
                        netAllocationSize = 0;
                        for (i=0; i<numScratchRecs; i++) {
                            id = *recIdList[i][0];
                            netAllocationSize += ((memTab[id].base == 
                                    MEM_ILLEGAL)?  0 : memTab[id].size);
                        }

                        /* if better fit then previous best, update best */
                        if (netAllocationSize >= largestNetAllocationSize) {
                        
                            /* when tie, pick the one which uses least shared*/
                            if((netAllocationSize == largestNetAllocationSize)
                                &&
                               (sharedAllocationSize > minSharedAllocationSize))
                            {
                                /* do nothing, when there is better fit */
                            } else {
                                largestNetAllocationSize = netAllocationSize;
                                minSharedAllocationSize = sharedAllocationSize;

                                /* this permutation is the maximalRecIdList */
                                for (i=0; i<numScratchRecs; i++) {
                                    _locals->maximalRecIdList[i] = 
                                            *recIdList[i][0];
                                }
                            }
                        } 
                    }
                    /* 
                     * if smallestTotalAllocationSize is the best possible  
                     * stop evaluating other permutations, optimal order 
                     * of granting shared scratch memory has been found!
                     */
                    if (smallestTotalAllocationSize == 
                            thisAlgScrRequestSizeBestCase) {

                        break;
                    }
                }
            }
            /* TODO: may free & realloc shared buff with optimalSize, as long 
             * as reallocation has the same base/alignment as free'd one.
             */
 
            /* Now really assign scratch buffers using shared scratch*/
            for (i = 0; i < numScratchRecs; i++) {
                memTab[_locals->scratchRecIdList[i]].base = MEM_ILLEGAL;
            }

            /* Now really assign scratch buffers using shared scratch*/
            if (smallestTotalAllocationSize > 0) {

                /*
                 * Optimal allocation order has been found, use it to alloc
                 */
                for (i=0; i<numScratchRecs; i++) {

                    _DSKT2_useSharedScratch(scrBufDesc, 
                            &memTab[_locals->optimalRecIdList[i]]);
                }
            } 
            else if (largestNetAllocationSize > 0) {
                /*
                 * Otherwise, share the scratch buffer as fully as possible.
                 */
                for (i=0; i<numScratchRecs; i++) {

                    _DSKT2_useSharedScratch(scrBufDesc, 
                            &memTab[_locals->maximalRecIdList[i]]);
                }
            } 
            else if (createdNewSharedScratchBuffer) {
                /* 
                 * We have NO scratch buffer requests that can be allocated
                 * using the shared scratch buffer that we just created!!!
                 * It was a nice try, BUT now free it. 
                 * Note: no-one else in this scratch group is sharing it yet
                 */
                DBG_printf1(TRACE, "DSAM: freeing %x", (LgUns)scrBufDesc->base);
            
                if (MEM_free(_DSKT2_spaceToSeg(ialgHeap), scrBufDesc->base,
                                         scrBufDesc->size) == FALSE) {
                   DBG_printf1(TRACE,"DSAM: ERROR freeing %x",scrBufDesc->base);
                }
                scrBufDesc->scratchPtr = scrBufDesc->base = MEM_ILLEGAL; 
                scrBufDesc->size = 0;
            }   
            
        } /* end if -- no scratch memory request for this alg in this heap */ 
    }

    /* free locals on exit */
    DBG_printf1(TRACE,"DSAM: freeing _locals:%x", _locals);
    if (MEM_free(_DSKT2_heapId, _locals, sizeof(_Locals)) ==  FALSE) {
        DBG_printf1(TRACE,"DSAM: ERROR freeing _locals:%x", _locals);
    }
}

/*
 *  ======== _DSKT2_allocateInDesignatedSpace ========
 *
 * Allocate as "persistent" in the designated IALG MemSpace.
 */
Bool _DSKT2_allocateInDesignatedSpace(IALG_MemRec memTab[], Int i,
         IALG_MemSpace ialgSpace, Int extHeapId)
{
    Int  segId;
    Bool status = TRUE;

    DBG_printf1(TRACE, "DSAM: allocing space %x", ialgSpace);
    DBG_printf1(TRACE, "DSAM: allocing size %x", memTab[i].size);
    DBG_printf1(TRACE, "DSAM: allocing align %x", memTab[i].alignment);

    segId = _DSKT2_spaceToSeg(ialgSpace);

    /*
     *  All IALG memory types that are mapped to the same segment as
     *  IALG_ESDATA will use the external heap segment specified by
     *  extHeapId.
     */
    if (segId == _DSKT2_spaceToSeg(IALG_ESDATA)) {
        segId = extHeapId;
    }

    memTab[i].base = MEM_calloc(segId, memTab[i].size, memTab[i].alignment);
    DBG_printf1(TRACE, "DSAM: alloc'd addr = %x", (LgUns)memTab[i].base);

    if (memTab[i].base == MEM_ILLEGAL) {
        DBG_printf1(TRACE, "DSAM: MEM_calloc failure, index %d",i);
        status = FALSE; /* return false on failure */

    } else {
        memTab[i].space = ialgSpace;
        if (memTab[i].attrs == IALG_SCRATCH) {
            memTab[i].attrs = IALG_PERSIST;
        }
    }

    return(status);
}

/*
 *  ======== _DSKT2_allocateInInternal ========
 *
 *  Allocate as "persistent" in one of the available "internal" heaps
 */
Bool _DSKT2_allocateInInternal(IALG_MemRec memTab[], Int i, Int extSegId)
{
    Int heapId;
    Int segId;

    IALG_MemSpace INTERNAL_HEAPS[] = {IALG_DARAM0, IALG_DARAM1, IALG_DARAM2,
                                      IALG_SARAM0, IALG_SARAM2, IALG_SARAM2};

    for (heapId = 0; heapId < _DSKT2_NUM_INTERNAL_HEAPS; heapId++) {

        segId = _DSKT2_spaceToSeg(INTERNAL_HEAPS[heapId]);

        if (segId == _DSKT2_spaceToSeg(IALG_ESDATA)) {
            segId = extSegId;
        }

        DBG_printf1(TRACE, "DSAM: allocing space %x", memTab[i].space);
        DBG_printf1(TRACE, "DSAM: allocing size %x",  memTab[i].size);
        DBG_printf1(TRACE, "DSAM: allocing align %x", memTab[i].alignment);

        if ((memTab[i].base = MEM_calloc(segId, memTab[i].size,
                     memTab[i].alignment)) != MEM_ILLEGAL) {

            memTab[i].space = INTERNAL_HEAPS[heapId];

            if (memTab[i].attrs == IALG_SCRATCH) {
                memTab[i].attrs = IALG_PERSIST;
            }
            DBG_printf1(TRACE, "DSAM: alloc'd addr = 0x%x",
                    (LgUns)memTab[i].base);

            return (TRUE);
        }
    }
    DBG_printf1(TRACE, "DSAM: MEM_calloc failure, index %d", i);

    return (FALSE);
}


/*
 *  ======== _DSKT2_spaceToSeg ========
 */
Uns _DSKT2_spaceToSeg(Uns space)
{
    Uns segment;

    switch (space) {
        case IALG_DARAM0:
            segment = _DSKT_DARAM0;
            break;
        case IALG_DARAM1:
            segment = _DSKT_DARAM1;
            break;
        case IALG_DARAM2:
            segment = _DSKT_DARAM2;
            break;
        case IALG_SARAM0:
            segment = _DSKT_SARAM0;
            break;
        case IALG_SARAM1:
            segment = _DSKT_SARAM1;
            break;
        case IALG_SARAM2:
            segment = _DSKT_SARAM2;
            break;
        case IALG_IPROG:
            segment = _DSKT_IPROG;
            break;
        case IALG_EPROG:
            segment = _DSKT_EPROG;
            break;
        case IALG_ESDATA:
            segment = _DSKT_ESDATA;
            break;
        default:
            segment = _DSKT_ESDATA;
    }

    return (segment);
}

/*
 *  ======== _DSKT2_exitScratch ========
 */
Int _DSKT2_exitScratch(Int scratchMutexId)
{
    /*
     * If ref-count for sharing scratch at this scratchMutexId dropped to zero
     * free any shared heap's that are allocated for share
     */

     _DSKT2_ScratchBufferDesc *scrBufDesc;
     Int heapId;
     Int ialgHeap;
     Int status = _DSKT2_OK;

     if ((scratchMutexId < 0) ||
             (scratchMutexId >= DSKT2_NUM_SCRATCH_GROUPS)) {

         return (status);
     }

     if (_DSKT2_scratchShareRefCount[scratchMutexId] > 0) {

         return (status);
     }

     for (heapId = 0; heapId < DSKT2_NUM_SHAREDHEAPS; heapId++) {
         scrBufDesc = &_DSKT2_scratchDescTbl[heapId][scratchMutexId];
         ialgHeap = (heapId == 0) ? IALG_DARAM0: IALG_SARAM0;

         if ((scrBufDesc->size > 0) && (scrBufDesc->base != MEM_ILLEGAL )) {
             DBG_printf1(TRACE, "DSAM: freeing %x", (LgUns)scrBufDesc->base);

             if (MEM_free(_DSKT2_spaceToSeg(ialgHeap), scrBufDesc->base,
                     scrBufDesc->size) != TRUE) {
                 status = _DSKT2_FAIL_FREE_SHARED_SCRATCH;
             }
             scrBufDesc->base = MEM_ILLEGAL;
         }
    }

    return status;
}

/*
 *  ======== _DSKT2_incrScratchReferenceCount ========
 */
Void _DSKT2_incrScratchReferenceCount(Int scratchMutexId)
{
    DBC_require((scratchMutexId >= 0) &&
                (scratchMutexId < DSKT2_NUM_SCRATCH_GROUPS));

    /* critical section around access to global data structure */
    TSK_disable();
    _DSKT2_scratchShareRefCount[scratchMutexId]++;

    /* restore critical section before return */
    TSK_enable();
}

/*
 *  ======== _DSKT2_decrScratchReferenceCount ========
 */
Void _DSKT2_decrScratchReferenceCount(Int scratchMutexId)
{
    DBC_require((scratchMutexId >= 0) &&
                (scratchMutexId < DSKT2_NUM_SCRATCH_GROUPS));

    /* critical section around access to global data structure */
    TSK_disable();

    _DSKT2_scratchShareRefCount[scratchMutexId]--;

    DBC_assert(_DSKT2_scratchShareRefCount[scratchMutexId] >= 0);

    /* restore critical section before return */
    TSK_enable();
}

/*
 *  ======== _DSKT2_allocateFromSharedScratch ========
 */
Bool _DSKT2_allocateFromSharedScratch(Int mutexId, IALG_MemRec *memTab)
{
    _DSKT2_ScratchBufferDesc *primaryScrBufDesc;
    _DSKT2_ScratchBufferDesc *secondaryScrBufDesc;

    DBC_require((mutexId >= 0) && (mutexId < DSKT2_NUM_SCRATCH_GROUPS));

    if ((memTab->space == IALG_DARAM0) || (memTab->space == IALG_DARAM1) ||
            (memTab->space == IALG_DARAM2)) {
        primaryScrBufDesc  = &_DSKT2_scratchDescTbl[_DSKT2_DARAMHEAP][mutexId];
        secondaryScrBufDesc= &_DSKT2_scratchDescTbl[_DSKT2_SARAMHEAP][mutexId];

    } else {
        primaryScrBufDesc  = &_DSKT2_scratchDescTbl[_DSKT2_SARAMHEAP][mutexId];
        secondaryScrBufDesc= &_DSKT2_scratchDescTbl[_DSKT2_DARAMHEAP][mutexId];
    }

    return (_DSKT2_useSharedScratch(primaryScrBufDesc, memTab) ||
            _DSKT2_useSharedScratch(secondaryScrBufDesc, memTab) );
}

/*
 *  ======== _DSKT2_isSharedScratchAddr ========
 */
Bool _DSKT2_isSharedScratchAddr(Int mutexId, Void *addr)
{
    if ((mutexId < 0) || (mutexId >= DSKT2_NUM_SCRATCH_GROUPS)) {
        return FALSE;
    }

    return (
        ((_DSKT2_scratchDescTbl[_DSKT2_DARAMHEAP][mutexId].base != MEM_ILLEGAL)
                &&
            (addr >= _DSKT2_scratchDescTbl[_DSKT2_DARAMHEAP][mutexId].base)  &&
            ( (LgUns) addr <
            ( (LgUns) _DSKT2_scratchDescTbl[_DSKT2_DARAMHEAP][mutexId].base +
                      _DSKT2_scratchDescTbl[_DSKT2_DARAMHEAP][mutexId].size)))
            ||

        ((_DSKT2_scratchDescTbl[_DSKT2_SARAMHEAP][mutexId].base != MEM_ILLEGAL)
                &&
            (addr >= _DSKT2_scratchDescTbl[_DSKT2_SARAMHEAP][mutexId].base)  &&
            ( (LgUns) addr <
            ( (LgUns) _DSKT2_scratchDescTbl[_DSKT2_SARAMHEAP][mutexId].base +
                      _DSKT2_scratchDescTbl[_DSKT2_SARAMHEAP][mutexId].size))));
}

/*
 *  ======== _DSKT2_useSharedScratch ========
 */
Bool _DSKT2_useSharedScratch(_DSKT2_ScratchBufferDesc *scrBuffDesc,
         IALG_MemRec *memTab)
{
    Uns reqBuffSize;  /* in MAUs */
#if USESCRATCHOPT
    Uns align;
    Uns offset;
#endif
    /* TODO: optimize size of shared region if already aligned */

    if (scrBuffDesc->base == MEM_ILLEGAL) {

        return FALSE;
    }

#if USESCRATCHOPT
    /* Minimize scratch usage if scratchPtr is already aligned */
    align = memTab->alignment;
    if (align < sizeof(Uns)) {
        align = sizeof(Uns);
    }

    /* # of MAUs from last aligned address */
    offset = 0;
    if ((LgUns)scrBuffDesc->scratchPtr & (align - 1)) {
        offset = (Uns)((LgUns)scrBuffDesc->scratchPtr & (align - 1));
        offset = align - offset;
    }

    reqBuffSize = memTab->size + offset;

#else
    /* Allocating more than necessary if scratchPtr is already aligned */
    reqBuffSize = (memTab->alignment > 1) ?
                      memTab->size + memTab->alignment :
                      memTab->size;
#endif
    /* Fail if current scratch req is greater than remaining scratch */
    if (reqBuffSize > (scrBuffDesc->size -
            ((LgUns)scrBuffDesc->scratchPtr - (LgUns)scrBuffDesc->base))) {

        return (FALSE);
    }

#if USESCRATCHOPT
    memTab->base = (Void *)((LgUns)scrBuffDesc->scratchPtr + offset);
#else
    /*
     *  Assign shared scratch memory address at the current free pointer
     *  plus any adjustment to create the correct alignment.
     */
    if (memTab->alignment > 1) {
        memTab->base = (Void *)
            ( ((LgUns)scrBuffDesc->scratchPtr + memTab->alignment) &
            (~((LgUns)memTab->alignment - 1)) );
    }
    else {
        memTab->base =  scrBuffDesc->scratchPtr;
    }
#endif

    /* advance free scratch pointer past current allocation */
    scrBuffDesc->scratchPtr = (Void *) ((LgUns)scrBuffDesc->scratchPtr +
                                        reqBuffSize);

    return (TRUE);
}

/*
 *  ======== _DSKT2_enqueueMemTab ========
 */
Bool _DSKT2_enqueueMemTab(Int segid, IALG_MemRec *memTab, Int memTabSize,
                          Int numRecs, Int extHeapId)
{
    _DSKT2_MemTabListNode *listNodePtr;

    DBC_require(extHeapId >= 0);

    /* allocate the linked list node to store the memTab associated with alg*/
    if ((listNodePtr = MEM_calloc(segid, sizeof(_DSKT2_MemTabListNode), 0))
            == MEM_ILLEGAL) {
       return FALSE;
    }

    /* add to the front of the list */
    listNodePtr->memTab  = memTab;
    listNodePtr->memTabSize = memTabSize;
    listNodePtr->numRecs = numRecs;
    listNodePtr->extHeapId = extHeapId;

    /* critical section around access to global data structure */
    TSK_disable();

    listNodePtr->next    = _DSKT2_memTabList;
    _DSKT2_memTabList = listNodePtr;

    /* restore critical section before return */
    TSK_enable();

    return TRUE;
}

/*
 *  ======== _DSKT2_dequeueMemTab ========
 */
IALG_MemRec *_DSKT2_dequeueMemTab(Int segid, IALG_Handle alg, Int *memTabSize,
    Int *numRecs, Int *extHeapId)
{
    _DSKT2_MemTabListNode *listNodePtr, *prev;
    IALG_MemRec *memTab;

    /* critical section around access to global data structure */
    TSK_disable();

    /* first find the saved memTab entry for the alg Handle */
    prev = listNodePtr = _DSKT2_memTabList;

    while (listNodePtr != NULL) {

        if (listNodePtr->memTab[0].base == alg) {
            prev->next  = listNodePtr->next;
            memTab      = listNodePtr->memTab;
            *memTabSize = listNodePtr->memTabSize;
            *numRecs    = listNodePtr->numRecs;
            *extHeapId  = listNodePtr->extHeapId;

            DBC_assert(listNodePtr->extHeapId >= 0);

            if (_DSKT2_memTabList == listNodePtr) {
                _DSKT2_memTabList = listNodePtr->next;
            }

             /* restore critical section before calling MEM_free & return*/
            TSK_enable();

            /* free the list node but not the actual memTab[] */
            if (MEM_free(segid, listNodePtr, sizeof(_DSKT2_MemTabListNode))
                == FALSE) {

                return (MEM_ILLEGAL);
            }

            return (memTab);
        }
        prev = listNodePtr;
        listNodePtr = listNodePtr->next;
    }

    TSK_enable(); /* restore critical section before return */

    return (MEM_ILLEGAL);
}

/*
 *  ======== _DSKT2_registerCreationCallbackFxn ========
 */
Void _DSKT2_registerCreationCallbackFxn(_DSKT2_CreateCallbackFxn fxn)
{
    _DSKT2_createCallbackFxn = fxn;
}

/*
 *  ======== _DSKT2_unRegisterCreationCallbackFxn ========
 */
Void _DSKT2_unRegisterCreationCallbackFxn(_DSKT2_CreateCallbackFxn fxn)
{
    _DSKT2_createCallbackFxn = NULL;
}

/*
 *  ======== _DSKT2_getScratchUsed ========
 */
LgUns _DSKT2_getScratchUsed(IALG_Handle alg, Int mutexId, Int heapId)
{
    LgUns                     size = 0;
    Int                       i;
    _DSKT2_ScratchBufferDesc *desc;
    IALG_MemRec               memTab;
    _DSKT2_MemTabListNode    *listNodePtr = NULL;
    LgUns                     scratchBase;
    LgUns                     scratchEnd;

    TSK_disable();

    /* Find alg in memTab queue */
    if (alg) {
        listNodePtr = _DSKT2_memTabList;
        while (listNodePtr != NULL) {
            if (listNodePtr->memTab[0].base == alg) {
                /* found */
                break;
            }
            listNodePtr = listNodePtr->next;
        }
    }

    TSK_enable();

    if (listNodePtr != NULL) {
        desc = &_DSKT2_scratchDescTbl[heapId][mutexId];
        scratchBase = (LgUns)(desc->base);
        scratchEnd = (LgUns)(desc->base) + desc->size;

        /*
         *  Compute the scratch memory used - Find the largest memTab
         *  base from the scratch base, and add the size for that memTab.
         *  This accounts for holes left by alignment.
         */
        if (desc != NULL) {
            for (i = 0; i < listNodePtr->numRecs; i++) {
                memTab = listNodePtr->memTab[i];
                if (((LgUns)(memTab.base) >= scratchBase) &&
                        ((LgUns)(memTab.base) < scratchEnd)) {
                    scratchBase = (LgUns)(memTab.base);
                    size = memTab.size;
                }
            }
        }
        size += (scratchBase - (LgUns)desc->base);
    }

    return (size);
}

/*
 *  ======== DSKT2_allocScratch ========
 *  Allocate from scratch memory for alg.
 *  If startAddr = NULL, allocate after scratch that was allocated to alg when
 *  it was created. If startAddr != NULL, start allocating from that address.
 */
Bool DSKT2_allocScratch(IALG_Handle alg, Int mutexId, IALG_MemRec *memTab,
        Int numRecs)
{
    LgUns                     scratchUsed = 0;
    IALG_MemRec               dummyMem;
    Int                       heapId;
    Int                       i;
    _DSKT2_ScratchBufferDesc *desc;
    IALG_MemSpace             space;
    Int                       daram0Seg;
    Int                       saram0Seg;
    Uns                       size = 0;
    Bool                      retVal = TRUE;

    if ((mutexId < 0) || (mutexId >= DSKT2_NUM_SCRATCH_GROUPS)) {
        return (FALSE);
    }

    /*
     *  Determine which scratch heap to use by looking at the space of the
     *  first request for scratch.
     *  Pass space as an argument to DSKT2_allocScratch()? Then it would
     *  be clear which space is being used.
     */
    daram0Seg = _DSKT2_spaceToSeg(IALG_DARAM0);
    saram0Seg = _DSKT2_spaceToSeg(IALG_SARAM0);

    for (i = 0; i < numRecs; i++) {
        if (memTab[i].attrs == IALG_SCRATCH) {
            space = memTab[i].space;
            if (_DSKT2_spaceToSeg(space) == daram0Seg) {
                space = IALG_DARAM0;
                heapId = _DSKT2_DARAMHEAP;
                break;
            }
            if (_DSKT2_spaceToSeg(space) == saram0Seg) {
                space = IALG_SARAM0;
                heapId = _DSKT2_SARAMHEAP;
                break;
            }
        }
    }

    /*
     *  If none of the memory requests were for scratch DARAM0 or SARAM0
     *  memory, there is nothing to do.
     */
    if (i == numRecs) {
        return (TRUE);
    }

    /*
     *  Make sure alignment is 0 if size is 0, so we don't allocate a scratch
     *  heap if all sizes in the memTab array are 0.
     */
    for (i = 0; i < numRecs; i++) {
        if (memTab[i].size == 0) {
            memTab[i].alignment = 0;
        }
    }

    desc = &_DSKT2_scratchDescTbl[heapId][mutexId];

#if ALLOCSCRATCH
    /*
     *  Set up a dummy memTab to force scratch heap to be allocated, if not
     *  allready allocated and the configured size for the scratch group is
     *  non-zero. _DSKT2_initializeAndUseSharedScratch() will also reset the 
     *  scratch pointer to the start of the scratch heap.
     *
     *  Or, should we not allocate the scratch memory if it has not been
     *  allocated yet????
     */
    size = _DSKT2_getReqScratchSize(space, memTab, numRecs);
#if ALLOC_ORIGSIZE
    dummyMem.size = _DSKT2_MAX(desc->origSize, size);
#else
    /*
     *  If the scratch heap has not been created, only make it big enough
     *  for this request.
     */
    dummyMem.size = size;
#endif
    dummyMem.alignment = 0;
    dummyMem.space = space;
    dummyMem.attrs = IALG_SCRATCH;

    _DSKT2_initializeAndUseSharedScratch(mutexId, &dummyMem, 1);
#endif

    if (alg != NULL) {
        /*
         *  Set the base pointer to the end of the scratch memory that was
         *  already allocated (if any) for this alg.
         */
        scratchUsed = _DSKT2_getScratchUsed(alg, mutexId, heapId);
    }

    desc->scratchPtr = (Void *)((LgUns)desc->base + scratchUsed);

    for (i = 0; i < numRecs; i++) {
        if ((memTab[i].size > 0) && (memTab[i].attrs == IALG_SCRATCH)) {
            retVal = _DSKT2_useSharedScratch(desc, &memTab[i]);
            if (!retVal) {
                break;
            }
        }
    }

    if (retVal) {
        /* Increment scratch ref count for all scratch buffers allocted */
        for (i = 0; i < numRecs; i++) {
            if ((memTab[i].size > 0) && (memTab[i].attrs == IALG_SCRATCH)) {
                _DSKT2_incrScratchReferenceCount(mutexId);
            }
        }
    }

    return (retVal);
}

/*
 *  ======== DSKT2_freeScratch ========
 *  Free scratch memory allocated with DSKT2_allocScratch. This will
 *  only decrement the reference count of the scratch buffer, and not change
 *  the amount of scratch used by the alg.
 */
Void DSKT2_freeScratch(Int mutexId, Void *addr, Uns size)
{
    Int                       heapId;
    LgUns                     base;
    LgUns                     end;
    _DSKT2_ScratchBufferDesc *desc;

    if ((mutexId < 0) || (mutexId >= DSKT2_NUM_SCRATCH_GROUPS) ||
            (size == 0)) {
        return;
    }

    /* Make sure base address is in a scratch region */
    for (heapId = 0; heapId < DSKT2_NUM_SHAREDHEAPS; heapId++) {
        desc = &_DSKT2_scratchDescTbl[heapId][mutexId];

        base = (LgUns)desc->base;
        end = (LgUns)desc->base + desc->size;
        if (((LgUns)addr >= base) && ((LgUns)addr + size <= end)) {
            break;
        }
    }

    if (heapId < DSKT2_NUM_SHAREDHEAPS) {
        _DSKT2_decrScratchReferenceCount(mutexId);
        _DSKT2_exitScratch(mutexId);
    }
}
/*
 *  @(#) ti.sdo.fc.dskt2; 1, 0, 1,66; 5-10-2007 10:50:09; /db/atree/library/trees/fc-b12x/src/
 */

