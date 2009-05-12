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
 *  ======== ssktcrea.c ========
 *  xDAIS socket create function.
 *
 */
#include <std.h>

#include <mem.h>
#include <rmstypes.h>
#include <rms_sh.h>
#include <dskt2.h>
#include "sskt.h"
#include "scale_ti.h"

#define TRACE &trace


/*
 *  ======== _createStream (deprecated DSKT_createStream) ========
 */
STRM_Handle _createStream(RMS_StrmDef *def, STRM_DIRECTION dir, Uns *size,
        Int segId)
{
    STRM_Attrs attrs = STRM_ATTRS;

    attrs.nbufs = def->nbufs;

    /*
     *  If an external heap was assigned to the node, use it for the stream
     *  buffers. Otherwise, use the heap in the RMS_StrmDef.
     */
    attrs.segid = (segId > 0) ? segId : def->segid;

    attrs.timeout = def->timeout;
    *size = (Uns)def->bufsize;


    return (STRM_create(def->name, dir, def->bufsize, &attrs));
}


/*
 *  ======== SCALESOCKET_TI_create ========
 */
RMS_STATUS SCALESOCKET_TI_create(Int argLength, Char * argData,
    IALG_Fxns *algFxns, Int numInStreams, RMS_WORD inDef[], Int numOutStreams,
    RMS_WORD outDef[], NODE_EnvPtr node)
{
    struct SSKT_Obj *dataPtr;
    Int nodePriority;
    Int segId;

    /* Using bitwise OR to prevent compiler warning */
    argLength |= argLength;
    *argData |= *argData;
    numInStreams |= numInStreams;
    numOutStreams |= numOutStreams;

    /* Get the node's external heap segment Id, if it exists */
    segId = NODE_getHeapSeg(node);

    if (segId < 0) {
        /*
         *  Use heap 0 to allocate the node object if no external heap
         *  was defined for the node.
         */
        segId = 0;
    }


    if ((dataPtr = MEM_calloc(segId, sizeof(SSKT_Obj), 0)) != MEM_ILLEGAL) {

        /* attach socket's context structure to the node environment */
        node->moreEnv = dataPtr;

        /* Save the external heap segment for delete phase. */
        dataPtr->segId = segId;


        /* parse stream definition params, create input stream */
        dataPtr->inStream = _createStream((RMS_StrmDef *)inDef[0],
            STRM_INPUT, &dataPtr->inSize, segId);


        /* parse stream definition params, create output stream */
        dataPtr->outStream = _createStream((RMS_StrmDef *)outDef[0],
            STRM_OUTPUT, &dataPtr->outSize, segId);

        /* check for stream creation failure */
        if((dataPtr->inStream == NULL) || (dataPtr->outStream == NULL)) {
            return (RMS_ESTREAM);
        }


        /* allocate data buffers */
        dataPtr->inBuf = STRM_allocateBuffer(dataPtr->inStream,
            dataPtr->inSize);
        dataPtr->outBuf = STRM_allocateBuffer(dataPtr->outStream,
            dataPtr->outSize);

        /* check for buffer allocation failure */
        if((dataPtr->inBuf == NULL) || (dataPtr->outBuf == NULL)) {
            return (RMS_EOUTOFMEMORY);
        }


        /* create an algorithm instance object */
        nodePriority = NODE_getPri(node);

        if (segId > 0) {
            dataPtr->algHandle = DSKT2_createAlg2(nodePriority,
                    (IALG_Fxns *)algFxns, (IALG_Handle)NULL,
                    (IALG_Params *)NULL, segId);
        }
        else {
            dataPtr->algHandle = DSKT2_createAlg(nodePriority,
                    (IALG_Fxns *)algFxns, (IALG_Handle)NULL,
                    (IALG_Params *)NULL);
        }

        /* check for algorithm instance creation failure */
        if (dataPtr->algHandle == NULL) {
            return (RMS_EOUTOFMEMORY);
        }

        /* activation done during exec phase */
        // dataPtr->algHandle->fxns->algActivate(dataPtr->algHandle);

        return (RMS_EOK);
    }
    else {
        return (RMS_EOUTOFMEMORY);
    }
}
