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
 *  ======== ssktexec.c ========
 *  xDAIS socket execute function.
 *
 */
#include <std.h>

#include <node.h>
#include <rmstypes.h>
#include <rms_sh.h>
#include <dskt2.h>
#include "sskt.h"
#include "iscale.h"

#define TRACE &trace

/*
 *  ======== SCALESOCKET_TI_execute ========
 */
RMS_STATUS SCALESOCKET_TI_execute(NODE_EnvPtr node)
{
    struct SSKT_Obj *dataPtr = node->moreEnv;
    Bool firstFrame = TRUE;
    ISCALE_Status status;
    Uns readyStreams;
    RMS_DSPMSG msg;
    Uns msgsReady;
    Arg arg;


    /* prime the input stream with an empty buffer */
    STRM_issue(dataPtr->inStream, dataPtr->inBuf,dataPtr->inSize,
        dataPtr->inSize, 0);

    for (;;) {

        /* wait until input stream is ready, or until GPP message arrives */
        readyStreams = NODE_wait(node, &dataPtr->inStream, 1, NODE_FOREVER,
            &msgsReady);

        /* if a message has arrived: */
        if (msgsReady != 0) {

            /* get the message */
            NODE_getMsg(node, &msg, 0);

            /* if got CMD_SETSCALE, invoke alg's algControl function */
            if(msg.cmd == CMD_SETSCALE) {
                status.scaleFactor = (Int) msg.arg1;

                /* call algControl to change the alg scale factor */
                DSKT2_controlAlg(dataPtr->algHandle,
                        (IALG_Cmd)ISCALE_SETSCALE, (IALG_Status *)&status);

                msg.cmd = CMD_ACKSCALE;
                NODE_putMsg(node, NODE_TOGPP, &msg, 1000);
            }

            /* else if got RMS_EXIT then exit the execute-phase */
            else if(msg.cmd == RMS_EXIT) {
                break;
            }
        }

        /* if an buffer is ready on the input stream: */
        if (readyStreams != 0) {

            /* get the buffer from the input stream */
            STRM_reclaim(dataPtr->inStream, (Ptr *) &dataPtr->inBuf, NULL, &arg);

            /* if this is the first iteration use the output buffer directly */
            if (firstFrame == TRUE) {
                firstFrame = FALSE;
            }
            /* else reclaim empty buffer from output stream */
            else {
                STRM_reclaim(dataPtr->outStream, (Ptr *)&dataPtr->outBuf,
                    NULL, &arg);
            }

            /* activate the algorithm */
            DSKT2_activateAlg(NODE_getPri(node), dataPtr->algHandle);

            /* call the alg's scale function to tranform the input buffer */
            ((ISCALE_Handle)(dataPtr->algHandle))->fxns->scale(
                (ISCALE_Handle)dataPtr->algHandle, dataPtr->inBuf,
                dataPtr->outBuf, dataPtr->inSize);

            /* deactivate the algorithm */
            DSKT2_deactivateAlg(NODE_getPri(node), dataPtr->algHandle);

            /* send the output buffer to the output stream */
            STRM_issue(dataPtr->outStream, dataPtr->outBuf, dataPtr->inSize,
                dataPtr->inSize, 0);

            /* send the empty input buffer back to the input stream */
            STRM_issue(dataPtr->inStream, dataPtr->inBuf, dataPtr->inSize,
                dataPtr->inSize, 0);
        }
    }

    /* idle both streams */
    STRM_idle(dataPtr->inStream, TRUE);
    STRM_idle(dataPtr->outStream, TRUE);

    /* reclaim any buffers remaining in the streams */
    STRM_reclaim(dataPtr->inStream, (Ptr *) &dataPtr->inBuf, NULL, &arg);
    if (firstFrame == FALSE) {
        STRM_reclaim(dataPtr->outStream, (Ptr *) &dataPtr->outBuf, NULL, &arg);
    }

    /* done */
    return(RMS_EOK);
}
