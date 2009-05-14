/*
 *  Copyright 2001-2008 Texas Instruments - http://www.ti.com/
 * 
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 *  ======== cnxdiag.c ========
 *  DSP side program to complement wcnxdiag.
 *
 *  This program accompanies the wcnxdiag.c DirectDSP sample program.
 *
 *  Usage:  cnxdiag
 *
 */

#include <std.h>
#include <mem.h>
#include <rms.h>
#include <rmsdefs.h>
#include <rms_sh.h>
#include <node.h>
#include <strm.h>
#include <sys.h>

/* Siocopy task object properties. */
typedef struct {
    STRM_Handle  outStream;      /* An SIO output channel. */
    Ptr         pBuf;           /* A data buffer. */
    Uns         uSegid;         /* Memory segment ID. */
    Int         nBufsize;       /* Size of data buffer. */
} CnxdiagObj;


/*
 *  ======== cnxdiagCreate ========
 */
RMS_STATUS cnxdiagCreate(Int argLength, Char * argData, Int numInStreams,
    RMS_WORD inDef[], Int numOutStreams, RMS_WORD outDef[],
    NODE_EnvPtr  node)
{
    RMS_StrmDef     * streamAttrs;
    STRM_Attrs      attrs = STRM_ATTRS;
    CnxdiagObj      * pCopyObj;
    RMS_STATUS      cResult = RMS_EOK;

    /* Allocate context structure for this instance of the siocopy object. */
    if ((pCopyObj = MEM_calloc(0, sizeof(CnxdiagObj), 0)) != NULL) {
        streamAttrs = (RMS_StrmDef *)outDef[0];

        attrs.nbufs = streamAttrs->nbufs;
        attrs.segid = streamAttrs->segid;
        attrs.align = streamAttrs->align;
        attrs.timeout = streamAttrs->timeout;
        /* Initialize remainder of context structure. */
        pCopyObj->uSegid = streamAttrs->segid;
        pCopyObj->nBufsize = streamAttrs->bufsize;

        SYS_printf("cnxdiagCreate: output stream = %s\n", streamAttrs->name);
        SYS_printf("  bufsize 0x%x, nbufs %d, segid %d, timeout %d\n",
            streamAttrs->bufsize, streamAttrs->nbufs, streamAttrs->segid,
            streamAttrs->timeout);

        pCopyObj->outStream = STRM_create(streamAttrs->name, STRM_OUTPUT,
            streamAttrs->bufsize, &attrs);

        if(pCopyObj->outStream == NULL) {
            SYS_printf("cnxdiagCreate: Failed to Create the Stream\n");
            return(RMS_ESTREAM);
        }
        pCopyObj->pBuf = STRM_allocateBuffer(pCopyObj->outStream, \
            streamAttrs->bufsize);

        /* Verify allocations; if failure, free all allocated objects. */
        if (pCopyObj->pBuf != MEM_ILLEGAL){
       /*
        * Return pointer to siocopy context structure for use in execute
        * and delete phases.
        */
            node->moreEnv = (Ptr)pCopyObj;
        }
        else {
            /* Free all allocated objects. */
            SYS_printf("cnxdiagCreate: Failed to Allocate Stream Buffer\n");
            if (pCopyObj->outStream != NULL) {
                STRM_delete(pCopyObj->outStream);
            }
            cResult = RMS_EOUTOFMEMORY;
        }
    }
    else {
        /* If unable to allocate object, return RMS_E_OUTOFMEMORY. */
        cResult = RMS_EOUTOFMEMORY;
        SYS_printf("cnxdiagCreate: Failed to Allocate Context structure\n");
    }

    return (cResult);
}


/*
 *  ======== cnxdiagDelete ========
 */
RMS_STATUS cnxdiagDelete(NODE_EnvPtr node)
{
    CnxdiagObj      *pCopyObj;

    /* Dereference context structure stored in the env variable. */
    pCopyObj = (CnxdiagObj*)node->moreEnv;

    /* Free stream buffers. */
    if (pCopyObj->outStream != NULL) {

        if(pCopyObj->pBuf != NULL){
            STRM_freeBuffer(pCopyObj->outStream, pCopyObj->pBuf, \
                pCopyObj->nBufsize);
        }

        STRM_idle(pCopyObj->outStream, TRUE);
        STRM_delete(pCopyObj->outStream);
    }

    MEM_free(0, pCopyObj, sizeof(CnxdiagObj));

    return (RMS_EOK);
}


/*
 *  ======== cnxdiagExecute ========
 */
RMS_STATUS cnxdiagExecute(NODE_EnvPtr env)
{
    Int         status = 0;
    CnxdiagObj  *pCopyObj;
    Arg         bufArg;
    Int         nBufsize;
    Int         * iPtr;
    Uns         i;
    RMS_DSPMSG  msg;
    Uns         msgReady;
    Uns         readyStreams;

    /* Dereference context structure stored in the env variable. */
    pCopyObj = (CnxdiagObj*)env->moreEnv;
    nBufsize = pCopyObj->nBufsize;

    /* Fill buffer to send to GPP. */
    iPtr = (Int *)pCopyObj->pBuf;
    for (i = 2; i < nBufsize; i++) {
        *iPtr++ = i;
    }

    /* Initialize msg structure. */
    msg.cmd = ~(RMS_EXIT);

    /* Issue the pre-pared buffer to the output stream. */
    STRM_issue(pCopyObj->outStream, pCopyObj->pBuf, nBufsize, nBufsize, NULL);

    for (;;) {

        /* Wait until input stream is ready, or until GPP message arrives. */
        readyStreams = NODE_wait(env, &pCopyObj->outStream, 1,
            NODE_FOREVER, &msgReady);

        /* If a message has arrived. */
        if (msgReady != 0) {

            /* Get message. */
            NODE_getMsg(env, &msg, 0);

            if (msg.cmd == RMS_EXIT) {
                SYS_printf("Got RMS_EXIT\n");
                break;
            }
        }

        /* If a buffer is ready on the output stream. */
        if (readyStreams != 0) {

            /* Reclaim an empty buffer from the output stream. */
            status = STRM_reclaim(pCopyObj->outStream,
                &pCopyObj->pBuf, NULL, &bufArg);

            if (status < 0) {
                SYS_printf("STRM_reclaim error: %d\n", status);
            }
        }
    }

    STRM_idle(pCopyObj->outStream, TRUE);

    return (RMS_EOK);
}


