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
 *  ======== fault.c =========
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
#include "faultTest.h"

/*
 *  ======== faultCreate ========
 */
RMS_STATUS faultCreate(Int argLength, Char * argData, Int numInStreams,
                       RMS_StrmDef * inDef[], Int numOutStreams,
                       RMS_StrmDef * outDef[], NODE_EnvPtr env)
{
    STRM_Attrs      strmAttrs = STRM_ATTRS;
    FaultObj        *pFaultObj;
    RMS_STATUS      cResult = RMS_EOK;

    if (numInStreams != 1 || numOutStreams != 1)
    {
        return(RMS_EFAIL);
    }

    /* Allocate context structure for this instance of the fault object. */
    if ((pFaultObj = MEM_calloc(0, sizeof(FaultObj), 0)) != NULL)
    {

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


/*
 *  ======== faultExecute ========
 */
RMS_STATUS faultExecute(NODE_EnvPtr env)
{
    Int             datalength;
    Int             status = 0;
    FaultObj       *pFaultObj;
    Arg             bufArg;
    Int             inBufSize, outBufSize;
    Uns             readyStreams;
    RMS_DSPMSG      msg;
    Uns             msgReady;
    Bool            firstFrame = TRUE;
    Bool            firstTime = TRUE;

    /* Dereference context structure stored in the env variable. */
    pFaultObj = (FaultObj*)env->moreEnv;

    SYS_printf("Waiting for first buffer from host...\n");

    /* receive buffer */

    status = STRM_issue (pFaultObj->inStream, pFaultObj->pInBuf,
                         2, pFaultObj->nInBufsize, 0);

    for (;;)
    {
        /* Wait until input stream is ready, or until GPP message arrives. */
        readyStreams = NODE_wait(env, &pFaultObj->inStream, 2,
                                 NODE_FOREVER, &msgReady);


        /* If a message has arrived. */
        if (msgReady != 0)
        {
            /* Get message. */
            NODE_getMsg(env, &msg, 0);

            if (msg.cmd == RMS_EXIT)
            {
                break;
            }
        }

        if (readyStreams != 0)
        {
/*            if (pFaultObj->faultParams.phase == EXECUTE_PHASE)
            {
                pFaultObj->faultParams.count--;
            } */

            if (firstFrame == TRUE)
            {
                datalength = STRM_reclaim (pFaultObj->inStream,
                                           &pFaultObj->pInBuf, NULL, &bufArg);
                if (datalength <= 0)
                {
                    SYS_printf("STRM_reclaim error: %d\n", status);
                    for (;;);
                }
                outBufSize = *(Int *)pFaultObj->pInBuf;
                inBufSize = *((Int *)pFaultObj->pInBuf + 1);


                if (inBufSize > 0)
                {
                    Int x;
                    status = STRM_issue (pFaultObj->inStream, pFaultObj->pInBuf,
                                         inBufSize, pFaultObj->nInBufsize, 0);
                    for (x = 0; x < outBufSize; x++)
                    {
                        *((Int *)pFaultObj->pOutBuf + x) =
                        *((Int *)pFaultObj->pInBuf + x) * 2 ;
                    }
                }

                if (outBufSize > 0)
                {
                    status = STRM_issue (pFaultObj->outStream, pFaultObj->pOutBuf,
                                         outBufSize, pFaultObj->nOutBufsize,0);
                }

                firstFrame = FALSE;
            }

            if (inBufSize > 0 && !firstTime)
            {
            if (pFaultObj->faultParams.phase == EXECUTE_PHASE)
                {
                    pFaultObj->faultParams.count--;
                }
        if (pFaultObj->faultParams.phase == EXECUTE_PHASE &&
                    pFaultObj->faultParams.errPos == BEFORE_RECLAIM &&
                    !(pFaultObj->faultParams.count))
                {
                    generate_fault(pFaultObj);
                    pFaultObj->faultParams.count--;
                }

                datalength = STRM_reclaim (pFaultObj->inStream,
                                           &pFaultObj->pInBuf, NULL, &bufArg);

                if (pFaultObj->faultParams.phase == EXECUTE_PHASE &&
                    pFaultObj->faultParams.errPos == AFTER_RECLAIM &&
                    !(pFaultObj->faultParams.count))
                {
                    generate_fault(pFaultObj);
                    pFaultObj->faultParams.count--;
                }

                if (datalength < 0)
                {
                    SYS_printf("STRM_reclaim error: %d\n", datalength);
                    for (;;);
                } else
                {
                    Int x;

                    for (x = 0; x < outBufSize; x++)
                    {
                        *((Int *)pFaultObj->pOutBuf + x) =
                        *((Int *)pFaultObj->pInBuf + x) * 2 ;
                    }
                }

                status = STRM_issue (pFaultObj->inStream, pFaultObj->pInBuf,
                                     inBufSize, pFaultObj->nInBufsize, 0);

                if (pFaultObj->faultParams.phase == EXECUTE_PHASE &&
                    pFaultObj->faultParams.errPos == AFTER_ISSUE &&
                    !(pFaultObj->faultParams.count))
                {
                    generate_fault(pFaultObj);
                    pFaultObj->faultParams.count--;
                }
                if (outBufSize > 0  && !firstTime)
                {
                if (pFaultObj->faultParams.phase == EXECUTE_PHASE)
                    {
                        pFaultObj->faultParams.count--;
                    }
                    if (pFaultObj->faultParams.phase == EXECUTE_PHASE &&
                        pFaultObj->faultParams.errPos == BEFORE_RECLAIM &&
                        !(pFaultObj->faultParams.count))
                    {
                        generate_fault(pFaultObj);
                        pFaultObj->faultParams.count--;
                    }

                    datalength = STRM_reclaim (pFaultObj->outStream,
                                               &pFaultObj->pOutBuf, NULL, &bufArg);

                    if (pFaultObj->faultParams.phase == EXECUTE_PHASE &&
                        pFaultObj->faultParams.errPos == AFTER_RECLAIM &&
                        !(pFaultObj->faultParams.count))
                    {
                        generate_fault(pFaultObj);
                        pFaultObj->faultParams.count--;
                    }

                    if (datalength < 0)
                    {
                        SYS_printf("STRM_reclaim error: %d\n", datalength);
                        for (;;);
                    }


                    /* send a buffer of size (buf_in[0]) to the ARM */

                    status = STRM_issue (pFaultObj->outStream, pFaultObj->pOutBuf, outBufSize, pFaultObj->nOutBufsize, 0);

                    if (pFaultObj->faultParams.phase == EXECUTE_PHASE &&
                        pFaultObj->faultParams.errPos == AFTER_ISSUE &&
                        !(pFaultObj->faultParams.count))
                    {
                        generate_fault(pFaultObj);
                        pFaultObj->faultParams.count--;
                    }

                    if (status != SYS_OK)
                    {
                        SYS_printf("STRM_issue error: %d\n", status);
                        for (;;);
                    }


                }


                if (status != SYS_OK)
                {
                    SYS_printf("STRM_issue error: %d\n", status);
                    for (;;);
                }

            }

            if (firstTime)
            {
                firstTime = FALSE;
            }

        } /* if readyStreams */

    } /* for (;;) */

    if (inBufSize > 0)
    {
        STRM_idle(pFaultObj->inStream, TRUE);
        STRM_reclaim(pFaultObj->inStream, (Ptr)&pFaultObj->pInBuf, NULL, &bufArg);
    }
    if (outBufSize > 0)
    {
        STRM_idle(pFaultObj->outStream, TRUE);
        STRM_reclaim(pFaultObj->outStream, (Ptr)&pFaultObj->pOutBuf, NULL, &bufArg);
    }

    return(RMS_EOK);
}

void generate_fault(FaultObj *pFaultObj)
{
    Int     i;
    MdUns   val;

    switch (pFaultObj->faultParams.errType)
    {

    case MMU_FAULT_READ:
        /* Attempt to read from invalid address */
        for (i = 0; i < pFaultObj->faultParams.nMMUFaults; i++)
        {
            val = *((short *) 0x480000);
            /* Using bitwise or to eliminate compiler warning. */
            val |= val;
        }
        break;
    case DSP_GEN_ERROR:
        generate_message(pFaultObj);
        SYS_error(pFaultObj->errorMessage,TEST_EXCEPTION_VALUE);
        break;
    case MMU_FAULT_BRANCH:
        /* Branch to invalid address */

#ifdef _2430_
	// there is no "goto" command in the 64x assembly language, so
	// store invalid address in a register, then branch to it:

	//store address 0x009000000 into register B4
	asm(" MVKL .S2 0x8000, B4");
	asm(" MVKLH .S2 0x400F, B4");
	//now branch to this invalid address
	asm(" B .S2 B4");
	asm(" NOP 5");

#else
        asm(" goto 0x900000");
#endif
        break;
    }
}

void generate_message(FaultObj *pFaultObj)
{
    char *phaseMsg, *errPosMsg;

    switch (pFaultObj->faultParams.phase)
    {
    case CREATE_PHASE:
        phaseMsg = CREATE_PHASE_STRING;
        break;
    case EXECUTE_PHASE:
        phaseMsg = EXECUTE_PHASE_STRING;
        break;
    case DELETE_PHASE:
        phaseMsg = DELETE_PHASE_STRING;
        break;
    }

    if (pFaultObj->faultParams.phase == EXECUTE_PHASE)
    {
        switch (pFaultObj->faultParams.errPos)
        {
        case BEFORE_RECLAIM:
            errPosMsg = BEFORE_RECLAIM_STRING;
            break;
        case AFTER_RECLAIM:
            errPosMsg = AFTER_RECLAIM_STRING;
            break;
        case AFTER_ISSUE:
            errPosMsg = AFTER_ISSUE_STRING;
            break;
        }
    } else
    {
        switch (pFaultObj->faultParams.errPos)
        {
        case START:
            errPosMsg = START_STRING;
            break;
        case MIDDLE:
            errPosMsg = MIDDLE_STRING;
            break;
        case END:
            errPosMsg = END_STRING;
            break;
        }
    }

    pFaultObj->errorMessage = strncat(DSP_GEN_ERROR_STRING, phaseMsg, 30);
    pFaultObj->errorMessage = strncat(pFaultObj->errorMessage, errPosMsg, 30);

}



