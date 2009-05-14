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
 *  ======== dmmcopyExecute.c =========
 *  Sample DSP program to copy one dynamically mapped MPU buffer to another.
 *  This program accompanies the wdmmcopy.c DSP/BIOS Bridge sample program.
 *
 */

#include <std.h>
#include <tsk.h>
#include <rms.h>
#include <node.h>
/* BCACHE only exists for Joule/GEM */
#ifdef _64P_
#include <bcache.h>
#endif

#define TRACE &trace

#define DMMMEMSEG           0
#define DMM_SETUPBUFFERS    0xABCD
#define DMM_WRITEREADY      0xADDD

/*
 *  ======== DMMCOPY_TI_execute ========
 */
RMS_STATUS DMMCOPY_TI_execute(NODE_EnvPtr env)
{
    RMS_DSPMSG      msg;
    LgUns           size;
    Uns             *fromGPP;
    Uns             *toGPP;
    LgUns           tempAddr1;
    LgUns           tempAddr2;
#ifdef _64P_
    LgUns           bufSize;
#endif

    /* Initialize msg structure. */
    msg.cmd = ~(RMS_EXIT);

    for (;;) {

        /* Get message. */
        NODE_getMsg(env, &msg, NODE_FOREVER);

        if (msg.cmd == RMS_EXIT) {

            /* Exit if GPP says we should */
            break;

        } else if (msg.cmd == DMM_SETUPBUFFERS) {

            /* Setup buffer addresses */
            fromGPP = (Uns *)(msg.arg1);
            toGPP = (Uns *)(msg.arg2);

        } else if (msg.cmd == DMM_WRITEREADY) {

            /* How many MAUs to copy? */
#ifdef _64P_
            size  = (LgUns)(msg.arg1);
            bufSize = size;
#else
            size  = (LgUns)(msg.arg1) / sizeof(Uns);
#endif

            tempAddr1 = (LgUns) toGPP;
            tempAddr2 = (LgUns) fromGPP;

            /*
             * Note: We can't use memcpy since pointers for buffers of
             * size > 64K will wrap around.  Instead, we do our own
             * copying using a while loop with LgUns buffer pointers.
             */
#ifdef _64P_
            BCACHE_invalidate((Ptr)fromGPP,bufSize);
#endif
            while (size > 0 )
            {
#ifdef _64P_
                *(volatile unsigned char *)tempAddr1 = *(volatile unsigned char *)tempAddr2;
                tempAddr1++;
                tempAddr2++;
#else
                *(volatile Uns *)tempAddr1 = *(volatile Uns *)tempAddr2;

                tempAddr1 += (sizeof (Uns));
                tempAddr2 += (sizeof (Uns));
#endif
                size--;
            }

#ifdef _64P_

            BCACHE_writebackInvalidate((Ptr)toGPP, bufSize);

#endif

            /* Tell GPP that we're done copying */
            NODE_putMsg(env, NODE_TOGPP, &msg, 0);
        }
    }

    return (RMS_EOK);
}
