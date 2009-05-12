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
 *  ======== _dskt2.h ========
 *  internal xDAIS socket library definitions.
 *
 */

#ifndef _DSKT2_H
#define _DSKT2_H

#include <_dskt.h>
#include <ti/xdais/ialg.h>
#include <dskt2.h>

#define _DSKT2_MEMTAB_SIZE 16
#define _DSKT2_DARAMHEAP 0
#define _DSKT2_SARAMHEAP 1

#define _DSKT2_OK 0
#define _DSKT2_FAIL_FREE_SHARED_SCRATCH 1

typedef struct _DSKT2_ScratchBufferDesc {
    Uns size;
    Uns origSize;
    Void *base;
    Void *scratchPtr;
} _DSKT2_ScratchBufferDesc;

typedef struct _DSKT2_MemTabListNode {
    IALG_MemRec *memTab;
    Int memTabSize;
    Int numRecs;
    Int extHeapId;              /* segId of algorithm's external heap */
    struct _DSKT2_MemTabListNode *next;
} _DSKT2_MemTabListNode;

typedef void (*_DSKT2_CreateCallbackFxn)(IALG_Handle h, const IALG_MemRec 
   *memTab, Uns numTab, Uns scratchGroupId);

/* Cache writeback function pointer used by DSKT2 if not NULL */
#ifdef _6x_
extern far DSKT2_CacheWBInvFxn DSKT2_cacheWBInvFxn;
#else
extern DSKT2_CacheWBInvFxn DSKT2_cacheWBInvFxn;
#endif

#ifdef _6x_
extern far Uns _DSKT2_HEAP;
extern far Uns _DSKT2_ALLOW_EXTERNAL_SCRATCH;
#else
extern Uns _DSKT2_HEAP;
extern Uns _DSKT2_ALLOW_EXTERNAL_SCRATCH;
#endif

extern _DSKT2_CreateCallbackFxn _DSKT2_createCallbackFxn;

extern Int _DSKT2_heapId;

extern IALG_Handle *_DSKT2_activeAlg;
extern IALG_Handle *_DSKT2_lastActiveAlg;

extern Void _DSKT2_init();
extern Void _DSKT2_registerCreationCallbackFxn(_DSKT2_CreateCallbackFxn fxn); 
extern Void _DSKT2_unRegisterCreationCallbackFxn(_DSKT2_CreateCallbackFxn fxn);
extern Uns  _DSKT2_spaceToSeg(Uns space);

extern Bool _DSKT2_freeInstanceMemory(Int scratchMutexId, IALG_MemRec *memTab, 
    Int numRecs, Int extHeapId);
extern Bool _DSKT2_assignInstanceMemory(Int scratchMutexId, 
    IALG_MemRec *memTab, Int numRecs, Int extHeapId);
extern Uns  _DSKT2_spaceToSeg(Uns space); 

extern Bool _DSKT2_enqueueMemTab(Int segid, IALG_MemRec *memTab, 
    Int memTabSize, Int numRecs, Int extHeapId);
extern IALG_MemRec *_DSKT2_dequeueMemTab(Int segid, IALG_Handle alg, 
    Int *memTabSize, Int *numRecs, Int *extHeapId);
extern Void _DSKT2_throwMPUMemException(Char *msg);

extern Uns _DSKT2_DARAM_SCRATCH_SIZES[];
extern Uns _DSKT2_SARAM_SCRATCH_SIZES[];

#endif

/*
 *  @(#) ti.sdo.fc.dskt2; 1, 0, 1,66; 5-10-2007 10:50:09; /db/atree/library/trees/fc-b12x/src/
 */

