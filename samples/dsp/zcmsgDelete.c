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
 *  ======== zcmsgDelete.c =========
 *  Sample DSP program to zcmsg buffers supplied from a host input channel back
 *  into a host output.  Demonstrates end-of-stream processing and flushing of
 *  the output channel on shutdown.
 */
/*! Revision History
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
 *  ======== ZCMSG_TI_delete ========
 */
RMS_STATUS ZCMSG_TI_delete(NODE_EnvPtr env)
{
    ZCMSG_TI_Obj      *zcMsgPtr;

    /* Dereference context structure stored in the env variable. */
    if(env->moreEnv)
    {
        zcMsgPtr = (ZCMSG_TI_Obj*)env->moreEnv;

        if (zcMsgPtr)
        {
            /* Free buffer. */
            if (zcMsgPtr->msgBuf != NULL)
            {
                NODE_freeMsgBuf(env, zcMsgPtr->msgBuf, zcMsgPtr->bufSize);
               }
            MEM_free(0, zcMsgPtr, sizeof(ZCMSG_TI_Obj));
        }
    }

    return(RMS_EOK);
}

