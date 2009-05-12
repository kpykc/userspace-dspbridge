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
 *  ======== pingDelete.c ========
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
 *  ======== PING_TI_delete ========
 *  Ensures:   Failed:  returns RMS_EFREE: couldn't free context object; or
 *             Success: returns RMS_EOK and PING_TI_Obj freed.
 */
RMS_STATUS PING_TI_delete(NODE_EnvPtr node)
{
    PING_TI_Obj  *pingPtr;
    RMS_STATUS    retval = RMS_EOK;

    /* Dereference context structure stored in the node variable. */
    pingPtr = node->moreEnv;

    if (pingPtr != NULL) {

       /* We now have one less PING node instance to kick around. */
       PING_TI_instances--;

       /* Free Ping context structure. */
       if (!MEM_free(PINGMEMSEG, pingPtr, sizeof(PING_TI_Obj)))  {
            retval = RMS_EFREE;
       }
    }
    PING_print("PING_TI_delete: retval = %x", retval);

    /* Assert Ensures */
    DBC_ensure((retval == RMS_EFREE) || (retval == RMS_EOK));

    return (retval);
}

