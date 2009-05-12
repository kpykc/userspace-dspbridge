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
 *  ======== dskt2free.c ========
 *  DSKT2 API to free the memory used by an XDAIS algorithm instance, and then
 *  delete the instance.
 *
 */
#include <std.h>

#include <dbg.h>
#include <ti/xdais/ialg.h>
#include <mem.h>
#include <tsk.h>
#include <dskt2.h>
#include <_dskt2.h>

/*
 *  ======== DSKT2_freeAlg ========
 */
Bool DSKT2_freeAlg(Int scratchMutexId, IALG_Handle alg)
{
    Int numRecs;
    Int memTabSize;
    IALG_MemRec *memTab;
    Int extHeapId;
    Int status = TRUE;

    /*
     * If alg instance is currently active remove info from active table.
     * (No need to actually 'deactivate' the instance.)
     *
     * We need to do this early so pre-empting tasks know not to deactivate
     * alg.
     */
    if ((scratchMutexId >= 0) && (scratchMutexId < DSKT2_NUM_SCRATCH_GROUPS)) {
        TSK_disable();

        if (_DSKT2_activeAlg[scratchMutexId] == alg) {
            _DSKT2_activeAlg[scratchMutexId] = NULL;
        }
        else if (_DSKT2_lastActiveAlg[scratchMutexId] == alg) {
            _DSKT2_lastActiveAlg[scratchMutexId] = NULL;
        }

        TSK_enable();
    }

    /*
     * Use the memTab stored during create phase to deallocate instance
     * memory, as well as the memTab structure itself. 
     * No need to call alg's algFree function anymore.
     */ 
    memTab =_DSKT2_dequeueMemTab(_DSKT2_heapId, alg, &memTabSize, &numRecs,
            &extHeapId);

    if (memTab == MEM_ILLEGAL) {
        DBG_printf1(TRACE, "DSFO: DSKT2_freeAlg failing for: %x", alg);

        return FALSE; /* return without freeing any memory */
    }

    /* 
     * free algorithm's memory, using the scratch-group Id for determining 
     * shared scratch heap. 
     */
    status = _DSKT2_freeInstanceMemory(scratchMutexId, memTab, numRecs,
            extHeapId);

    /* done with stashed away memTab, free it */
    if (MEM_free(_DSKT2_heapId, memTab, memTabSize)) {
        DBG_printf2(TRACE, "DSFO: freed memTab = %x, size = %x",
            (LgUns)memTab, memTabSize);

    } else {
        DBG_printf2(TRACE, "DSFO: ERROR freeing memTab = %x, size = %x",
            (LgUns)memTab, memTabSize);
    }

    return (status);
}

/*
 *  @(#) ti.sdo.fc.dskt2; 1, 0, 1,66; 5-10-2007 10:50:09; /db/atree/library/trees/fc-b12x/src/
 */

