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
 *  ======== dskt2crea.c ========
 *  DSKT2 API to instantiate an XDAIS algorithm instance.
 *
 */
#include <std.h>

#include <dbc.h>
#include <dbg.h>
#include <ti/xdais/ialg.h>
#include <mem.h>

#include <dskt2.h>
#include <_dskt2.h>

/*
 *  ======== DSKT_createAlg ========
 *
 * Creates an instance of a XDAIS algorithm using its IALG interface.
 * Instances created with the same scratch-group Id share the same DARAM and/or
 * SARAM scratch buffers (as mapped to BIOS heaps in configuration). 
 * Scratch Group Id: -1 is used to turn OFF scratch sharing for this instance,
 * and allocate all scratch memory persistently.
 *
 * NOTE on RE-ENTRANCY: 
 *
 * Since MEM_calloc cannot be used within a TSK_disable'd context
 * mutual exclusion needs to be enforced by the application framework
 * when executing this function. All algorithm instances created using
 * the same scratchMutexId must be guaranteed by the
 * framework/application to run mutually exclusively.  This means, we
 * are assuming create phase of XDAIS Socket Nodes at the same node
 * 'priority' cannot be scheduled concurrently.
 */
IALG_Handle DSKT2_createAlg(Int scratchMutexId, IALG_Fxns * fxns, 
        IALG_Handle parent, IALG_Params * params)
{
    Int     extHeapId;

    extHeapId = _DSKT2_spaceToSeg(IALG_ESDATA);
    DBC_assert(extHeapId >= 0);

    return (DSKT2_createAlg2(scratchMutexId, fxns, parent, params, extHeapId));
}

/*
 *  ======== DSKT_createAlg2 ========
 */
IALG_Handle DSKT2_createAlg2(Int scratchMutexId, IALG_Fxns * fxns, 
    IALG_Handle parent, IALG_Params * params, Int extHeapId)
{
    IALG_Fxns *fxnsPtr;
    IALG_Handle alg;
    Int numRecs;
    Int sizeofMemTab;
    IALG_MemRec *memTab;
    Int numActualRecs;
    Int i;

    DBC_require(extHeapId >= 0);

    _DSKT2_init();

    if (fxns != NULL) {

        /* determine size of the memTab structure */
        if (fxns->algNumAlloc != NULL) {
            numRecs = fxns->algNumAlloc();

        } else {
            /* 
             * Note: as per XDAIS Spec (SPRU360) algNumAlloc() is optional
             * and if it is not implemented, the maximum number of memory 
             * records for algAlloc() must be less than IALG_DEFMEMRECS.
             */
            numRecs = IALG_DEFMEMRECS;
        }

        /* allocate a memTab based on number of records alg specified */
        sizeofMemTab = numRecs * sizeof(IALG_MemRec);
        memTab = MEM_calloc(_DSKT2_heapId, sizeofMemTab, 0);

        DBG_printf2(TRACE, "DSCO: alloc'ed memTab = %x, size = %x", 
            (LgUns)memTab, sizeofMemTab);

        if (memTab != MEM_ILLEGAL) {
            /* call alg's algAlloc fxn to fill in memTab[]  */
            numActualRecs = fxns->algAlloc((IALG_Params *)params, &fxnsPtr, 
                memTab);

            DBG_printf1(TRACE, "DSCO: algAlloc returned %x", numActualRecs);

            /* allocate memory for alg */
            if ((numActualRecs <= 0) || 
                    (_DSKT2_assignInstanceMemory(scratchMutexId, memTab, 
                    numActualRecs, extHeapId) == FALSE)) {

                /* if failure abort create phase. */
                DBG_printf0(TRACE, "DSCO: ERROR assigning Instance Memory.");

                /* abort instance creation after freeing  memTab */
                if (MEM_free(_DSKT2_heapId, memTab, sizeofMemTab)) {
                    DBG_printf2(TRACE, "DSCO: free'ed memTab = %x, size = %x", 
                        (LgUns)memTab, sizeofMemTab);
 
                } else {
                    DBG_printf2(TRACE, "DSCO: ERROR free memTab= %x,size =%x", 
                        (LgUns)memTab, sizeofMemTab);
                } 

                return(NULL);
            }

            /* set alg object handle, and set instance's function pointer */
            alg = ((IALG_Handle)memTab[0].base);
            alg->fxns = fxns;

            /* stash away memTab (to an ext.mem queue) til delete phase */
            if (!_DSKT2_enqueueMemTab(_DSKT2_heapId, memTab, sizeofMemTab, 
                    numActualRecs, extHeapId)){
                
                /* if 'enqueue' fails abort create phase. */
                DBG_printf0(TRACE, "DSCO: enqueueMemTab FAILED.");
                DBG_printf0(TRACE, "DSCO: freeing algorithm memory");

                /* 
                 * Call freeAlg to free all instance memory, saved memTab recs
                 * and maintain the shared scratch data structures. 
                 */
                DSKT2_freeAlg(scratchMutexId, (IALG_Handle) alg);

                return(NULL);
            }               

            /* Writeback and invalidate cache, if function is specified */
            if (DSKT2_cacheWBInvFxn != NULL) {
                for (i = 0; i < numActualRecs; i++) {
                    (DSKT2_cacheWBInvFxn)(memTab[i].base, memTab[i].size,
                            TRUE);
                }
            }

            /* call alg initialize function with the memory it requested */
            /* if algInit successful return the alg object's handle */
            if (fxns->algInit(alg, memTab, NULL, (IALG_Params *)params) == 
                    IALG_EOK) {
                DBG_printf0(TRACE, "DSCO: algInit OK");

                if (_DSKT2_createCallbackFxn != NULL) { 
                    _DSKT2_createCallbackFxn(alg, memTab, numActualRecs, 
                        scratchMutexId);
                }
 
                return((IALG_Handle) alg);

            } else {
                DBG_printf0(TRACE, "DSCO: algInit FAILED.");
                DBG_printf0(TRACE, "DSCO: freeing algorithm memory");

                /* 
                 * Call freeAlg to free all instance memory, saved memTab recs
                 * and maintain the shared scratch data structures. 
                 */
                DSKT2_freeAlg(scratchMutexId, (IALG_Handle) alg);
            }
        }
    }

    return (NULL);
}

/*
 *  @(#) ti.sdo.fc.dskt2; 1, 0, 1,66; 5-10-2007 10:50:09; /db/atree/library/trees/fc-b12x/src/
 */

