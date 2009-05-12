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
 * ======== acpy3_configure.c ======== 
 * Configure a logical channel
 */
 
#include <std.h>

#include <dbc.h>

#include <ti/xdais/idma3.h>
#include <acpy3_util.h>
#include <acpy3_qdma.h>
#include <acpy3.h>


/*
 *  ======= ACPY3_configure ========
 *  Configure all DMA transfer settings for the logical channel.
 */
#pragma CODE_SECTION(ACPY3_configure, ".text:ACPY3_configure")
Void ACPY3_configure(IDMA3_Handle handle, ACPY3_Params * restrict params, 
        MdInt transferNo)
{
    ACPY3_PaRamRegs * restrict paRamCache;
    MdUns            tcc;
    MdInt            waitId;
    ACPY3_MoreEnv   * restrict env;

    DBC_require(handle != NULL);
    DBC_require(handle->protocol == &ACPY3_PROTOCOL);
    DBC_require(params->waitId < ((ACPY3_MoreEnv *)(handle->env))->numTccs);
    DBC_require(transferNo >= 0);
    DBC_require(transferNo < ((ACPY3_MoreEnv *)(handle->env))->numPaRams);
    DBC_require(params->numFrames < 2);
    DBC_require(!handle->transferPending);

    env = (ACPY3_MoreEnv *)(handle->env);
    
#if 0
    paRamCache = (ACPY3_PaRamRegs *)ACPY3_getPaRamCache(handle, transferNo);
#else    
    paRamCache = (ACPY3_PaRamRegs *)env - (transferNo + 1);
#endif    
    
    /*
     * If the waitId is set to the last allocated waitId, just ignore it.
     * This will be handled by ACPY3_setFinal instead.
     */
    if ((waitId = params->waitId) == env->numTccs - 1)
        waitId = -1;

#if _ACPY3_DIRTY_BITS
    /*
     * Mark all bits in the dirty bit table for this PaRAM as dirty
     */
    env->dirtyBitTable[transferNo] = 0x3f;
#endif /* if _ACPY3_DIRTY_BITS */
    
    /* #1 */
    paRamCache->src = params->srcAddr;

    /* #2 */
    paRamCache->acnt = params->elementSize;
    paRamCache->bcnt = 1; /* default for 1D1D */

    /* #4 */
    paRamCache->dst = params->dstAddr;

    /* #5 - Set bcnt reload value to bcnt, set to no link */
    paRamCache->bCntrld = params->numElements;

    if (params->transferType != ACPY3_1D1D) {
 
        /* Field #2: update with 2nd dimension */
        paRamCache->bcnt = params->numElements;

        /* Field #3: set Element Indexes for 2D2D */
        paRamCache->srcElementIndex = params->srcElementIndex;
        paRamCache->dstElementIndex = params->dstElementIndex;
         
        if (params->transferType == ACPY3_1D2D) {
            paRamCache->srcElementIndex = params->elementSize;
        }
        else if (params->transferType == ACPY3_2D1D) {
            paRamCache->dstElementIndex = params->elementSize;
        }
    }

    /* 
     * If configuring handle with linked transfers then
     * adjust the 'link' and 'tcc' synchronization, etc., in opt.
     */
    if ((transferNo < (env->numPaRams - 1)) &&
        (transferNo != (handle->numPaRams - 1))) {    

        /* #5 */
        /*
         *  Setup this param's link field to point to the NEXT transfer when
         *  setting up linked transfers we assign PARAM addresses in
         *  descending order, so that the last entry is the first transfer,
         *  etc., handle's paRamAddr is pointing to the 'last' PARAM entry
         *  assigned to the channel handle (along with the preceding
         *  "numTransfer" PARAM entries.
         */
        paRamCache->link = (MdUns)ACPY3_getPaRamLinkIndex(handle, 
                                                          transferNo + 1);

        /* setup tcc for intermediate transfers waits and unset STATIC */

        if (waitId >= 0) {        
            tcc  = handle->tccTable[waitId + 1];
            /*
             * If this is the highest intermediate waitId,
             * then update handle->numTccs
             */
            if (waitId > ((int)handle->numTccs) - 2)                  
                handle->numTccs = waitId + 2;                 
            paRamCache->opt = ACPY3_TCCBITS(tcc)       |   /* TCC_CHAN */
                              ACPY3_PARAMOPTS_AB_SYNC  |
                              ACPY3_PARAMOPTS_TCC_INTEN;
        } 
        else { /* cannot wait on this transfer */
            paRamCache->opt = ACPY3_PARAMOPTS_AB_SYNC;
        }
    }
    else {
        /* #5 - Set no link for final PaRam */
        paRamCache->link = 0xffff;
        
        env->prevWaitIdOfFinal = waitId;
        
        /* Single transfer or the last one of the Linked transfers */
        /* Pick the tcc from the handle - even when waitId is (-1) */
        tcc = handle->tccTable[0];
        if ((transferNo == handle->numPaRams - 1) && (waitId >= 0)) {
            handle->numTccs = waitId + 1;
        }

        /* #0 */
        paRamCache->opt = ACPY3_TCCBITS(tcc)        |   /* TCC_CHAN */
                          ACPY3_PARAMOPTS_AB_SYNC   |
                          ACPY3_PARAMOPTS_TCC_INTEN |
                          ACPY3_PARAMOPTS_STATIC;
    }
}


 
/*
 *  @(#) ti.sdo.fc.acpy3; 1, 0, 0,66; 5-10-2007 10:49:19; /db/atree/library/trees/fc-b12x/src/
 */

