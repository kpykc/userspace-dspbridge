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
 *  ======== dskt2_qos.c ========
 *  DSKT2 QOS scratch sharing support functions
 *
 */
#include <std.h>

#include <dbg.h>
#include <dbc.h>
#include <mem.h>

#include <_dskt2.h>
#include <dskt2.h>
#include <dskt2_qos.h>

/*
 * Global private module reference.
 */
extern _DSKT2_ScratchBufferDesc *_DSKT2_scratchDescTbl[DSKT2_NUM_SHAREDHEAPS];

/*
 * ======== DSKT2_QOS_getStat ========
 *  DSKT2 QOS support function
 */ 

Void  DSKT2_QOS_getStat(Uns heapType, Uns mutexId, LgUns *sizeSharedScratch,
        Uns *inHeap)
{
    Int scratchMutexId;
    Int heapId;
    Uns heapStart, heapEnd;
    Uns scratchGroupStart, scratchGroupEnd;
    _DSKT2_ScratchBufferDesc *scrBufDesc; 
        
    DBC_require( (mutexId < DSKT2_NUM_SCRATCH_GROUPS)
                || (mutexId == DSKT2_QOS_ALL_SCRATCHGROUPS));
        
    DBC_require((heapType == DSKT2_QOS_SHARED_SCRATCH_ALLHEAPS) 
                || (heapType == DSKT2_QOS_SHARED_SCRATCH_DARAM) 
                || (heapType == DSKT2_QOS_SHARED_SCRATCH_SARAM) );

    _DSKT2_init();
         
    switch (heapType) {
        
        case DSKT2_QOS_SHARED_SCRATCH_ALLHEAPS: 
                heapStart = 0;
                heapEnd  = DSKT2_NUM_SHAREDHEAPS-1;
                break;
                
        case DSKT2_QOS_SHARED_SCRATCH_DARAM:
                heapStart = heapEnd = 0;
                *inHeap = _DSKT2_spaceToSeg(IALG_DARAM0);
                break;
                        
        case DSKT2_QOS_SHARED_SCRATCH_SARAM:
                heapStart = heapEnd = 1;
                *inHeap = _DSKT2_spaceToSeg(IALG_SARAM0);
    }
        
    if (mutexId == DSKT2_QOS_ALL_SCRATCHGROUPS) {
        scratchGroupStart = 0;
        scratchGroupEnd =  DSKT2_NUM_SCRATCH_GROUPS;
                
    } else {
    
        scratchGroupStart = scratchGroupEnd = (Uns) mutexId;
    } 

    *sizeSharedScratch = 0;
         
    for (heapId = heapStart; heapId <= heapEnd; heapId++) {
                
        for (scratchMutexId = scratchGroupStart; 
                scratchMutexId <= scratchGroupEnd; scratchMutexId++) {
        
            scrBufDesc = &_DSKT2_scratchDescTbl[heapId][scratchMutexId];
                 
            if ((scrBufDesc->size > 0) && (scrBufDesc->base != MEM_ILLEGAL )) {
                *sizeSharedScratch += scrBufDesc->size;

            }
        }
    }
}

/*
 *  @(#) ti.sdo.fc.dskt2; 1, 0, 1,66; 5-10-2007 10:50:09; /db/atree/library/trees/fc-b12x/src/
 */

