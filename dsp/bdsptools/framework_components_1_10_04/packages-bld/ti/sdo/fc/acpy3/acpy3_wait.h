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
 *  ======== acpy3_wait.h ========
 */


#ifdef _ACPY3_

# ifndef _ACPY3_WAIT_
# define _ACPY3_WAIT_

#include <ti/xdais/idma3.h>
#include <ti/sdo/fc/acpy3/acpy3_qdma.h>
#include <ti/sdo/fc/acpy3/acpy3_util.h>

#ifndef _ACPY3_CPUCOPY_

#if defined(ACPY3_WAIT_INLINE) || defined(ACPY3_INLINE_ALL)
ACPY3_VERSION_MACRO
static __inline 
#endif
#if defined(ACPY3_WAIT_INLINE) || defined(ACPY3_INLINE_ALL) || \
    defined(ACPY3_WAIT_IMPLEMENTATION)

/*
 *  ======== ACPY3_wait ========
 */
Void ACPY3_wait(IDMA3_Handle restrict handle)
{
    Uns                       csr;
    IDMA3_Handle  * restrict  lockPtr;
    
    ACPY3_MoreEnv * restrict  moreEnv;
    volatile Uns             *iprRegister; /* QDMA Interrupt pending register */
    Uns                       iprMask;  
    Uns                       icrlMask;  
    Uns                       icrhMask;  
    Uns                       pending;
    Uns                       StatusReg;
    IDMA3_Handle              prevLock;
  
#if defined(ACPY3_WAIT_IMPLEMENTATION)
    DBC_require(handle != NULL);
#endif    

    moreEnv = (ACPY3_MoreEnv *)(handle->env);         /* read in advance ptr */
    pending = handle->transferPending;                /* read in advance */
    lockPtr = &_ACPY3_qchLockTbl[handle->qdmaChan];   /* read in advance ptr */
    handle->transferPending = FALSE;                  /* reset in advance */

    iprRegister = (volatile Uns *)moreEnv->iprRegister; /* read in advance */
    iprMask = moreEnv->iprMask;                       /* read in advance */
    icrlMask = moreEnv->icrlMask;                     /* read in advance */
    icrhMask = moreEnv->icrhMask;                     /* read in advance */
    StatusReg = *iprRegister;                         /* read STATUS */

    /* 
     * Wait for a channel only once. Set the 'pending' flag whenever new
     * transfer is submitted, then clear it when ACPY3_wait* is issued.
     */
    if (!pending) 
        return;

    while (!(StatusReg & iprMask)) {    /* wait on the final TCC being set */
        StatusReg = *iprRegister;
    }  

    /* 
     * Mark the QDMA channel as unlocked.
     * This must be done atomically.
     */
    csr = _disable_interrupts();
    prevLock = *lockPtr;
    
    /* Clear all the TCCs for the channel (moved here for removing nops) */
    if (icrlMask)
       *ACPY3_QDMA_ICR = icrlMask;
    if (icrhMask)
       *ACPY3_QDMA_ICRH = icrhMask;
       
    if (prevLock == handle)
        *lockPtr = 0;
    _restore_interrupts(csr); /* reenable interrupts */
}
#endif /* if defined(ACPY3_WAIT_INLINE) || 
       **    defined(ACPY3_INLINE_ALL) || 
       **    defined(ACPY3_WAIT_IMPLEMENTATION)
       */
#  endif /* ifndef _ACPY3_CPUCOPY_ */
# endif /* _ACPY3_WAIT_ */
#else
#error "ti/sdo/fc/acpy3/acpy3.h must be included first"
#endif 

/*
 *  @(#) ti.sdo.fc.acpy3; 1, 0, 0,66; 5-10-2007 10:49:19; /db/atree/library/trees/fc-b12x/src/
 */

