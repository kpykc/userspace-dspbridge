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
 *  ======== strmcopy.c =========
 *  Sample DSP program to copy buffers supplied from a host input channel back
 *  into a host output.  Demonstrates end-of-stream processing and flushing of
 *  the output channel on shutdown.
 *
 *  This program accompanies the wstrmcopy.c DSP/BIOS Bridge sample program.
 *
 *  Usage:  strmcopy
 *
 */

#include <std.h>
#include <stdlib.h>
#include <mem.h>
#include <strm.h>
#include <tsk.h>
#include <rms.h>
#include <log.h>
#include <node.h>

#define DLLDEBUG 0
#define UTL_breakPoint() asm(" .if (.MNEMONIC)\n ESTOP_1\n .else\n ESTOP_1()\n .endif\n NOP")

/* Strmcopy task object properties. */
typedef struct {
    STRM_Handle inStream;       /* A STRM input channel. */
    STRM_Handle outStream;      /* A STRM output channel. */
    Uns         *inBuf;         /* A data buffer. */
    Uns         *outBuf;        /* A data buffer. */
    Uns         inSize;         /* Size of input buffer. */
    Uns         outSize;        /* Size of output buffer. */
} StrmcopyObj;

#define TRACE &trace
extern LOG_Obj trace;


/*
 *  ======== STRMCOPY_TI_delete ========
 */
RMS_STATUS STRMCOPY_TI_delete(NODE_EnvPtr env)
{
    StrmcopyObj     *copyObj;

#if DLLDEBUG
    UTL_breakPoint();
#endif

    /* Dereference context structure stored in the env variable. */
    copyObj = (StrmcopyObj*)env->moreEnv;

    if (copyObj) {
        if (copyObj->inStream != NULL) {

            /* Free stream buffer. */
            if (copyObj->inBuf != NULL) {
                STRM_freeBuffer(copyObj->inStream, copyObj->inBuf,
                    copyObj->inSize);
            }

            /* Free stream object. */
            STRM_delete(copyObj->inStream);
        }

        if (copyObj->outStream != NULL) {

            /* Free stream buffer. */
            if (copyObj->outBuf != NULL) {
                STRM_freeBuffer(copyObj->outStream, copyObj->outBuf,
                    copyObj->outSize);
            }

            /* Free stream object. */
            STRM_delete(copyObj->outStream);
        }

        MEM_free(0, copyObj, sizeof(StrmcopyObj));
    }

    SYS_printf("DeLeTeD!\n"); 

    return (RMS_EOK);
}
