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
 *  ======== dsptoolCreate.c ========
 *
 *  DSP/BIOS Bridge Task Node that gets DSP debug info.
 *
 */

#include <std.h>
#include <stdlib.h>
#include <sys.h>
#include <dbc.h>      /* Design by Contract assertions                        */
#include <rmsdefs.h>  /* DSP/BIOS Bridge Resource Mangager Server definitions */
#include <rms_sh.h>   /* RMS definitions shared on both GPP and DSP           */
#include <node.h>     /* DSP/BIOS Bridge Node APIs                            */

#include "dsptooldata.h"

#define TRACE &trace

#define  DSPTOOLMEMSEG   0    /* Segment used as default DSP/BIOS bridge heap    */

/*
 *  ======== DSPTOOL_TI_Obj ========
 *  DSPTOOL task node context.
 */
typedef struct DSPTOOL_TI_Obj {
   Uns instance;      /* Instance number of this DSPTOOL node.                   */
} DSPTOOL_TI_Obj;


/*
 *  ======== DSPTOOL_TI_create ========
 *
 *  Requires:  argData not NULL
 *             argLength > 0
 *  Ensures:   Failed:  returns RMS_EFAIL: Invalid delay (< 1); or
 *                      returns RMS_EOUTOFMEMORY; or
 *             Success: returns RMS_EOK; and
 *                      node stores DSPTOOL_TI_OBJ; and
 *                      DSPTOOL_TI_Obj stores valid delay.
 */
RMS_STATUS DSPTOOL_TI_create(Int argLength, Char * argData,
              Int numInStreams, RMS_WORD inDef[],
              Int numOutStreams, RMS_WORD outDef[], NODE_EnvPtr node)
{
    DSPTOOL_TI_Obj     *dsptoolPtr;
    RMS_STATUS      retval;

	/* Using bitwise OR to prevent compiler warning */
	argLength |= argLength;
	*argData |= *argData;
	numInStreams |= numInStreams;
	*inDef |= *inDef;
	numOutStreams |= numOutStreams;
	*outDef |= *outDef;


    /* Allocate task node context object. */
    if ((dsptoolPtr = MEM_calloc(DSPTOOLMEMSEG, sizeof(DSPTOOL_TI_Obj), 0))
         != MEM_ILLEGAL) {

         /* Initialize dsptool node context structure. */

         /*
          *  Set the instance number of this node.
          *  Note: since the create and delete functions run in the context
          *  of the RMS server thread, which is not reentrant, it is safe to
          *  directly alter global node class data in these functions.
          */
         dsptoolPtr->instance = ++DSPTOOL_TI_instances;

         /* Save dsptol node context for use in execute and delete phases. */
         node->moreEnv = dsptoolPtr;
         retval = RMS_EOK;
    }
    else {
         /* If unable to allocate object, return out of memory error. */
         retval = RMS_EOUTOFMEMORY;
    }
    /* Assert Ensures */
    DBC_ensure((retval == RMS_EFAIL) || (retval == RMS_EOUTOFMEMORY) ||
     ((retval == RMS_EOK) && (node->moreEnv != NULL)) );

    return (retval);
}
