/* 
 *  Copyright 2007
 *  Texas Instruments Incorporated
 *
 *  All rights reserved.  Property of Texas Instruments Incorporated
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 * 
 */
/*
 *  ======== dma4/acpy2_complete.c ========
 *  Implementaion of ACPY2_complete.
 */

#include <std.h>
#include <ti/xdais/acpy2.h>
#ifndef _RTS_MODE
#include <dbc.h>
#else
#include <dbc_rts.h>
#endif
#include "_dma.h"

#pragma CODE_SECTION(ACPY2_complete, ".text:ACPY2_complete")

/*
 *  ======== ACPY2_complete ========
 *  Check to see if all dma transfers on logical channel have completed.
 */
Int ACPY2_complete(IDMA2_Handle handle)
{
    DBC_require(handle != NULL);
    return (!((_DMA_Handle)handle)->numJobs);    
}
/*
 *  @(#) ti.sdo.fc.acpy2; 1,0,0,20; 5-10-2007 10:49:05; /db/atree/library/trees/fc-b12x/src/
 */

