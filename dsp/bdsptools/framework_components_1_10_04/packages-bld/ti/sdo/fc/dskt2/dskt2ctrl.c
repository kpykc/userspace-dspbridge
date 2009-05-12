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
 *  ======== dskt2ctrl.c ========
 *  Function to simplify call to algorithm's algControl function.
 *
 */
#include <std.h>

#include <ti/xdais/ialg.h>
#include <dskt2.h>


/*
 *  ======== DSKT2_controlAlg ========
 */
Int DSKT2_controlAlg(IALG_Handle alg, IALG_Cmd cmd, 
    IALG_Status *status)
{
    Int retVal = IALG_EFAIL;

    if ((alg != NULL) && (status != NULL) && (alg->fxns->algControl != NULL)) {
        retVal = alg->fxns->algControl(alg, cmd, status);
    }

    return(retVal);
}

/*
 *  @(#) ti.sdo.fc.dskt2; 1, 0, 1,66; 5-10-2007 10:50:09; /db/atree/library/trees/fc-b12x/src/
 */

