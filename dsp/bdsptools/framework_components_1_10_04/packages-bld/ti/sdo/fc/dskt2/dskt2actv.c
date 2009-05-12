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
 *  ======== dskt2actv.c ========
 *  Function to simplify call to algorithm's algActivate function.
 *
 */
#include <std.h>

#include <dbc.h>
#include <ti/xdais/ialg.h>
#include <dskt2.h>
#include <_dskt2.h>


/*
 *  ======== DSKT2_activateAlg ========
 *
 * Does 'optimistic' algorithm activation/deactivation!
 * That is, we defer instance 'deactivation' until when we need to
 * 'activate' a *new* algorithm instance. At that point
 * the 'current' active instance  gets deactivated, and the new instance
 * get activated.
 */
Void DSKT2_activateAlg(Int scratchMutexId, IALG_Handle alg)
{
    IALG_Handle lastActiveAlg = NULL;
    Bool validScratchRange;

    validScratchRange = (scratchMutexId >= 0) && (scratchMutexId <
        DSKT2_NUM_SCRATCH_GROUPS);

    /* Use the scratchMutexId for determining shared scratch heap */
    if (validScratchRange) {
        lastActiveAlg = _DSKT2_lastActiveAlg[scratchMutexId];

        /*
         *  lastActiveAlg will either be deactivated or become the current
         *  activate alg, so we can set _DSKT2_lastActiveAlg[scratchMutexId]
         *  to NULL.
         *
         *  Should we check for _DSKT2_activeAlg[scratchMutexId] != NULL?
         *  (ie, somebody didn't call DSKT2_deactivateAlg())
         */
        _DSKT2_lastActiveAlg[scratchMutexId] = NULL;
        _DSKT2_activeAlg[scratchMutexId] = alg;
    }

    if (lastActiveAlg != alg) {

        /*
         * At given scratch group, if this instance is not the currently
         *  active alg instance, deactivate any existing 'current' active
         */
        if (lastActiveAlg != NULL) {
            /*
             *  lastActiveAlg has been 'lazily' deactivated. Now we need
             *  to really deactivate it.
             */
            if (lastActiveAlg->fxns->algDeactivate != NULL) {
                lastActiveAlg->fxns->algDeactivate(lastActiveAlg);
            }
        }

        if ((alg != NULL) && (alg->fxns->algActivate != NULL)) {
            /*
             * 'activate' current instance & make it the current active
             * instance for this scratch group.
             */
            alg->fxns->algActivate(alg);
        }
    }
    /* else current algorithm instance is already active/or does not need
     * activation
     */
}

/*
 *  @(#) ti.sdo.fc.dskt2; 1, 0, 1,66; 5-10-2007 10:50:09; /db/atree/library/trees/fc-b12x/src/
 */

