/*
 *! ============================================================
 *! Copyright 2004 by Texas Instruments Incorporated.
 *! All rights reserved. Property of Texas Instruments Incorporated.
 *! Restricted rights to use, duplicate or disclose this code are
 *! granted through contract.
 *! ============================================================
 */
/*
 *  Copyright 2000 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  U.S. Patent Nos. 5,283,900  5,392,448
 */
/* "@(#) DSPBridge 0.85 05-25-00 (__imports)" */
/*
 *  ======== faultCreate.c =========
 *
 *! Jan 24,2003  mk  Created from fault.c.
 *! Oct 07,2003  connell  Removed redundant delete and execute fxns.
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
 *  ======== faultCreate ========
 */
RMS_STATUS faultCreate(Int argLength, Char * argData, Int numInStreams,
                       RMS_StrmDef * inDef[], Int numOutStreams,
                       RMS_StrmDef * outDef[], NODE_EnvPtr env)
{
    STRM_Attrs     strmAttrs = STRM_ATTRS;
    FaultObj      *pFaultObj;
    RMS_STATUS     cResult = RMS_EOK;

    volatile MdUns   val;


	//generate fault
    val = *((short *) 0x480000);


    if (numInStreams != 1 || numOutStreams != 1)
    {
        return(RMS_EFAIL);
    }

    /* Allocate context structure for this instance of the fault object. */
    if ((pFaultObj = MEM_calloc(0, sizeof(FaultObj), 0)) != NULL)
    {
	/*
	 * This code will be fine for 55 and 64P since an MdUns is 16-bit on both cpus
	 */
        pFaultObj->faultParams.errType = (MdUns)((FaultParams *)argData)->errType;
        pFaultObj->faultParams.phase = (MdUns)((FaultParams *)argData)->phase;
        pFaultObj->faultParams.count = (MdUns)((FaultParams *)argData)->count;
        pFaultObj->faultParams.errPos = (MdUns)((FaultParams *)argData)->errPos;
        pFaultObj->faultParams.nMMUFaults = (MdUns)((FaultParams *)argData)->nMMUFaults;


        if ((pFaultObj->faultParams.phase == CREATE_PHASE) && !(pFaultObj->faultParams.count--))
        {
            generate_fault(pFaultObj);
        }


        strmAttrs.nbufs = inDef[0]->nbufs;
        strmAttrs.segid = inDef[0]->segid;
        strmAttrs.timeout = inDef[0]->timeout;

        /* Allocate input and output streams: */
        pFaultObj->inStream = STRM_create(inDef[0]->name, STRM_INPUT,
                                          inDef[0]->bufsize, &strmAttrs);

        strmAttrs.nbufs = outDef[0]->nbufs;
        strmAttrs.segid = outDef[0]->segid;
        strmAttrs.timeout = outDef[0]->timeout;

        pFaultObj->outStream = STRM_create(outDef[0]->name, STRM_OUTPUT,
                                           outDef[0]->bufsize, &strmAttrs);

        if (pFaultObj->faultParams.phase == CREATE_PHASE && !(pFaultObj->faultParams.count--))
        {
            generate_fault(pFaultObj);
        }

        /* Allocate a data buffer for SIO exchange. */
        pFaultObj->pInBuf = STRM_allocateBuffer(pFaultObj->inStream, inDef[0]->bufsize);
        pFaultObj->pOutBuf = STRM_allocateBuffer(pFaultObj->outStream, outDef[0]->bufsize);

        /* Initialize remainder of context structure. */
        pFaultObj->uSegid = inDef[0]->segid;
        pFaultObj->nInBufsize = inDef[0]->bufsize;
        pFaultObj->nOutBufsize = outDef[0]->bufsize;

        /* Verify allocations; if failure, free all allocated objects. */
        if ((pFaultObj->inStream != NULL) && (pFaultObj->outStream != NULL)
            && (pFaultObj->pInBuf != NULL)
            && (pFaultObj->pOutBuf != NULL))
        {
            /*
             * Return pointer to fault context structure for use in execute
             * and delete phases.
             */
            env->moreEnv = (Ptr)pFaultObj;
        } else
        {
            cResult = RMS_EOUTOFMEMORY;
        }
    } else
    {
        /* If unable to allocate object, return RMS_E_OUTOFMEMORY. */
        cResult = RMS_EOUTOFMEMORY;
    }

    if (pFaultObj->faultParams.phase == CREATE_PHASE && !(pFaultObj->faultParams.count--))
    {
        generate_fault(pFaultObj);
    }

    return(cResult);
}

