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
 *  ======== acpy3_setfinal.h ========
 */

#ifdef _ACPY3_

#ifndef _ACPY3_SETFINAL_
#define _ACPY3_SETFINAL_

#include <ti/xdais/idma3.h>
#include <ti/sdo/fc/acpy3/acpy3_util.h>
#include <ti/sdo/fc/acpy3/acpy3.h>

#if defined(ACPY3_SETFINAL_INLINE) || defined(ACPY3_INLINE_ALL)
ACPY3_VERSION_MACRO
static __inline 
#endif
#if defined(ACPY3_SETFINAL_INLINE) || defined(ACPY3_INLINE_ALL) || \
    defined(ACPY3_SETFINAL_IMPLEMENTATION)

/*
 *  ======== ACPY3_setFinal ========
 */
Void ACPY3_setFinal(IDMA3_Handle restrict handle, MdInt transferNo)
{
    ACPY3_PaRamRegs * restrict paRamCache;
    ACPY3_MoreEnv   * restrict env;
    SmUns           * restrict tccTblPtr;
    MdUns                      numTotalPaRams;
    MdUns                      final;
    MdUns                      tcc;
    Int                        i, j;
    Int                        moreNumTccs;
    Int                        prevWaitIdOfFinal;
#if _ACPY3_DIRTY_BITS
    SmUns           * restrict dirtyBitTblPtr;
#endif

#if defined(ACPY3_SETFINAL_IMPLEMENTATION)
    DBC_require(handle != NULL);
    DBC_require(handle->env != NULL);
    DBC_require(handle->protocol == &ACPY3_PROTOCOL);
#endif    

    env = (ACPY3_MoreEnv *)handle->env;
    numTotalPaRams = env->numPaRams;
#if defined(ACPY3_SETFINAL_IMPLEMENTATION)
    DBC_require((transferNo < numTotalPaRams) && (transferNo >= 0));
#endif    

    /* The previous transfer number that was configured as final. */
    final = handle->numPaRams - 1;

    if ((numTotalPaRams == 1) || (transferNo == final)) {
        /* Nothing to do */
        return;
    }

#if defined(ACPY3_SETFINAL_IMPLEMENTATION)
    /* Following code makes this assumption. */
    DBC_assert(transferNo != final);
#endif    

    tccTblPtr = handle->tccTable;
            
#if _ACPY3_DIRTY_BITS
    dirtyBitTblPtr = env->dirtyBitTable;
#endif

    /*
     *  Reset the link id of the previous transfer that was configured as
     *  final, unless it is the last PaRam allocated to the channel.
     */

    if (final < (numTotalPaRams - 1)) {
        /* paRamCache = (ACPY3_PaRamRegs *)ACPY3_getPaRamCache(handle, final); */
        paRamCache = (ACPY3_PaRamRegs *)env - (final + 1);
        
        prevWaitIdOfFinal = env->prevWaitIdOfFinal;
        
        paRamCache->link = (MdUns)ACPY3_getPaRamLinkIndex(handle, final + 1);


        /*
         *  Clear the static bit since "final" is no longer the last transfer.
         *  Clear the old tcc and TCC_INTEN.
         */
        paRamCache->opt &= ~(ACPY3_TCCMASK | 
                             ACPY3_PARAMOPTS_TCC_INTEN | 
                             ACPY3_PARAMOPTS_STATIC);
    
        /*
         * If the previous "final" paRam used to have a waitId 
         *  associated with it, restore that one.
         */
        if (prevWaitIdOfFinal >= 0)
        {
            tcc  = tccTblPtr[prevWaitIdOfFinal + 1];
            paRamCache->opt = ACPY3_TCCBITS(tcc)       |   /* TCC_CHAN */
                              ACPY3_PARAMOPTS_AB_SYNC  |
                              ACPY3_PARAMOPTS_TCC_INTEN;
        }   
        
#if _ACPY3_DIRTY_BITS
        /* Mark the link field as dirty, since it has changed. */
        /* Mark the opt field as dirty */
        dirtyBitTblPtr[final] |= ACPY3_DIRTYOPTS | ACPY3_DIRTYLINK;
#endif
    }

    /* 
     * Update the paRam cache for the new final transfer 
     */
    
    /* paRamCache = (ACPY3_PaRamRegs *)ACPY3_getPaRamCache(handle, transferNo); */
    paRamCache = (ACPY3_PaRamRegs *)env - (transferNo + 1);

    handle->numPaRams = transferNo + 1;
    
    /* Set up final transfer with no linking */
    paRamCache->link = 0xffff;


    /*
     *  Since this will be the last transfer, we need to use a tcc, 
     *  whether or not the transfer was configured with a waitId >= 0.
     *  
     *  Since the last transfer always uses the first tcc in the table, 
     *  we update the opt field of the PaRam to use that. If the transfer 
     *  was configured with a waitId >= 0, we must guarentee that when 
     *  ACPY3_waitLinked() is called with that waitId, we use the first tcc. 
     *  Therefore, we set handle->numWaits to waitId + 1.
     *  Determine if a TCC was set for this transfer by checking the
     *  TCC_INT_EN bit of the opts field. If so, extract the tcc and
     *  use it to determine the waitId so we can set numWaits.
     */
    if (paRamCache->opt & ACPY3_PARAMOPTS_TCC_INTEN) {
        tcc = (paRamCache->opt & ACPY3_TCCMASK) >> ACPY3_TCCSHIFT;
#pragma MUST_ITERATE(1)        
        for (i = 0; i < env->numTccs; i++) {
            if (tccTblPtr[i] == tcc)
                break;
        }

        /* i = waitId + 1 */
#if defined(ACPY3_SETFINAL_IMPLEMENTATION)
        DBC_assert(i < env->numTccs);
#endif        

        /*
         *  TCC used is handle->tccTable[waitId + 1], or tccTable[0]
         *  for the last transfer.
         */
        env->prevWaitIdOfFinal = i - 1;
    }
    else {
        /*
         *  This transfer is not a synchronization point, so we don't really
         *  care that handle->numWaits may be more than the actual number
         *  of calls to ACPY3_waitLinked().
         */
        env->prevWaitIdOfFinal = -1;
    }

    tcc = tccTblPtr[0];
    paRamCache->opt = ACPY3_TCCBITS(tcc)        |   /* TCC_CHAN */
                      ACPY3_PARAMOPTS_AB_SYNC   |
                      ACPY3_PARAMOPTS_TCC_INTEN |
                      ACPY3_PARAMOPTS_STATIC;

#if _ACPY3_DIRTY_BITS
    /* Mark the opts field of the PaRam for this transfer as dirty. */
    dirtyBitTblPtr[transferNo] |= ACPY3_DIRTYLINK | ACPY3_DIRTYOPTS;
#endif

    /*
     * Determine total number of tccs
     */
    if (((moreNumTccs = env->numTccs) > 1) && (handle->numPaRams > 1)) {
        
        /* Determine highest intermediate tcc */
#pragma MUST_ITERATE(1)        
        for (j = handle->numPaRams - 1; j > 0; j--) {
            paRamCache++;
            if (paRamCache->opt & ACPY3_PARAMOPTS_TCC_INTEN) {
                tcc = (paRamCache->opt & ACPY3_TCCMASK) >> ACPY3_TCCSHIFT;
                /* Skip over tccTable[0] because it's only used 
                 * for the "final" paRam.
                 */
#pragma MUST_ITERATE(1)        
                for (i = 1; i < moreNumTccs; i++)
                {
                    if (tccTblPtr[i] == tcc) {
                        handle->numTccs = i + 1;
                        return;
                    }
                }
            }            
        }
    }
    handle->numTccs = 1;
}
#endif /* if defined(ACPY3_SETFINAL_INLINE) || defined(ACPY3_INLINE_ALL) || 
       ** defined(ACPY3_SETFINAL_IMPLEMENTATION) 
       */
#endif /* _ACPY3_SETFINAL_ */
#else
#error "ti/sdo/fc/acpy3/acpy3.h must be included first"
#endif 


 
/*
 *  @(#) ti.sdo.fc.acpy3; 1, 0, 0,66; 5-10-2007 10:49:19; /db/atree/library/trees/fc-b12x/src/
 */

