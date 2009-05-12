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
 *  ======== acpy3_fastsetfinal.h ========
 */


#ifdef _ACPY3_

#ifndef _ACPY3_FASTSETFINAL_
#define _ACPY3_FASTSETFINAL_

#include <ti/xdais/idma3.h>
#include <ti/sdo/fc/acpy3/acpy3_util.h>
#include <ti/sdo/fc/acpy3/acpy3.h>

#if defined(ACPY3_FASTSETFINAL_INLINE) || defined(ACPY3_INLINE_ALL)
ACPY3_VERSION_MACRO
static __inline 
#endif
#if defined(ACPY3_FASTSETFINAL_INLINE) || defined(ACPY3_INLINE_ALL) || \
    defined(ACPY3_FASTSETFINAL_IMPLEMENTATION)

/*
 *  ======== ACPY3_fastSetFinal ========
 */
Void ACPY3_fastSetFinal(IDMA3_Handle restrict handle, MdInt transferNo)
{
    ACPY3_PaRamRegs * restrict paRamCacheOld;
    ACPY3_PaRamRegs * restrict paRamCacheNew;
    ACPY3_MoreEnv   * restrict env;
    MdInt                      transferNoP1;
    MdInt                      finalP1;
#if _ACPY3_DIRTY_BITS
    MdInt                      final;
    SmUns           * restrict dirtyBitTblPtr;
#endif

#if defined(ACPY3_FASTSETFINAL_IMPLEMENTATION)
    DBC_require(handle != NULL);
    DBC_require(handle->env != NULL);
    DBC_require(handle->protocol == &ACPY3_PROTOCOL);
    DBC_require((transferNo < ((ACPY3_MoreEnv *)(handle->env))->numPaRams) && 
                (transferNo >= 0));
    /* The handle is only allowed a single TCC */            
    DBC_require(((ACPY3_MoreEnv *)(handle->env))->numTccs == 1);
#endif /* if defined(ACPY3_FASTSETFINAL_IMPLEMENTATION) */

    env = (ACPY3_MoreEnv *)handle->env;
    /* The previous transfer number that was configured as final. */
    finalP1 = (MdInt)handle->numPaRams;
    transferNoP1 = transferNo + 1;
    handle->numPaRams = transferNoP1;
    
#if _ACPY3_DIRTY_BITS
    final = finalP1 - 1;
#endif

    /* paRamCacheOld = (ACPY3_PaRamRegs *)ACPY3_getPaRamCache(handle,final); */
    /* paRamCacheNew = (ACPY3_PaRamRegs *)ACPY3_getPaRamCache(handle, transferNo); */
    paRamCacheOld = (ACPY3_PaRamRegs *)env - finalP1;
    paRamCacheNew = (ACPY3_PaRamRegs *)env - transferNoP1;

#if _ACPY3_DIRTY_BITS
    dirtyBitTblPtr = env->dirtyBitTable;
#endif

    /*
     *  Reset the link id of the previous transfer that was configured as
     *  final, unless it is the last PaRam allocated to the channel.
     */

    paRamCacheOld->link = (MdUns)ACPY3_getPaRamLinkIndex(handle, finalP1);

    /*
     *  Clear the static bit since "final" is no longer the last transfer.
     *  Clear the old tcc and TCC_INTEN.
     */
    paRamCacheOld->opt &= ~(ACPY3_TCCMASK             | 
                            ACPY3_PARAMOPTS_TCC_INTEN | 
                            ACPY3_PARAMOPTS_STATIC);
    
    /* 
     * Update the paRam cache for the new final transfer.
     * Set up final transfer with no linking.
     */
    
    paRamCacheNew->link = 0xffff;

    /*
     *  Since this will be the last transfer, we need to use a tcc. 
     *  Since the last transfer always uses the first tcc in the table, 
     *  we update the opt field of the PaRam to use that. 
     */

    paRamCacheNew->opt = ACPY3_TCCBITS(handle->tccTable[0]) |   /* TCC_CHAN */
                         ACPY3_PARAMOPTS_AB_SYNC            |
                         ACPY3_PARAMOPTS_TCC_INTEN          |
                         ACPY3_PARAMOPTS_STATIC;

#if _ACPY3_DIRTY_BITS
    /* Mark the link field as dirty, since it has changed. */
    dirtyBitTblPtr[final] |= ACPY3_DIRTYLINK;

    /* Mark the opts field of the PaRam for this transfer as dirty. */
    dirtyBitTblPtr[transferNo] |= ACPY3_DIRTYLINK | ACPY3_DIRTYOPTS;
#endif
}
#endif /* if defined(ACPY3_FASTSETFINAL_INLINE) || 
       **    defined(ACPY3_INLINE_ALL) || 
       **    defined(ACPY3_FASTSETFINAL_IMPLEMENTATION) 
       */
#endif /* _ACPY3_FASTSETFINAL_ */
#else
#error "ti/sdo/fc/acpy3/acpy3.h must be included first"
#endif 

/*
 *  @(#) ti.sdo.fc.acpy3; 1, 0, 0,66; 5-10-2007 10:49:19; /db/atree/library/trees/fc-b12x/src/
 */

