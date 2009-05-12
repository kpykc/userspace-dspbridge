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
 *  ======== acpy3_wait_cpucopy.c ========
 */


#include <std.h>

#include <dbc.h>

#include <ti/xdais/idma3.h>
#include <acpy3_util.h>
 
#ifdef _ACPY3_CPUCOPY_

#pragma CODE_SECTION(ACPY3_wait, ".text:ACPY3_wait")


static Void doTransfer(IDMA3_Handle handle);

/*
 *  ======== ACPY3_wait ========
 *  Wait for all submitted DMA transfer on this logical channel to complete.  
 */
Void ACPY3_wait(IDMA3_Handle handle)
{
    DBC_require(handle != NULL);

    /* 
     * Wait for a channel only once. Set the 'pending' flag whenever new
     * transfer is submitted, then clear it when ACPY3_wait* is issued.
     */
    if (!handle->transferPending) {
        return;  /* already  waited since last start. */
    }

    doTransfer(handle);

    /* 
     * Mark channel handle state to indicate pending transfer has completed 
     * and 'wait' done 
     */
    handle->transferPending = FALSE;

    DBC_ensure(handle->transferPending == FALSE);
}

/* 
 *  ======= ACPY3_waitLinked ========
 * Blocking wait for an individual transfer in a Linked transfer to finish.
 * Requires the individual transfer indicated by the 'waitId'
 * to be configured as synchronous, otherwise returns immediately.
 */
Void ACPY3_waitLinked(IDMA3_Handle handle, MdUns waitId)
{
    DBC_require((handle != NULL) && (waitId < handle->numTccs) );

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
        doTransfer(handle);
    }

    /* 
     * Mark channel handle state to indicate pending transfer has completed 
     * and 'wait' done 
     */
    handle->transferPending = FALSE;
}


/*
 *  ======== doTransfer ========
 */
static Void doTransfer(IDMA3_Handle handle)
{
    SmUns *src, *dst;
    SmUns *srcFrameStart, *dstFrameStart;
    ACPY3_PaRamRegs *paRamCache;           
    Int numTransfers = handle->numPaRams;
    Int transferNo;
    MdUns ccnt;
    MdUns bcnt;
    MdUns acnt;
    MdInt srcElementIndex;
    MdInt dstElementIndex;
    MdInt srcFrameIndex;
    MdInt dstFrameIndex;

    for (transferNo = 0; transferNo < numTransfers; transferNo++) {
        /*
         * Use each transfer's cached paRam settings. The 
         */
        paRamCache = (ACPY3_PaRamRegs *)ACPY3_getPaRamCache(handle,
                transferNo);

        src  = paRamCache->src;
        dst  = paRamCache->dst;
        acnt = paRamCache->acnt;
        bcnt = paRamCache->bcnt;

        /*
         *  QDMA can only transfer a single frame, so ccnt must always be 1.
         */
        ccnt = 1;

        srcElementIndex = paRamCache->srcElementIndex;
        dstElementIndex = paRamCache->dstElementIndex;
        srcFrameIndex = paRamCache->srcFrameIndex;
        dstFrameIndex = paRamCache->dstFrameIndex;

        while (ccnt-- > 0) {
            /* Keep track of the start of the frame */
            srcFrameStart = src;
            dstFrameStart = dst;

            /* Copy a frame */
            while (bcnt-- > 0) {

                /* Copy an element */
                while (acnt-- > 0) {
                    *dst++ = *src++;
                }

                /* 
                 * Reset ACNT counter. 
                 * Advance src/dst ptrs to beginning of next line/1D-vector 
                 */
                acnt = paRamCache->acnt;
                src = (src - acnt) + srcElementIndex ;
                dst = (dst - acnt) + dstElementIndex;
            }

            /* adjust src and dst to the beginning of a frame + frameIndex*/
            bcnt = paRamCache->bcnt;
            src = srcFrameStart + srcFrameIndex;
            dst = dstFrameStart + dstFrameIndex;
        }
    } /* for each transfer */
}

#endif 

/*
 *  @(#) ti.sdo.fc.acpy3; 1, 0, 0,66; 5-10-2007 10:49:19; /db/atree/library/trees/fc-b12x/src/
 */

