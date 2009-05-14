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
 *  ======== pingCreate.c ========
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
#include <stdlib.h>
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
 *  ======== PING_TI_create ========
 *
 *  Requires:  argData not NULL
 *             argLength > 0
 *  Ensures:   Failed:  returns RMS_EFAIL: Invalid delay (< 1); or
 *                      returns RMS_EOUTOFMEMORY; or
 *             Success: returns RMS_EOK; and
 *                      node stores PING_TI_OBJ; and
 *                      PING_TI_Obj stores valid delay.
 */
RMS_STATUS PING_TI_create(Int argLength, Char * argData,
              Int numInStreams, RMS_WORD inDef[],
              Int numOutStreams, RMS_WORD outDef[], NODE_EnvPtr node)
{
    PING_TI_Obj     *pingPtr;
    RMS_STATUS      retval;

	/* Using bitwise OR to prevent compiler warning */
	argLength |= argLength;
	*argData |= *argData;
	numInStreams |= numInStreams;
	*inDef |= *inDef;
	numOutStreams |= numOutStreams;
	*outDef |= *outDef;


    /* Allocate task node context object. */
    if ((pingPtr = MEM_calloc(PINGMEMSEG, sizeof(PING_TI_Obj), 0))
         != MEM_ILLEGAL) {

         /* Initialize Ping node context structure. */

         /*
          *  Set the instance number of this node.
          *  Note: since the create and delete functions run in the context
          *  of the RMS server thread, which is not reentrant, it is safe to
          *  directly alter global node class data in these functions.
          */
         pingPtr->instance = ++PING_TI_instances;

         /* Save Ping node context for use in execute and delete phases. */
         node->moreEnv = pingPtr;
         retval = RMS_EOK;
    }
    else {
         /* If unable to allocate object, return out of memory error. */
         retval = RMS_EOUTOFMEMORY;
    }

    PING_print("PING_TI_create: retval = 0x%x\n", retval);

    /* Assert Ensures */
    DBC_ensure((retval == RMS_EFAIL) || (retval == RMS_EOUTOFMEMORY) ||
     ((retval == RMS_EOK) && (node->moreEnv != NULL)) );

    return (retval);
}
