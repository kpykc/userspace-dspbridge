/*
 *  Copyright 2007 by Texas Instruments Incorporated.
 *
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */

/*
 *  ======== fcpy.c ========
 *  FCPY Module - implementation of FCPY module APIs.
 */

#pragma CODE_SECTION (FCPY_init, ".text:init")
#pragma CODE_SECTION (FCPY_exit, ".text:exit")
#pragma CODE_SECTION (FCPY_apply, ".text:apply")
#pragma CODE_SECTION (FCPY_control, ".text:control")

#include <std.h>

#include <ti/sdo/fc/utils/api/alg.h>
#include <fcpy.h>
#include <fcpy_ti.h>

/*
 *  ======== FCPY_apply ========
 *  Apply a FCPY function to the input array and place results in the
 *  output array.
 */
Void FCPY_apply(FCPY_Handle handle, Void * in, Void * out)
{
    /* activate instance object */
    ALG_activate((ALG_Handle)handle);
    
    handle->fxns->doCopy(handle, in, out);      /* copy data */

    /* deactivate instance object */
    ALG_deactivate((ALG_Handle)handle);
}

/*
 *  ======== FCPY_control ========
 *  Used to either write to the read/write parameters in the status
 *  structure or to read all the parameters in the status structure.
 */
XDAS_Bool FCPY_control(FCPY_Handle handle, FCPY_Cmd cmd, FCPY_Status *status)
{
    /* Check for valid handle and fxn ptr and do control operation */
    if (handle->fxns->control) {
        return (handle->fxns->control(handle, cmd, status));
    }

    return (XDAS_FALSE);
}

/*
 *  ======== FCPY_exit ========
 *  Module finalization
 */
Void FCPY_exit(Void)
{
    FCPY_TI_exit();
}

/*
 *  ======== FCPY_init ========
 *  Module initialization
 */
Void FCPY_init(Void)
{
    FCPY_TI_init();
}

/*
 *  @(#) ti.sdo.fc.dman3.examples; 1,0,0,36; 5-10-2007 10:49:53; /db/atree/library/trees/fc-b12x/src/
 */

