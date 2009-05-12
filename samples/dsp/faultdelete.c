/*
 *  Copyright 2000 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  U.S. Patent Nos. 5,283,900  5,392,448
 */
/* "@(#) DSPBridge 0.85 05-25-00 (__imports)" */
/*
 *  ======== faultDelete.c =========
 *
 *! Jan 24,2003  mk  Created from fault.c.
 *! Oct 07,2003  connell  Removed redundant create and execute fxns.
 *
 */


#include <std.h>
#include <stdlib.h>
#include <string.h>
#include <mem.h>
#include <strm.h>
#include <tsk.h>
#include <rms.h>
#include <node.h>
#include <sys.h>
#include <log.h>
#include "faultTest.h"

/*
 *  ======== faultDelete ========
 */
RMS_STATUS faultDelete(NODE_EnvPtr env)
{
    FaultObj      *pFaultObj;

    /* Dereference context structure stored in the env variable. */
    pFaultObj = (FaultObj*)env->moreEnv;


    if (pFaultObj->faultParams.phase == DELETE_PHASE && !(pFaultObj->faultParams.count--))
    {
        generate_fault(pFaultObj);
    }
    if (pFaultObj)
    {
        /* Free streams. */
        if (pFaultObj->inStream != NULL)
        {
            STRM_idle(pFaultObj->inStream, TRUE);
            STRM_delete(pFaultObj->inStream);
        }
        if (pFaultObj->outStream !=NULL)
        {
            STRM_idle(pFaultObj->outStream, TRUE);
            STRM_delete(pFaultObj->outStream);
        }

        if (pFaultObj->faultParams.phase == DELETE_PHASE && !(pFaultObj->faultParams.count--))
        {
            generate_fault(pFaultObj);
        }

        /* Free stream buffers. */

        if (pFaultObj->pInBuf != NULL)
        {
            STRM_freeBuffer(pFaultObj->inStream, pFaultObj->pInBuf, pFaultObj->nInBufsize);
        }
        if (pFaultObj->pOutBuf != NULL)
        {
            STRM_freeBuffer(pFaultObj->outStream, pFaultObj->pOutBuf, pFaultObj->nOutBufsize);
        }

        MEM_free(0, pFaultObj, sizeof(FaultObj));

        if (pFaultObj->faultParams.phase == DELETE_PHASE && !(pFaultObj->faultParams.count--))
        {
            generate_fault(pFaultObj);
        }
    }

    return(RMS_EOK);
}

