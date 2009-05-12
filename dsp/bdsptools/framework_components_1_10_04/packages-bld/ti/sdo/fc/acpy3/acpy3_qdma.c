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
 *  ======== acpy3_qdma.c ========
 *
 * New functions that are called to do Channel activation/deactivation 
 * for scratch DMA channels.
 */
#include <std.h>

#include <dbc.h>

#include <ti/xdais/idma3.h>
#include <acpy3.h>
#include <acpy3_qdma.h>
#include <acpy3_util.h>

#pragma CODE_SECTION(ACPY3_activate, ".text:ACPY3_activate")
#pragma CODE_SECTION(ACPY3_deactivate, ".text:ACPY3_deactivate")

/*
 *  ======== ACPY3_activate ========
 */
Void ACPY3_activate(IDMA3_Handle handle)
{
#if _ACPY3_DIRTY_BITS
    SmUns * restrict dirtyBytePtr;
#endif
    Int    i;
    Int    numPaRamsM1;
    Uns    nBytesPaRams;
    Uns    nBytes;
    Uns    nWords;
    ACPY3_MoreEnv *env;
    Void  * restrict scratchEnv;
    Int   * restrict ptrPersist;
    Int   * restrict ptrScratch;
#ifndef _ACPY3_CPUCOPY_
    Uns    paRam;
    Uns    begPaRam;
    Uns    endPaRam;
    MdUns  qdmaChan;
    volatile Uns   *paRamRegs;
    volatile Uns   *lastPaRam;
    Uns    triggerIdx;
    Uns    nullPaRam = ACPY3_QDMA.nullPaRamIndex;
#endif

    DBC_require(handle != NULL);

    env = (ACPY3_MoreEnv *)(handle->env);

    if ((scratchEnv = env->scratchEnv) && (scratchEnv != env)) {
        /*
         * Copy env->persistEnv to env->scratchEnv.
         * Then set env in handle to point to scratch memory until
         * ACPY3_deactivate() is called.
         */
        nBytesPaRams = env->numPaRams * (sizeof(ACPY3_PaRamRegs));
#if _ACPY3_DIRTY_BITS
        nBytes = (sizeof(ACPY3_MoreEnv) - sizeof(SmUns)) +
            nBytesPaRams + (env->numPaRams * sizeof(SmUns));
#else
        nBytes = sizeof(ACPY3_MoreEnv) + nBytesPaRams;
#endif
        ptrPersist = (Int *)(((SmInt *)env) - nBytesPaRams);
        ptrScratch = (Int *)(((SmInt *)scratchEnv) - nBytesPaRams);

        /* Copy 4 bytes at a time for optimization */
        nWords = (nBytes + 3) >> 2;

#pragma MUST_ITERATE((sizeof(ACPY3_MoreEnv) + sizeof(ACPY3_PaRamRegs) + 3) / 4)
        for (i = 0; i < nWords; i++) {
            *ptrScratch++ = *ptrPersist++;
        }
        handle->env = env = scratchEnv;
    }
#ifndef _ACPY3_CPUCOPY_
    qdmaChan = handle->qdmaChan;
#endif
    /* Set CCNT to 1 in all physical PaRams owned by this channel */
    numPaRamsM1 = env->numPaRams - 1;

#ifndef _ACPY3_CPUCOPY_
    /*
     * If any other QDMA channel uses the current PaRam set for this channel,
     * Modify it to use paRam 0 or 127 instead.
     * This shouldn't really happen, but we really need to avoid it.
     */
    endPaRam = ((Uns)handle->paRamAddr) - ACPY3_QDMA_PARAM_BASE;
    begPaRam = endPaRam - (numPaRamsM1 << 5);

    for (i = 0; i < ACPY3_NUM_QDMA_CHANNELS; i++) {
        if (i == qdmaChan) {
            continue;
        }

        paRam = (ACPY3_QCHMAP[i] & 0x3fe0);
        if ((paRam >= begPaRam) && (paRam <= endPaRam)) {
            /*
             *  Set trigger word of other channels to a paRam not in use
             *  (trigger word is ccnt of the null PaRam)
             */
            ACPY3_QCHMAP[i] = (Uns)(nullPaRam << 5) | (0x7 << 2);
        }
    }

    /*
     *  Set the QCHMAP for this channel to the null PaRam, since the static bit
     *  will always be set for it (and we know there is no transfer in progress
     *  on first call to ACPY3_start()).
     */
    ACPY3_QCHMAP[qdmaChan] = (Uns)(nullPaRam << 5) | (0x7 << 2);

    /* Enable QEER */
    *ACPY3_QEESR = 0x1 << ACPY3_QDMA.qdmaChannels[qdmaChan];

    /*
     * Set the last two elements of each real allocated paRam to their
     * expected values.  They should never get written again as long as
     * this handle is activated.
     */
    lastPaRam = (Uns *)(ACPY3_QDMA_PARAM_BASE +
            (ACPY3_QCHMAP[qdmaChan] & 0x3fe0));
    triggerIdx = (ACPY3_QCHMAP[qdmaChan] & (0x7 << 2))  >> 2;

    for (i = env->numPaRams - 1; i >= 0; i--) {
        paRamRegs = (Uns *)ACPY3_getPaRamPhysicalAddress(handle, i);

        if ((paRamRegs == lastPaRam) && (triggerIdx >= 6)) {
            /* set trigger word to link|bCntrld of 1st paRam */
            ACPY3_QCHMAP[qdmaChan] = (Uns)(handle->paRamAddr) | (0x5 << 2);
        }
        paRamRegs[0x6] = 0; /* frame indices = 0 */
        paRamRegs[0x7] = 1; /* ccnt=1, rsvd=0 */
    }
#endif /* _ACPY3_CPUCOPY */

#if _ACPY3_DIRTY_BITS
    /* Set all PaRAM cache bits as "dirty". */
    dirtyBytePtr = env->dirtyBitTable;

    for (i = handle->numPaRams; i > 0; i--) {
        *dirtyBytePtr++ = 0xff;
    }
#endif

    handle->transferPending = FALSE;
    handle->numPaRams = numPaRamsM1 + 1;
    handle->numTccs = env->numTccs;

    return;
}

/*
 *  ======== ACPY3_deactivate ========
 */
Void ACPY3_deactivate(IDMA3_Handle handle)
{
    Uns    nBytesPaRams;
    Uns    nBytes;
    Uns    nWords;
    Int    i;
    ACPY3_MoreEnv *env;
    Void  * restrict scratchEnv;
    Void  * restrict persistEnv;
    Int   * restrict ptrPersist;
    Int   * restrict ptrScratch;
#ifndef _ACPY3_CPUCOPY_
    Uns    nullPaRam = ACPY3_QDMA.nullPaRamIndex;
#endif

    DBC_require(handle != NULL);

    env = (ACPY3_MoreEnv *)(handle->env);

    if ((scratchEnv = env->scratchEnv) && (env == scratchEnv)) {
        /*
         * Copy env->scratchEnv to env->persisthEnv.
         * Then set env in handle to point to persistent shadow until
         * next ACPY3_activate() is called.
         */
        nBytesPaRams = env->numPaRams * (sizeof(ACPY3_PaRamRegs));
        persistEnv = env->persistEnv;
#if _ACPY3_DIRTY_BITS
        nBytes = (sizeof(ACPY3_MoreEnv) - sizeof(SmUns)) +
            nBytesPaRams + (env->numPaRams * sizeof(SmUns));
#else
        nBytes = sizeof(ACPY3_MoreEnv) + nBytesPaRams;
#endif
        ptrScratch = (Int *)(((SmInt *)scratchEnv) - nBytesPaRams);
        ptrPersist = (Int *)(((SmInt *)persistEnv) - nBytesPaRams);

        /* Copy 4 bytes at a time for optimization */
        nWords = (nBytes + 3) >> 2;

#pragma MUST_ITERATE((sizeof(ACPY3_MoreEnv) + sizeof(ACPY3_PaRamRegs) + 3) / 4)
        for (i = 0; i < nWords; i++) {
            *ptrPersist++ = *ptrScratch++;
        }
        handle->env = persistEnv;
    }

#ifndef _ACPY3_CPUCOPY_
    /* Set the QCHMAP back to null PaRam */
    ACPY3_QCHMAP[handle->qdmaChan] = (Uns)(nullPaRam << 5) | (0x7 << 2);
#endif

    return;
}

/*
 *  @(#) ti.sdo.fc.acpy3; 1, 0, 0,66; 5-10-2007 10:49:19; /db/atree/library/trees/fc-b12x/src/
 */

