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
 *  ======== scalel_ti.c ========
 *  Implementation of the SCALE_TI.h interface; TI's implementation
 *  of the ISCALE interface.
 *
 */
#pragma CODE_SECTION(SCALE_TI_alloc, ".text:algAlloc")
#pragma CODE_SECTION(SCALE_TI_free, ".text:algFree")
#pragma CODE_SECTION(SCALE_TI_initObj, ".text:algInit")
#pragma CODE_SECTION(SCALE_TI_init, ".text:init")
#pragma CODE_SECTION(SCALE_TI_exit, ".text:exit")

#include <std.h>
#include "iscale.h"

#include "scale_ti.h"

/*
 *  ======== SCALE_TI_Obj ========
 */
typedef struct SCALE_TI_Obj {
    IALG_Obj	alg;		/* MUST be first field of all SCALE objs */
    Uns 	scaleFactor;
} SCALE_TI_Obj;


/*
 *  ======== SCALE_TI_activate ========
 *  Activate our object; e.g., initialize any scratch memory required
 *  by the SCALE_TI processing methods.
 */
Void SCALE_TI_activate(IALG_Handle handle)
{
	/* adding NULL to prevent compiler warning */
	handle = handle + NULL;
}


/*
 *  ======== SCALE_TI_alloc ========
 *  Return a table of memory descriptors that describe the memory needed
 *  to construct a SCALE_TI_Obj structure.
 */
Int SCALE_TI_alloc(const IALG_Params *scaleParams, IALG_Fxns **fxns,
    IALG_MemRec memTab[])
{
    const ISCALE_Params *params = (Void *)scaleParams;

    /* adding NULL to prevent compiler warning */
    fxns = fxns + NULL;

    if (params == NULL) {
	params = &ISCALE_PARAMS;	/* set default parameters */
    }

    /* Request memory for SCALE object */
    memTab[0].size = sizeof(SCALE_TI_Obj);
    memTab[0].alignment = 0;
    memTab[0].space = IALG_DARAM0;
    memTab[0].attrs = IALG_PERSIST;

    return (1);
}


/*
 *  ======== SCALE_TI_control ========
 *  Execute a control operation.
 */
Void SCALE_TI_control(IALG_Handle handle, IALG_Cmd cmd, IALG_Status * status)
{
    SCALE_TI_Obj *scale = (Void *)handle;

    if (cmd == ISCALE_SETSCALE) {
        scale->scaleFactor = (Uns) ((ISCALE_Status *)status)->scaleFactor;
    }
    else if (cmd == ISCALE_GETSCALE) {
        ((ISCALE_Status *)status)->scaleFactor = scale->scaleFactor;
    }
}


/*
 *  ======== SCALE_TI_deactivate ========
 *  Deactivate our object; e.g., save any scratch memory requred
 *  by the SCALE_TI processing methods to persistent memory.
 */
Void SCALE_TI_deactivate(IALG_Handle handle)
{
	/* adding NULL to prevent compiler warning */
	handle = handle + NULL;
}


/*
 *  ======== SCALE_TI_exit ========
 *  Exit the SCALE_TI module as a whole.
 */
Void SCALE_TI_exit(Void)
{
}


/*
 *  ======== SCALE_TI_free ========
 *  Return a table of memory pointers that should be freed.  Note
 *  that this should include *all* memory requested in the
 *  SCALE_TI_alloc operation above.
 */
Int SCALE_TI_free(IALG_Handle handle, IALG_MemRec memTab[])
{
    SCALE_TI_Obj *scale = (Void *)handle;
    Int n;

    n = SCALE_TI_alloc(NULL, NULL, memTab);

    memTab[0].base = scale;

    return (n);
}


/*
 *  ======== SCALE_TI_init ========
 *  Initialize the SCALE_TI module as a whole.
 */
Void SCALE_TI_init(Void)
{
}


/*  ======== SCALE_TI_initObj ========
 *  Initialize the memory allocated for our instance.
 */
Int SCALE_TI_initObj(IALG_Handle handle, const IALG_MemRec memTab[],
    IALG_Handle p, const IALG_Params *scaleParams)
{
    SCALE_TI_Obj *scale = (Void *)handle;
    const ISCALE_Params *params = (Void *)scaleParams;

    /* adding NULL to prevent compiler warning */
    memTab = memTab + NULL;
    p = p + NULL;

    if (params == NULL) {
        params = &ISCALE_PARAMS;	/* if NULL params, use defaults */
    }

    scale->scaleFactor = params->scaleFactor;

    return (IALG_EOK);
}


/*  ======== SCALE_TI_moved ========
 *  Fixeup any pointers to data that has been moved by the client.
 */
Void SCALE_TI_moved(IALG_Handle handle, const IALG_MemRec memTab[],
    IALG_Handle p, const IALG_Params *scaleParams)
{
    /* NOT USED */

    /* adding NULL to prevent compiler warning */
    handle = handle + NULL;
    memTab = memTab + NULL;
    p = p + NULL;
    scaleParams = scaleParams + NULL;
}

/*
 *  ======== SCALE_TI_scale ========
 *  TI's implementation of the scale operation.
 */
Void  SCALE_TI_scale(ISCALE_Handle handle, Uns in[], Uns out[], Uns size)
{
    SCALE_TI_Obj *scale = (Void *)handle;
    Int i;

    for (i = 0; i < size / sizeof(Uns); i++) {
        out[i] = in[i] * scale->scaleFactor;
    }
}


