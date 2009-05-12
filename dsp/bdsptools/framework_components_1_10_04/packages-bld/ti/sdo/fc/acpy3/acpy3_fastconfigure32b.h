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
 *  ======== acpy3_fastconfigure32b.h ========
 */

#ifdef _ACPY3_

# ifndef _ACPY3_FASTCONFIGURE32B_
#define _ACPY3_FASTCONFIGURE32B_

#include <ti/xdais/idma3.h>
#include <ti/sdo/fc/acpy3/acpy3_qdma.h>
#include <ti/sdo/fc/acpy3/acpy3_util.h>

#if defined(ACPY3_FASTCONFIGURE32B_INLINE) || defined(ACPY3_INLINE_ALL)
ACPY3_VERSION_MACRO
static __inline 
#endif
#if defined(ACPY3_FASTCONFIGURE32B_INLINE) || defined(ACPY3_INLINE_ALL) || \
    defined(ACPY3_FASTCONFIGURE32B_IMPLEMENTATION)

/*
 *  ======== ACPY3_fastConfigure32b ========
 */
Void ACPY3_fastConfigure32b(IDMA3_Handle restrict handle, 
                            ACPY3_ParamField32b fieldId,
                            Uns value, MdInt transferNo)
{    
    ACPY3_PaRamRegs * restrict paRamCache;
# if _ACPY3_DIRTY_BITS
    ACPY3_MoreEnv   * restrict env;
    MdInt                      transferNoP1;
# endif

# if defined(ACPY3_FASTCONFIGURE32B_IMPLEMENTATION)
    DBC_require(handle != NULL);
    DBC_require(handle->protocol == &ACPY3_PROTOCOL);
    DBC_require(transferNo >= 0);
    DBC_require(transferNo < ((ACPY3_MoreEnv *)(handle->env))->numPaRams);
    DBC_require(!handle->transferPending);
# endif /* if defined(ACPY3_FASTCONFIGURE32B_IMPLEMENTATION) */
    
# if _ACPY3_DIRTY_BITS
    env = ((ACPY3_MoreEnv *)handle->env);
    /*
     * Mark a bit in the dirty bit table for this PaRAM as dirty
     */
    env->dirtyBitTable[transferNo] |= 1 << ((Int)fieldId >> 2);
    transferNoP1 = transferNo + 1;
    paRamCache = (ACPY3_PaRamRegs *)env - transferNoP1;
# else    
    paRamCache = (ACPY3_PaRamRegs *)ACPY3_getPaRamCache(handle, transferNo);
# endif

    *((Uns *)((Uns)paRamCache + fieldId)) = value;
}
#endif /* if defined(ACPY3_FASTCONFIGURE32B_INLINE) || 
       **    defined(ACPY3_INLINE_ALL) || 
       **    defined(ACPY3_FASTCONFIGURE32B_IMPLEMENTATION) 
       */
#endif /* _ACPY3_FASTCONFIGURE32B_ */
#else
#error "ti/sdo/fc/acpy3/acpy3.h must be included first"
#endif 

/*
 *  @(#) ti.sdo.fc.acpy3; 1, 0, 0,66; 5-10-2007 10:49:19; /db/atree/library/trees/fc-b12x/src/
 */

