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
 *  ======== dsptoolDelete.c ========
 *
 *  DSP/BIOS Bridge Task Node that gets DSP debug info.
 *
 */

#include <std.h>

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
 *  ======== DSPTOOL_TI_delete ========
 *  Ensures:   Failed:  returns RMS_EFREE: couldn't free context object; or
 *             Success: returns RMS_EOK and DSPTOOL_TI_Obj freed.
 */
RMS_STATUS DSPTOOL_TI_delete(NODE_EnvPtr node)
{
    DSPTOOL_TI_Obj  *dsptoolPtr;
    RMS_STATUS    retval = RMS_EOK;

    /* Dereference context structure stored in the node variable. */
    dsptoolPtr = node->moreEnv;

    if (dsptoolPtr != NULL) {

       /* We now have one less DSPTOOL node instance to kick around. */
       DSPTOOL_TI_instances--;

       /* Free DSPTOOL context structure. */
       if (!MEM_free(DSPTOOLMEMSEG, dsptoolPtr, sizeof(DSPTOOL_TI_Obj)))  {
            retval = RMS_EFREE;
       }
    }
    /* Assert Ensures */
    DBC_ensure((retval == RMS_EFREE) || (retval == RMS_EOK));

    return (retval);
}

