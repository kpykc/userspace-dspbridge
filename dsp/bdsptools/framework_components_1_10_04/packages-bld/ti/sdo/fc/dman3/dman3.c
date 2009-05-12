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
 *  ======== dman3.c ========
 */

#include <std.h>

#include <dman3.h>

/*
 *  ======== DMAN3 ========
 *  This static initialization defines the default parameters used to
 *  initialize the DMAN3 module.
 */


/*
 * DMAN3_QDMA_PARAM_BASE: Default Physical base address for PARAM0 in the 
 * EDMA3/QDMA hardware whose resources are being managed by DMAN3. 
 *
 * DMAN3_MAX_PARAM_ENTRIES: Default total numbar of PaRam entries on the
 * hardware (= 128 for IVA2).
 *
 * DMAN3_PARAM_BASE_INDEX: (0>value>255) represents the first PARAM TABLE ENTRY
 * NUMBER that is assigned by configuration for exclusive DMAN3 allocation.
 *
 * The DMAN3_NUM_CONTIGUOUS_PARAM_ENTRIES many PARAM Table entries starting at
 * DMAN3_PARAM_BASE_INDEX are assigned by configuration for exclusive DMAN3 
 * allocation.
 */
#define DMAN3_QDMA_PARAM_BASE  (Uns *)((Uns)0x01C00000 + 0x4000)
#define DMAN3_MAX_PARAM_ENTRIES 128
#define DMAN3_PARAM_BASE_INDEX 96
#define DMAN3_NUM_CONTIGUOUS_PARAM_ENTRIES  32

/* 
 * DMAN3_MAX_QDMA_CHANNELS: contains the total number of Physical QDMA channels
 * available on the hardware (eg, 4 for IVA2).
 *
 * DMAN3_NUM_QDMA_CHANNELS: contains the number of Physical QDMA channels
 * that are assigned to DMAN3 via configuration.
 *
 * DMAN3_QDMA_CHANNELS: array containing list of DMAN3_NUM_QDMA_CHANNELS
 * many Physical QDMA channels assigned to DMAN3 via configuration.
 */
#define DMAN3_MAX_QDMA_CHANNELS   4 
#define DMAN3_NUM_QDMA_CHANNELS   4 
far Uns DMAN3_QDMA_CHANNELS[] = {0, 1, 2, 3};

/* 
 * DMAN3_TCC_FREEMASK_H and DMAN3_TCC_FREEMASK_L:  are 32-bit bitmasks 
 * representing configuration provided list of TCCs for exclusive DMAN3 
 * allocation.
 * 
 * For example, for TCC's in the range 0-31 the Low Mask (DMAN3_TCC_FREEMASK_L)
 * is configured so that a '1' in bit position 'i' indicates the TCC 'i' is 
 * assigned to DMAN3
 */
#define DMAN3_TCC_FREEMASK_H  0xFFFFAFF7 
#define DMAN3_TCC_FREEMASK_L  0x05 

 /*
  * DMAN3_HEAP_INTERNAL: BIOS Heap ID for dynamic allocation of DMAN3 objects 
  * must be allocated in L1D Internal RAM.
  * DMAN3_HEAP_EXTERNAL: BIOS Heap ID for dyn allocation of private DMAN3
  * data structures. 
  * A value of -1 indicates that the heap is not defined.
  */
#define DMAN3_HEAP_INTERNAL  -1 
#define DMAN3_HEAP_EXTERNAL  0 

/*
 *  DMAN3_IDMA3INTERNAL: Use the internal DMAN3 memory heap to allocate IDMA3
 *  objects.
 */
#define DMAN3_IDMA3INTERNAL  1

#define DMAN3_SCRATCHALLOCFXN NULL
#define DMAN3_SCRATCHFREEFXN  NULL

#define DMAN3_NULLPARAM 0

far Int DMAN3_heapInternal = DMAN3_HEAP_INTERNAL;
far Int DMAN3_heapExternal = DMAN3_HEAP_EXTERNAL;

/*
 *  ======== DMAN3_Config ========
 *  Default module configuration structure for DMAN3 implementation.
 *  It is set at design time by the system integrator to adjust the behavior
 *  of the module to be optimal for its execution environment.
 */
DMAN3_Params DMAN3_PARAMS = {

    /* qdmaPaRamBase: Physical base address for PARAM0 */
    DMAN3_QDMA_PARAM_BASE,

    /* maxPaRamEntries: Total number of PaRam entries */
    DMAN3_MAX_PARAM_ENTRIES,

    /*
     * paRamBaseIndex: (0>value>255) represents the first PARAM TABLE ENTRY
     * NUMBER that is assigned by configuration for exclusive DMAN3 allocation.
     */
    DMAN3_PARAM_BASE_INDEX,
 
    /*
     * numPaRamEntries:  number of PARAM Table entries starting at
     * DMAN3_PARAM_BASE_INDEX assigned by configuration for exclusive DMAN3 
     * allocation.
     */
    DMAN3_NUM_CONTIGUOUS_PARAM_ENTRIES,

    /* 
     * maxQdmaChannels: total number of Physical QDMA channels on the hardware.
     *
     * numQdmaChannels: contains the number of Physical QDMA channels
     * that are assigned to DMAN3 via configuration.
     * qdmaChannels: array containing list of DMAN3_NUM_QDMA_CHANNELS
     * many Physical QDMA channels assigned to DMAN3 via configuration.
     */
    DMAN3_MAX_QDMA_CHANNELS,
    DMAN3_NUM_QDMA_CHANNELS,
    DMAN3_QDMA_CHANNELS,

    /* 
     * tccAllocationMaskH and tccAllocationMaskL:  are 32-bit bitmasks 
     * representing configuration provided list of TCCs for exclusive DMAN3 
     * allocation.
     * 
     * For example, for TCC's in the range 0-31 the Low Mask (tccAllocationMaskL)
     * is configured so that a '1' in bit position 'i' indicates the TCC 'i' is 
     * assigned to DMAN3
     */
    DMAN3_TCC_FREEMASK_H, 
    DMAN3_TCC_FREEMASK_L,

     /*
      * heapInternal: BIOS Heap ID for dynamic allocation of DMAN3 objects 
      * must be allocated in L1D Internal RAM.
      * heapExternal: BIOS Heap ID for dyn allocation of private DMAN3
      * data structures. 
      *
      * A value of -1 indicates that the heap is NOT DEFINED.
      */
    DMAN3_HEAP_INTERNAL,
    DMAN3_HEAP_EXTERNAL,

    /* numTccGroup[DMAN3_MAXGROUPS] */
    {4,  4, 4, 4, 4,
     0,  0, 0, 0, 0,
     0,  0, 0, 0, 0,
     0,  0, 0, 0, 0
    },

    /* numPaRamGroup[DMAN3_MAXGROUPS] */
    {4,  4, 4, 4, 4,
     0,  0, 0, 0, 0,
     0,  0, 0, 0, 0,
     0,  0, 0, 0, 0
    },

    /* idma3Internal: Are IDMA3 objects allocated in internal memory? */
    DMAN3_IDMA3INTERNAL,

    /* scratchAllocFxn*/
    DMAN3_SCRATCHALLOCFXN,

    /* scratchFreeFxn*/
    DMAN3_SCRATCHFREEFXN,

    /* nullPaRamIndex */
    DMAN3_NULLPARAM,
};

/*
 *  @(#) ti.sdo.fc.dman3; 1, 0, 1,66; 5-10-2007 10:49:43; /db/atree/library/trees/fc-b12x/src/
 */

