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
 *  ======== pingExecute.c ========
 *
 *  Example DSP/BIOS Bridge Task Node.
 *
 *  Demonstrates:
 *   1. Periodic messaging 'heartbeat' from DSP to GPP.
 *   2. How to create a task node with initial arguments.
 *   3. Use of node class data.
 *   4. Use of Design by Contract method to assert node function interface
 *      requirements.
 *
 *  Requires:
 *   1. Node PING_TI is configured in the base DSP image (via BIOS Config Tool)
 *   2. The corresponding GPP application must be invoked to instantiate the
 *      node and to display received messages on console.
 *
 */

#include <std.h>

#include <sys.h>
#include <dbc.h>      /* Design by Contract assertions                        */
#include <rmsdefs.h>  /* DSP/BIOS Bridge Resource Mangager Server definitions */
#include <rms_sh.h>   /* RMS definitions shared on both GPP and DSP           */
#include <node.h>     /* DSP/BIOS Bridge Node APIs                            */

#include "pinglib.h"
#include "pingdata.h"

#define TRACE &trace

#define  PINGMEMSEG   0    /* Segment used as default DSP/BIOS bridge heap    */

/*
 *  ======== PING_TI_Obj ========
 *  PING task node context.
 */
typedef struct PING_TI_Obj {
   Uns instance;      /* Instance number of this PING node.                   */
} PING_TI_Obj;


/*
 *  ======== PING_TI_execute ========
 *  Requires:  node's moreEnv pointer is set.
 */
RMS_STATUS PING_TI_execute(NODE_EnvPtr node)
{
    RMS_DSPMSG   msg;
    RMS_WORD	cnt = 0;
    Uns          instance;
    MEM_Stat     memUsage;
    Bool         status;

    /* Assert Requires */
    DBC_require(node->moreEnv != NULL);

    PING_print("PING_TI_execute: Entered ...\n");

    /* Extract attributes from node context object: */
    instance = ((PING_TI_Obj *)node->moreEnv)->instance;

    msg.cmd = ~(RMS_EXIT);  /* Initialize */
    do {
       /* Wait (with delay timeout) for RMS exit messsage from GPP: */
       NODE_getMsg(node, &msg, NODE_FOREVER);
       PING_print("PING_TI_execute: Received ping from GPP: cmd = 0x%x; cnt = 0x%x\n",
		       msg.cmd, msg.arg1);

       /*
        *  Unless a DSPNode_Terminate() call was made from GPP,
        *  NODE_getMsg will have timed out, with msg.cmd left as RMS_USER+1.
        */
       if (msg.cmd != RMS_EXIT) {
          MEM_stat(0, &memUsage);

          /* Setup message fields:  */
          msg.cmd  = RMS_USER + instance;   /* Node instance number */
          msg.arg1 = cnt++;                 /* Message count        */
          msg.arg2 = (RMS_WORD)(memUsage.size - memUsage.used); /* MAU's free */

          /* Send message, *without* blocking until message queue is ready.   */
          status = NODE_putMsg(node, NODE_TOGPP, &msg, 0);
          if (!status)  {
            /*
             *  Message queue not emptied, so GPP application hasn't pulled the
             *  last message.  In this case, we drop it, and proceed.
             *  If we do *not* wish to drop messages, set timeout parameter to
             *  NODE_FOREVER instead of zero.  In this case, the GPP will flow
             *  control the message rate.
             */
            PING_print("PING_TI_execute: dropped acknowledgement msg GPP: cmd = 0x%x; cnt = 0x%x\n",
                    msg.cmd, msg.arg1);
          }
       }

    }while(msg.cmd != RMS_EXIT);

    PING_print("PING_TI_execute: Exited\n");

    return (RMS_EOK);
}
