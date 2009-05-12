/*
 *  Copyright 2007 by Texas Instruments Incorporated.
 *
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */

/*
 *  ======== dman3_init.c ========
 *  Initialize the DMAN module
 */

#pragma CODE_SECTION(_DMAN3_initImpl, ".text:DMAN3_init");

#include <std.h>

#include <dbc.h>
#include <mem.h>
#include <sys.h>

#include <rmm.h>
#include <dman3.h>
#include <dman3_impl.h>

/*
 *  QCHMAP: Queue to channel map register which specifies which parameter
 *  entry in bits 13-5 and the trigger word (which is word 7 in ACPY3
 *  implementation), in bits 4-2.
 */
//#define QCHMAP ((volatile Uns *)(0x01c00200))

// TO DO: These 2 defines will go away with EDMA3 regs structures
#define QCHMAPOFFSET 0x200
#define PARAMOFFSET 0x4000
#define PARAMENTRYSIZE 8  /* Number of words in PaRam entry */

static Int _DMAN3_refCount = 0;


far SmUns _DMAN3_tccAllocationTable[_DMAN3_MAXNUMTCCS];

far Int   _DMAN3_numAssignedTccs;
far Int   _DMAN3_heapInt;
far Int   _DMAN3_heapExt;
far Int   _DMAN3_heapIdma3;

far SmUns         _DMAN3_numTccGroup[DMAN3_MAXGROUPS];
far MdUns         _DMAN3_numPaRamGroup[DMAN3_MAXGROUPS];
far DMAN3_Context _DMAN3_groupContext[DMAN3_MAXGROUPS];

far RMM_Handle _DMAN3_rmmTccs = NULL;
far RMM_Handle _DMAN3_rmmPaRams = NULL;

far DMAN3_ScratchAllocFxn _DMAN3_allocScratch = NULL;
far DMAN3_ScratchFreeFxn _DMAN3_freeScratch = NULL;

static Void updateTccAllocationTable(LgUns mask32b, Int base);


/*
 *  ======== DMAN3_init ========
 *  Initialize the DMAN module
 */
Void _DMAN3_initImpl(Void)
{
    RMM_Attrs       rmmAttrs;
#ifndef _DMAN3_CPUCOPY_
    volatile Uns   *qchmap;
    Uns            *paRam;
#endif
    Int             i;

    if (_DMAN3_refCount == 0) {
        /*
         * Initialize the tccAllocationTable with the provided TCCs.
         * Use the TCCs assigned in the High range (32-63) first, then
         * Low (0-31) when initializing the _DMAN3_tccAllocationTable.
         * Update the total number of assigned TCC count
         * (_DMAN3_numAssignedTccs)
         */
        _DMAN3_numAssignedTccs = 0;
        updateTccAllocationTable(DMAN3_PARAMS.tccAllocationMaskH, 32);
        updateTccAllocationTable(DMAN3_PARAMS.tccAllocationMaskL, 0);

        /*
         *  Initialize _DMAN3_heapInt and _DMAN3_heapExt with provided heaps.
         *  Use internal heap for _DMAN3_heapInt when provided. (Actually this
         *  is a fatal error if ACPY3 uses internal DMA (IDMA) to write to
         *  PARAMs.
         */
        _DMAN3_heapInt = DMAN3_PARAMS.heapInternal;
        _DMAN3_heapExt = DMAN3_PARAMS.heapExternal;

        _DMAN3_heapInt = (_DMAN3_heapInt == -1) ?
            _DMAN3_heapExt : _DMAN3_heapInt;
        _DMAN3_heapExt = (_DMAN3_heapExt == -1) ?
            _DMAN3_heapInt : _DMAN3_heapExt;
        _DMAN3_heapIdma3 = (DMAN3_PARAMS.idma3Internal) ?
            _DMAN3_heapInt : _DMAN3_heapExt;

        /* This is a fatal configuration error. */
        DBC_assert(_DMAN3_heapInt != -1);

        for (i = 0; i < DMAN3_MAXGROUPS; i++) {
            _DMAN3_numTccGroup[i] = DMAN3_PARAMS.numTccGroup[i];
            _DMAN3_numPaRamGroup[i] = DMAN3_PARAMS.numPaRamGroup[i];
        }

        /*
         *  Configured function for allocating IDMA3 object's env from
         *  shared scratch memory.
         */
        _DMAN3_allocScratch = DMAN3_PARAMS.scratchAllocFxn;

        /*
         *  Configured function for allocating IDMA3 object's env from
         *  shared scratch memory.
         */
        _DMAN3_freeScratch = DMAN3_PARAMS.scratchFreeFxn;

        /*
         *  If one of _DMAN3_allocScratch or _DMAN3_freeScratch is NULL,
         *  then the other must be NULL.
         */
        DBC_require((_DMAN3_allocScratch != NULL) ||
                    (_DMAN3_freeScratch == NULL));
        DBC_require((_DMAN3_freeScratch != NULL) ||
                    (_DMAN3_allocScratch == NULL));

        /*
         *  Attributes of TCC array that will be managed by RMM for
         *  allocation:
         *    segid - Use the external memory for the RMM object.
         *    base = 0 - We allocate indices of the array, starting with 0
         *    length = _DMAN3_numAssignedTccs - the number of TCCs in the array
         *    maxFreeListBlocks - Set to maximum number of free (unallocated)
         *        blocks that the array can fragment into. This happens when
         *        every other index is free.
         */
        rmmAttrs.segid = _DMAN3_heapExt;
        rmmAttrs.base = 0;
        rmmAttrs.length = _DMAN3_numAssignedTccs;
        rmmAttrs.maxFreeListBlocks = (_DMAN3_numAssignedTccs + 1) / 2;
        _DMAN3_rmmTccs = RMM_create(&rmmAttrs);

        /* Can't use SYS_abort() because of dependancy on BIOS */
        DBC_assert(_DMAN3_rmmTccs != MEM_ILLEGAL);

        /*
         *  Attributes of PaRam entry numbers that will be managed by RMM for
         *  allocation:
         *    segid - Use the external memory for the RMM object.
         *    base - The first PaRam index for DMAN3.
         *    length - Number of PaRam entries for DMAN3.
         *    maxFreeListBlocks - Set to maximum number of free (unallocated)
         *        blocks that can PaRam indices can fragment into. This
         *        happens when every other index is free.
         */
        rmmAttrs.base = DMAN3_PARAMS.paRamBaseIndex;
        rmmAttrs.length = DMAN3_PARAMS.numPaRamEntries;
        rmmAttrs.maxFreeListBlocks = (DMAN3_PARAMS.numPaRamEntries + 1) / 2;
        _DMAN3_rmmPaRams = RMM_create(&rmmAttrs);

        /* Can't use SYS_abort() because of dependancy on BIOS */
        DBC_assert(_DMAN3_rmmPaRams != MEM_ILLEGAL);

        /*
         *  For each of the QDMA channels used by DMAN3, set the STATIC
         *  bit of the currently mapped paRam. This is needed so that
         *  any IDMA3 protocol can check that the static bit of the mapped
         *  paRam is set, before changing the QDMA channel map register
         *  (if the STATIC bit is not set, a linked transfer may be in
         *  progress, and the QDMA channel map register should not be
         *  written to in that case).
         *  By setting the STATIC bit in DMAN3_init(), we are guarranteed
         *  that no protocol has yet mapped the QDMA channel, and so we
         *  will not mess up anyone's paRam.
         */
#ifndef _DMAN3_CPUCOPY_
        /* Set qchmap to address of first QCHMAP register */
        qchmap = (volatile Uns *)((Uns)DMAN3_PARAMS.qdmaPaRamBase -
                PARAMOFFSET + QCHMAPOFFSET);

        /*
         *  Set the trigger word of each QDMA channel to 7, so that when we
         *  set the static bit (of opts field of PaRam which is word 0) of the
         *  null PaRam, we don't start a transfer, in case the QCHMAP register
         *  happened to have the null PaRam with trigger word 0.
         */
        for (i = 0; i < DMAN3_PARAMS.maxQdmaChannels; i++) {
            *qchmap = *qchmap | (0x7 << 2);
            qchmap++;
        }

        /*
         *  Set the static bit of opts field of the null PaRam, since
         *  QCHMAP may get set to this PaRam when the channel is
         *  activated/deactivated. (The first word of the PaRam entry is
         *  the "opts" field).
         */
        paRam = (Uns *)((Uns)DMAN3_PARAMS.qdmaPaRamBase) +
            DMAN3_PARAMS.nullPaRamIndex * PARAMENTRYSIZE;
        *paRam |= 0x8;
#endif
        for (i = 0; i < DMAN3_MAXGROUPS; i++) {
            _DMAN3_groupContext[i] = NULL;
        }

        /*
         * Allocate a table for tracking which words of PaRAM cache
         * have been updated since the last time it was synchronized
         * with the physical PaRAM registers.
         */
    }

    _DMAN3_refCount++;
}

/*
 *  ======== DMAN3_exit ========
 *  Finalization method of the DMAN3 module
 */
Void DMAN3_exit(Void)
{
    _DMAN3_refCount--;

    if (_DMAN3_refCount == 0) {
        /* Free up everything */
        if (_DMAN3_rmmTccs) {
            RMM_delete(_DMAN3_rmmTccs);
            _DMAN3_rmmTccs = NULL;
        }

        if (_DMAN3_rmmPaRams) {
            RMM_delete(_DMAN3_rmmPaRams);
            _DMAN3_rmmPaRams = NULL;
        }
    }
}

/*
 *  ======== updateTccAllocationTable ========
 */
static Void updateTccAllocationTable(LgUns mask32b, Int base)
{
    Int i;

    for (i = 0; i < 32; i++) {

        if (mask32b & (0x1 << i)) {
            _DMAN3_tccAllocationTable[_DMAN3_numAssignedTccs++] = base + i;
        }
    }
}

/*
 *  @(#) ti.sdo.fc.dman3; 1, 0, 1,66; 5-10-2007 10:49:43; /db/atree/library/trees/fc-b12x/src/
 */

