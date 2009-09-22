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
 *  ======== STRMCOPY_TI_create ========
 */
RMS_STATUS STRMCOPY_TI_create(Int argLength,
                              Char * argData,
                              Int numInStreams,
                              RMS_WORD inDef[],
                              Int numOutStreams,
                              RMS_WORD outDef[],
                              NODE_EnvPtr env)
{
    RMS_StrmDef     *strmDef;
    STRM_Attrs      attrs = STRM_ATTRS;
    StrmcopyObj     *copyObj;
    RMS_STATUS      status = RMS_EOK;

    /* Using bitwise OR to prevent compiler warning */
    argLength |= argLength;
    *argData |= *argData;
    numInStreams |= numInStreams;
    numOutStreams |= numOutStreams;


#if DLLDEBUG
    UTL_breakPoint();
#endif

     #ifdef _INST2_
     //SYSTEM instrumentation testing
     INST2_INIT(0,INST2_FORMAT_ALL | INST2_TAG_ALL);
     #endif

    /* Allocate context structure for this instance of the strmcopy object. */
    if ((copyObj = MEM_calloc(0, sizeof(StrmcopyObj), 0)) != NULL) {

        /*
         *  Set env->moreEnv to copyObj to be used in execute and delete
         *  phase.
         */
        env->moreEnv = (Ptr)copyObj;

        /* Create input stream. */
        strmDef = (RMS_StrmDef *)inDef[0];

        attrs.nbufs = strmDef->nbufs;
        attrs.segid = strmDef->segid;
        attrs.timeout = strmDef->timeout;
        attrs.align = strmDef->align;


        copyObj->inStream = STRM_create(strmDef->name, STRM_INPUT,
            strmDef->bufsize, &attrs);

        copyObj->inSize = strmDef->bufsize;

        /* Create output stream. */
        strmDef = (RMS_StrmDef *)outDef[0];

        attrs.nbufs = strmDef->nbufs;
        attrs.segid = strmDef->segid;
        attrs.timeout = strmDef->timeout;
        attrs.align = strmDef->align;


        copyObj->outStream = STRM_create(strmDef->name, STRM_OUTPUT,
            strmDef->bufsize, &attrs);

        copyObj->outSize = strmDef->bufsize;

        /* Allocate data buffers for data streaming. */
        copyObj->inBuf = STRM_allocateBuffer(copyObj->inStream,
            copyObj->inSize);

        copyObj->outBuf = STRM_allocateBuffer(copyObj->outStream,
            copyObj->outSize);

        /* Verify allocations; if failure, free all allocated objects. */
        if ((copyObj->inStream == NULL) || (copyObj->outStream == NULL)) {
            status = RMS_ESTREAM;
        }

        if ((copyObj->inBuf == MEM_ILLEGAL) ||
            (copyObj->outBuf == MEM_ILLEGAL)) {
                status = RMS_EOUTOFMEMORY;
        }
    }
    else {
        /* If unable to allocate object, return RMS_E_OUTOFMEMORY. */
        status = RMS_EOUTOFMEMORY;
    }

    SYS_printf("CrEateD!\n");

#if DLLDEBUG
    UTL_breakPoint();
#endif

    return (status);
}
