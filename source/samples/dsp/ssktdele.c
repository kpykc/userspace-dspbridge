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
 *  ======== ssktdele.c ========
 *  xDAIS socket delete function.
 *
 */
#include <std.h>

#include <mem.h>
#include <rmstypes.h>
#include <rms_sh.h>
#include <dskt2.h>
#include <node.h>
#include <strm.h>
#include "sskt.h"

#include <sys.h>

#define TRACE &trace


/*
 *  ======== SCALESOCKET_TI_delete ========
 */
RMS_STATUS SCALESOCKET_TI_delete(NODE_EnvPtr node)
{
    struct SSKT_Obj * dataPtr = node->moreEnv;
    Int             nodePriority;


    if ((node != NULL) && (dataPtr != NULL)) {

        /* delete stream data buffers */
        if (dataPtr->inBuf != NULL) {
            STRM_freeBuffer(dataPtr->inStream, dataPtr->inBuf, dataPtr->inSize);
        }
        if (dataPtr->outBuf != NULL) {
            STRM_freeBuffer(dataPtr->outStream, dataPtr->outBuf,
            dataPtr->outSize);
        }

        /* delete streams */
        if (dataPtr->inStream != NULL) {
            STRM_delete(dataPtr->inStream);
        }
        if (dataPtr->outStream != NULL) {
            STRM_delete(dataPtr->outStream);
        }

        /* free the algorithm instance object */
        nodePriority = NODE_getPri(node);
        DSKT2_freeAlg(nodePriority, dataPtr->algHandle);

        /* free the socket node's context structure */
        MEM_free(dataPtr->segId, dataPtr, sizeof(SSKT_Obj));
    }

    return(RMS_EOK);
}
