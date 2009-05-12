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
 *  ======== acpy3_waitlinked.c ========
 */


#include <std.h>

#include <dbc.h>

#include <ti/xdais/idma3.h>
#include <acpy3.h>
#include <acpy3_qdma.h>
#include <acpy3_util.h>

#ifndef _ACPY3_CPUCOPY_

#pragma CODE_SECTION(ACPY3_waitLinked, ".text:ACPY3_waitLinked")

/*
 *  ======== waitForTransferToComplete ========
 *  Wait for a submitted DMA transfer on this logical channel associated
 *  with the assigned TCC to complete.  
 */

static __inline Void waitForTransferToComplete(Uns tcc)
{
    /*
     * Determine completion of a QDMA transfer by polling on the Transfer
     * Completion Code (TCC) assigned to the channel. For linked transfers 
     * wait on the TCC for the last transfer. 
     */
    Uns           mask;  
    volatile Uns *iprRegister; /* QDMA Interrupt pending register */

    /* 
     *  There are 64 transfer completion codes on Davinci. 
     *  0-31 associated with register IPR 32-63 with IPRH.  
     *  Select proper mask and register based on assigned TCC.
     *  
     *  NOTE: Possible Optimization Item: store the mask and IPR info 
     *  in the channel handle during ACPY3_configure to avoid the 
     *  extra conditionals each time.
     */
    if (tcc < 32) {
        iprRegister = ACPY3_QDMA_IPR;
        mask = (0x1 << tcc);
    }
    else {
        iprRegister = ACPY3_QDMA_IPRH;
        mask = (0x1 << (tcc - 32));
    }
       
   /*
    *  Poll on the IPR register until the channel's TCC bit is set 
    */
    while (!(*iprRegister & mask));
} 


/* 
 *  ======= ACPY3_waitLinked ========
 * Blocking wait for an individual transfer in a Linked transfer to finish.
 * Requires the individual transfer indicated by the 'waitId'
 * to be configured as synchronous, otherwise returns immediately.
 */
Void ACPY3_waitLinked(IDMA3_Handle handle, MdUns waitId)
{
    DBC_require((handle != NULL) && (waitId <= handle->numTccs - 1));

    /* 
     * Wait for a channel only once. Set the 'pending' flag whenever new
     * transfer is submitted, then clear it when ACPY3_wait* is issued.
     */
    if (handle->transferPending == FALSE) {
        return;  /* already  waited since last start. */
    }

    if (waitId == (handle->numTccs - 1) ) {
        ACPY3_wait(handle);
    } 
    else {
        waitForTransferToComplete(handle->tccTable[waitId + 1]);
    }
    /* 
     * NOTE: Individual transfer TCC bits for intermediate transfers 
     * are not cleared until the last transfer is waited on.
     */
}
 
#endif 

/*
 *  @(#) ti.sdo.fc.acpy3; 1, 0, 0,66; 5-10-2007 10:49:19; /db/atree/library/trees/fc-b12x/src/
 */

