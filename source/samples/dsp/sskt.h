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
 *  ======== sskt.h ========
 *
 */
#ifndef SSKT_
#define SSKT_

#include <ialg.h>
#include <node.h>
#include <std.h>
#include <rmstypes.h>

extern RMS_STATUS SCALESOCKET_TI_create(Int argLength, Char * argData, 
    IALG_Fxns *algFxns, Int numInStreams, RMS_WORD inDef[], Int numOutStreams, 
    RMS_WORD outDef[], NODE_EnvPtr node);

extern RMS_STATUS SCALESOCKET_TI_execute(NODE_EnvPtr node);

extern RMS_STATUS SCALESOCKET_TI_delete(NODE_EnvPtr node);

#define CMD_SETSCALE (RMS_USER + 1)
#define CMD_ACKSCALE (RMS_USER + 2)

/*
 *  ======== SSKT_Obj ========
 */
typedef struct SSKT_Obj {
    IALG_Handle algHandle;
    STRM_Handle inStream;
    STRM_Handle outStream;
    Uns * outBuf;
    Uns * inBuf;
    Uns outSize;
    Uns inSize;
    Int segId;  /* Scale node's external heap Id */
} SSKT_Obj;

#endif  /* SSKT_ */

