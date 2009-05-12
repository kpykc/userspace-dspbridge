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
 *  ======== dskt2dact.c ========
 *  Function to simplify call to algorithm's algDeactivate function.
 *
 */
#include <std.h>

#include <dbc.h>
#include <ti/xdais/ialg.h>
#include <dskt2.h>
#include <_dskt2.h>


/*
 * ======== DSKT2_deactivateAlg ======== 
 *
 * Performs 'optimistic' algorithm activation/deactivation!  
 * That is: alg 'deactivation' does not happen until  some *other* algorithm
 * instance needs to be activated.  
 * When activating the new algorithm, the former (i.e.'current') active 
 * instance gets deactivated.
 */
Void DSKT2_deactivateAlg(Int scratchId, IALG_Handle alg)
{
    Bool isScratch;

    isScratch = (scratchId >= 0) && (scratchId < DSKT2_NUM_SCRATCH_GROUPS);

    if (isScratch) {
        _DSKT2_lastActiveAlg[scratchId] = alg;
        _DSKT2_activeAlg[scratchId] = NULL;
    }
    else {
        if ((alg != NULL) && (alg->fxns->algDeactivate != NULL)) {
            /* Deactivate if alg is not using scratch */
            alg->fxns->algDeactivate(alg);
        }
    }
}

/*
 *  ======== DSKT2_deactivateAll ========
 */
Int DSKT2_deactivateAll()
{
    IALG_Handle alg;
    Int         i;
    Int         nActive = 0;

    /*
     *  Deactivate all algs that have been lazily deactivated.
     */
    for (i = 0; i < DSKT2_NUM_SCRATCH_GROUPS; i++) {
        if ((alg = _DSKT2_lastActiveAlg[i]) != NULL) {
            if (alg->fxns->algDeactivate != NULL) {
                alg->fxns->algDeactivate(alg);
            }
            _DSKT2_lastActiveAlg[i] = NULL;
        }
        if (_DSKT2_activeAlg[i] != NULL) {
            nActive++;
        }
    }

    return (nActive);
}

/*
 *  @(#) ti.sdo.fc.dskt2; 1, 0, 1,66; 5-10-2007 10:50:09; /db/atree/library/trees/fc-b12x/src/
 */

