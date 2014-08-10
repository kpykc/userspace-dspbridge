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
 *  ======== dmmcopyCreate.c =========
 *  Sample DSP program to copy one dynamically mapped MPU buffer to another.
 *  This program accompanies the wdmmcopy.c DSP/BIOS Bridge sample program.
 *
 */

#include <std.h>
#include <tsk.h>
#include <rms.h>
#include <node.h>

#define TRACE &trace

#define DMMMEMSEG           0
#define DMM_SETUPBUFFERS    0xABCD
#define DMM_WRITEREADY      0xADDD

/*
 *  ======== DMMCOPY_TI_delete ========
 */
RMS_STATUS VISION_TI_delete(NODE_EnvPtr env)
{
    /* adding NULL to prevent compiler warning */
    env = env + NULL;

    /* No streams to tear down! */

    return (RMS_EOK);
}
