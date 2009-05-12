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
 *  ======== acpy3_complete.c ========
 */
 
#pragma CODE_SECTION(ACPY3_complete, ".text:ACPY3_complete")

#include <std.h>

#include <dbc.h>

#include <ti/xdais/idma3.h>
#include <acpy3.h>
#include <acpy3_qdma.h>
#include <acpy3_util.h>


#ifndef _ACPY3_CPUCOPY_

static Bool checkForCompletion(Uns tcc);

#endif


/*
 *  ======== ACPY3_complete ========
 *  Check to see if all dma transfers on logical channel are finished. 
 *  This is a non-blocking call.
 */
Bool ACPY3_complete(IDMA3_Handle handle)
{       
#ifndef _ACPY3_CPUCOPY_
    Uns                    csr;
    IDMA3_Handle          *lockPtr;
    ACPY3_MoreEnv         *moreEnv;
    Uns                    icrlMask;  
    Uns                    icrhMask;  

    DBC_require(handle != NULL);

    if (!handle->transferPending) {
        /* No tansfers pending - channel already waited on since last start */
        return (TRUE);
    }

    /* 
     * Wait for a channel only once. Set the 'pending' flag whenever new
     * transfer is submitted, then clear it when ACPY3_wait() is issued.
     */
    moreEnv = (ACPY3_MoreEnv *)(handle->env);
    if (!(*moreEnv->iprRegister & moreEnv->iprMask))
       return FALSE;
       
    /* Clear all the TCCs for the channel */
    icrlMask = moreEnv->icrlMask;
    icrhMask = moreEnv->icrhMask;
    if (icrlMask)
       *ACPY3_QDMA_ICR = icrlMask;
    if (icrhMask)
       *ACPY3_QDMA_ICRH = icrhMask;
       
    /* 
     * Mark the QDMA channel as unlocked.
     * This must be done atomically.
     */
    lockPtr = &_ACPY3_qchLockTbl[handle->qdmaChan];
    csr = _disable_interrupts();
    if (*lockPtr == handle)
        *lockPtr = 0;
   _restore_interrupts(csr); /* reenable interrupts */
       
   handle->transferPending = FALSE;
        
   return (TRUE);   

#else
    DBC_require(handle != NULL);

    if (handle->transferPending) {
        ACPY3_wait(handle);
        return (TRUE);
    }
    else {
        /* No tansfers pending - channel already waited on since last start */
        return (TRUE);
    }
#endif
}

/* 
 *  ======== ACPY3_completeLinked ========
 * Non-blocking query to check completion of an individual
 * transfer in a Linked transfer.
 */
Bool ACPY3_completeLinked(IDMA3_Handle handle, MdUns waitId)
{
#ifndef _ACPY3_CPUCOPY_
    Uns     tcc;
#endif
    
    DBC_require((handle != NULL) && (waitId < handle->numTccs));
    
    if (!handle->transferPending) {
        /* No tansfers pending - channel already waited on since last start */
        return (TRUE);
    }
    if (waitId == (handle->numTccs - 1))
        return (ACPY3_complete(handle));


#ifndef _ACPY3_CPUCOPY_
    tcc = handle->tccTable[waitId + 1];

    /* 
     * Wait for a channel only once. Set the 'pending' flag whenever new
     * transfer is submitted, then clear it when ACPY3_wait is issued.
     */
    return (checkForCompletion(tcc));

#else
    return (ACPY3_complete(handle));
#endif
}

#ifndef _ACPY3_CPUCOPY_
/*
 *  ======== checkForCompletion ========
 */
static Bool checkForCompletion(Uns tcc)
{   
    Uns              mask;
    volatile Uns    *iprRegister; /* QDMA Interrupt pending register */
    
    /*
     * Determine completion of a QDMA transfer by polling on the Transfer
     * Completion Code (TCC) assigned to the channel. For linked transfers 
     * wait on the TCC for the last transfer. 
     */

    /* 
     *  There are 64 transfer completion codes on Davinci. 
     *  0-31 associated with register IPR 32-63 with  IPRH.  
     *  Select proper mask and register based on assigned TCC.
     *  
     *  NOTE: Optimization TODO Item: store the mask and IPR info in
     *  the channel handle during ACPY3_configure to avoid the extra
     *  conditionals each time.
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
    *  Query the IPR register to check if the channel's TCC bit is set 
    */
    if ((*iprRegister) & mask) {
        return (TRUE);
    }
    else {
        return (FALSE);
    }
}
#endif

/*
 *  @(#) ti.sdo.fc.acpy3; 1, 0, 0,66; 5-10-2007 10:49:19; /db/atree/library/trees/fc-b12x/src/
 */

