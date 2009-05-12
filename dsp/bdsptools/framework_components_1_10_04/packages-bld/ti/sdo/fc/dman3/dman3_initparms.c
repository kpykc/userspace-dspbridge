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
 *  ======== dman3_initparms.c ========
 *  Initialize the DMAN module
 */

#pragma CODE_SECTION(_DMAN3_initParams, ".text:DMAN3_init");

#include <std.h>

#include <dman3.h>

/*
 *  Number of words that contain packed TCCs and PaRams.
 */
#define NUMPACKEDGROUPTCCS DMAN3_MAXGROUPS / 4
#define NUMPACKEDGROUPPARAMS DMAN3_MAXGROUPS / 2

/* Offset of PaRams for base address of EDMA3/QDMA registers */
#define PARAMOFFSET (0x4000 / sizeof(Uns))

/*
 *  Variables initialized with configuration parameters in DMAN3_init macro.
 */
far LgUns DMAN3_edma3Base;

far Uns DMAN3_paRamBase;
far Uns DMAN3_maxPaRamEntries;
far Uns DMAN3_numPaRamEntries;
far Uns DMAN3_nullPaRam;

far Uns DMAN3_numQdmaChannels;
extern far Uns DMAN3_qdmaChannels[];

far Uns DMAN3_maxQdmaChannels;
far LgUns DMAN3_qdmaChannelMask;

far LgUns DMAN3_tccMask_h;
far LgUns DMAN3_tccMask_l;

far Int DMAN3_internalHeap;
far Int DMAN3_externalHeap;

far Uns DMAN3_idma3Internal;

/*
 *  Array of TCCs assigned to each group, packed 4 to 32-bit word
 */
far LgUns DMAN3_groupTccs[NUMPACKEDGROUPTCCS];

/*
 *  Array of PaRams assigned to each group, packed 2 to 32-bit word
 */
far LgUns DMAN3_groupPaRams[NUMPACKEDGROUPPARAMS];

far DMAN3_ScratchAllocFxn DMAN3_allocScratch;
far DMAN3_ScratchFreeFxn DMAN3_freeScratch;

/*
 *  ======== _DMAN3_initParams ========
 *  Initialize DMAN3_PARAMS with parameters configured in .tcf files.
 */
Void _DMAN3_initParams(Void)
{
    Int                         i, j;
    Int                         chanIdx = 0;
    SmUns           tcc;
    MdUns           paRam;


    DMAN3_PARAMS.qdmaPaRamBase = (Uns *)DMAN3_edma3Base + PARAMOFFSET;
    DMAN3_PARAMS.tccAllocationMaskH = DMAN3_tccMask_h;
    DMAN3_PARAMS.tccAllocationMaskL = DMAN3_tccMask_l;
    DMAN3_PARAMS.paRamBaseIndex = DMAN3_paRamBase;
    DMAN3_PARAMS.maxPaRamEntries = DMAN3_maxPaRamEntries;
    DMAN3_PARAMS.numPaRamEntries = DMAN3_numPaRamEntries;
    DMAN3_PARAMS.numQdmaChannels = DMAN3_numQdmaChannels;
    DMAN3_PARAMS.qdmaChannels = DMAN3_qdmaChannels;
    DMAN3_PARAMS.heapInternal = DMAN3_internalHeap;
    DMAN3_PARAMS.heapExternal = DMAN3_externalHeap;
    DMAN3_PARAMS.idma3Internal = (Bool)DMAN3_idma3Internal;
    DMAN3_PARAMS.scratchAllocFxn = DMAN3_allocScratch;
    DMAN3_PARAMS.scratchFreeFxn = DMAN3_freeScratch;
    DMAN3_PARAMS.nullPaRamIndex = DMAN3_nullPaRam;

    /* Unpack enabled QDMA channel mask into DMAN3_PARAMS.qdmaChannels */
    for (i = 0; i < DMAN3_maxQdmaChannels; i++) {
        if (DMAN3_qdmaChannelMask & (1 << i)) {
            DMAN3_PARAMS.qdmaChannels[chanIdx++] = i;
        }
    }

    /* Unpack the number of tccs assigned to each group */
    for (i = 0, j = 0; i < NUMPACKEDGROUPTCCS; i++) {
        tcc = (SmUns)(DMAN3_groupTccs[i] & 0x000000ff);
        DMAN3_PARAMS.numTccGroup[j++] = tcc;

        tcc = (SmUns)((DMAN3_groupTccs[i] & 0x0000ff00) >> 8);
        DMAN3_PARAMS.numTccGroup[j++] = tcc;

        tcc = (SmUns)((DMAN3_groupTccs[i] & 0x00ff0000) >> 16);
        DMAN3_PARAMS.numTccGroup[j++] = tcc;

        tcc = (SmUns)((DMAN3_groupTccs[i] & 0xff000000) >> 24);
        DMAN3_PARAMS.numTccGroup[j++] = tcc;
    }

    /* Unpack the number of PaRams assigned to each group */
    for (i = 0, j = 0; i < NUMPACKEDGROUPPARAMS; i++) {
        paRam = (MdUns)(DMAN3_groupPaRams[i] & 0x0000ffff);
        DMAN3_PARAMS.numPaRamGroup[j++] = paRam;

        paRam = (MdUns)((DMAN3_groupPaRams[i] & 0xffff0000) >> 16);
        DMAN3_PARAMS.numPaRamGroup[j++] = paRam;
    }
}
/*
 *  @(#) ti.sdo.fc.dman3; 1, 0, 1,66; 5-10-2007 10:49:43; /db/atree/library/trees/fc-b12x/src/
 */

