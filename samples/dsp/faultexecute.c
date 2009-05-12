/*
 *! ============================================================
 *! Copyright 2005 by Texas Instruments Incorporated.
 *! All rights reserved. Property of Texas Instruments Incorporated.
 *! Restricted rights to use, duplicate or disclose this code are
 *! granted through contract.
 *! ============================================================
 */

/*
 *  ======== faultExecute.c =========
 *
 *! Jan 24,2003  mk  Created from fault.c.
 *! Oct 07,2003  connell  Removed redundant create and delete fxns.
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
                         2 * sizeof(Int), pFaultObj->nInBufsize, 0);

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
                    for (x = 0; x < outBufSize/sizeof(Int); x++)
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
                }
		else
                {
                    Int x;

                    for (x = 0; x < outBufSize/sizeof(Int); x++)
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

