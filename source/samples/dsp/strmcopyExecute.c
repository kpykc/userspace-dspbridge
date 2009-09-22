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
#include <node.h>

#ifdef _INST2_
 #include <inst2.h>
#endif 

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


/*
 *  ======== STRMCOPY_TI_execute ========
 */
RMS_STATUS STRMCOPY_TI_execute(NODE_EnvPtr env)
{
    StrmcopyObj     *copyObj;
    Uns             bytesRead;
    Arg             bufArg;
    Uns             *inBuf;
    Uns             *outBuf;
    RMS_DSPMSG      msg;
    Uns             msgReady;
    Uns             readyStreams;
    Bool            firstFrame = TRUE;

#if DLLDEBUG
    UTL_breakPoint();
#endif

    /* Dereference context structure stored in the env variable. */
    copyObj = (StrmcopyObj*)env->moreEnv;
    inBuf = copyObj->inBuf;
    outBuf = copyObj->outBuf;

    /* Initialize msg structure. */
    msg.cmd = ~(RMS_EXIT);

    /* Prime the input stream with an empty buffer. */
    STRM_issue(copyObj->inStream, inBuf, copyObj->inSize, copyObj->inSize, 0);

    /* Until end of stream indicated, receive and send a host buffer: */
    for (;;) {

	/*
        * The following calls to INST2_COLLECT are only for testing porpose,
        * this calls will simulate the behavior of the INST2 meassurements of 
        * the USN layer
       */  
       #ifdef _INST2_
       /* collect time before the blocking call */
       INST2_COLLECT(INST2_FORMAT_BENCH | INST2_TAG_WAIT | INST2_SUFFIX_START, 0, 0, 0)
       #endif
	   
        /* Wait until input stream is ready, or until GPP message arrives. */
        readyStreams = NODE_wait(env, &copyObj->inStream, 1, NODE_FOREVER,
            &msgReady);
	   
        #ifdef _INST2_ 
        /* collect time after the blocking call */
        INST2_COLLECT(INST2_FORMAT_BENCH | INST2_TAG_WAIT | INST2_SUFFIX_STOP, 0, 0, 0)
        #endif
		
        /* If a message has arrived. */
        if (msgReady != 0) {

            /* Get message. */
            NODE_getMsg(env, &msg, 0);

            if (msg.cmd == RMS_EXIT) {
                break;
            }
        }

        /* If a buffer is ready on the input stream. */
        if (readyStreams != 0) {

            /* If first iteration, reclaim on output buffer not needed. */
            if (firstFrame == TRUE) {
                firstFrame = FALSE;
            }
            else {
                /* Reclaim output buffer. */
                STRM_reclaim(copyObj->outStream, (Ptr *)&outBuf, NULL, &bufArg);
            }

            /* Get the buffer from the input stream. */
            bytesRead = STRM_reclaim(copyObj->inStream, (Ptr *)&inBuf,
                NULL, &bufArg);
            /* Copy the buffer into the output buffer. */
            memcpy(outBuf, inBuf, bytesRead);

            /* Send an output buffer to the output stream. */
            STRM_issue(copyObj->outStream, outBuf, bytesRead,
                copyObj->outSize, 0);

            /* Send an empty buffer to the input stream. */
            STRM_issue(copyObj->inStream, inBuf, copyObj->inSize,
                copyObj->inSize, 0);
        }
    }

    /* Idle open streams before the delete phase. */
    STRM_idle(copyObj->inStream, TRUE);
    STRM_idle(copyObj->outStream, TRUE);

    /* Reclaim any outstanding buffers in the data streams. */
    STRM_reclaim(copyObj->inStream, (Ptr *)&inBuf, NULL, &bufArg);
    if (!firstFrame) {
        STRM_reclaim(copyObj->outStream, (Ptr *)&outBuf, NULL, &bufArg);
    }

    SYS_printf("ExEcUtEd!\n");

#if DLLDEBUG
    UTL_breakPoint();
#endif
    return (RMS_EOK);
}
