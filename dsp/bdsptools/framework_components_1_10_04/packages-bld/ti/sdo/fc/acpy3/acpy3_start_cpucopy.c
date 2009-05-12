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
 *  ======== acpy3_start_cpuCopy.c ========
 *  No DMA (just CPU copy) C model of the acpy3_start() implementation.
 */

#include <std.h>
#include <acpy3.h>
#include <ti/xdais/idma3.h>
#include <acpy3_util.h>

#ifdef _ACPY3_CPUCOPY_

#pragma CODE_SECTION(ACPY3_start, ".text:ACPY3_start") 

/*
 *  ======= ACPY3_start ========
 *  Submit a single or linked transfer using the most recently. 
 *  configured transfer parameter settings.
 */
void ACPY3_start(IDMA3_Handle handle)
{
    if (handle->transferPending) {
        /* Wait for previously started transfer (if any) to complete */
        ACPY3_wait(handle);
    }

    /* 
     * As soon as channel is clear mark the channel state to indicate a 
     * new transfer is being issued and 'transfer is pending'.
     * Only a subsequent 'ACPY3_wait' will clear the pending state.
     */
    handle->transferPending = TRUE;
}


#endif

/*
 *  @(#) ti.sdo.fc.acpy3; 1, 0, 0,66; 5-10-2007 10:49:19; /db/atree/library/trees/fc-b12x/src/
 */

