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
 *  ======== loadCreate.c ========
 */

#include <std.h>
#include <sys.h>
#include <tsk.h>
#include <rmsdefs.h>  /* DSP/BIOS Bridge Resource Mangager Server definitions */
#include <rms_sh.h>   /* RMS definitions shared on both GPP and DSP */
#include <node.h>     /* DSP/BIOS Bridge Node APIs */

#define TRACE &trace

/*
 *  ======== delay ========
 */
Void delay( long unsigned int t )
{
    volatile long unsigned int i;
    for( i=0; i<t; i++ ) {
       asm ( " NOP ");
    }
}

/*
 *  ======== LOAD_TI_create ========
 */
RMS_STATUS LOAD_TI_create(Int argLength, Char * argData,
              Int numInStreams, RMS_WORD inDef[],
              Int numOutStreams, RMS_WORD outDef[], NODE_EnvPtr env)
{

 /* Using bitwise OR, adding NULL to prevent compiler warning */
 argLength |= argLength;
 *argData |= *argData;
 numInStreams |= numInStreams;
 *inDef |= *inDef;
 numOutStreams |= numOutStreams;
 *outDef |= *outDef;
 env = env + NULL;

 return (RMS_EOK);
}

/*
 *  ======== LOAD_TI_execute ========
 */
RMS_STATUS LOAD_TI_execute(NODE_EnvPtr env)
{
    RMS_DSPMSG   msg;

    msg.cmd = ~(RMS_EXIT);

    while (NODE_getMsg(env, &msg, 50)==FALSE) {

        delay(3000);
    };

    return (RMS_EOK);
}

/*
 *  ======== LOAD_TI_delete ========
 */
RMS_STATUS LOAD_TI_delete(NODE_EnvPtr env)
{

/* adding NULL to prevent compiler warning */
env = env + NULL;

return (RMS_EOK);
}

