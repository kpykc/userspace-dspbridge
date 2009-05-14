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
 *  ======== zcmsgCreate.c =========
 *  Sample DSP program to zcmsg buffers supplied from a host input channel back
 *  into a host output.  Demonstrates end-of-stream processing and flushing of
 *  the output channel on shutdown.
 */
/*  Usage:  zcmsg
 *
 *! Revision History
 *! ================
 *! 12-Dec-2005 connell  Created from zcmsg.c
 */

#include <std.h>
#include <mem.h>
#include <strm.h>
#include <tsk.h>
#include <rms.h>
#include "zcmsg.h"

/*
 *  ======== ZCMSG_TI_create ========
 */
RMS_STATUS ZCMSG_TI_create(Int argLength,
                           Char * argData,
                           Int numInStreams,
                           RMS_WORD inDef[],
                           Int numOutStreams,
                           RMS_WORD outDef[],
                           NODE_EnvPtr node)
{

    Int             bufSize;
    ZCMSG_TI_Obj    *zcMsgPtr;
    RMS_STATUS      cResult = RMS_EOK;

    /* Obtain input argument */
    bufSize = atoi(argData);

    /* Using bitwise OR to prevent compiler warning */
    argLength |= argLength;
    numInStreams |= numInStreams;
    *inDef |= *inDef;
    numOutStreams |= numOutStreams;
    *outDef |= *outDef;


    /* Allocate task node context object */
    if ((zcMsgPtr = MEM_calloc(0, sizeof(ZCMSG_TI_Obj), 0)) != NULL)
    {
        zcMsgPtr->msgBuf = NODE_allocMsgBuf(node, bufSize, 0);
        zcMsgPtr->bufSize = bufSize;

        if(zcMsgPtr->msgBuf != NULL)
        {
            node->moreEnv = (Ptr)zcMsgPtr;
        } else
        {
            cResult = RMS_EOUTOFMEMORY;
        }
    } else
    {
        /* If unable to allocate object, return RMS_E_OUTOFMEMORY. */
        cResult = RMS_EOUTOFMEMORY;
    }

    return(cResult);
}

