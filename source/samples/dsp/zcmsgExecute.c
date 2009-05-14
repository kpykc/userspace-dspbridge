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
 *  ======== zcmsgExecute.c =========
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
 *  ======== ZCMSG_TI_execute ========
 */
RMS_STATUS ZCMSG_TI_execute(NODE_EnvPtr env)
{
    RMS_DSPMSG       msg;
    ZCMSG_TI_Obj            *zcMsgPtr;

    /* Dereference context structure stored in the env variable. */
    zcMsgPtr = (ZCMSG_TI_Obj *)env->moreEnv;

    /* Initialize msg structure. */
    msg.cmd = ~(RMS_EXIT);


    while (1)
    {
        /* get the message */
        NODE_getMsg(env, &msg, NODE_FOREVER);

        /* if got RMS_EXIT then exit the execute-phase */
        if (msg.cmd == RMS_EXIT)
        {
            break;
        }

        else if (msg.cmd == RMS_BUFDESC)
        {
            Int size = msg.arg2;
            Int i;

            for (i = 0; i < size; i++)
            {
                *((Int *)zcMsgPtr->msgBuf + i) = *((Int *)msg.arg1 + i) * 2;
            }

            msg.arg1 = (RMS_WORD) zcMsgPtr->msgBuf;
            NODE_putMsg(env, NODE_TOGPP, &msg, NODE_FOREVER);
        }

    } /* while(1) */

    return(RMS_EOK);
}

