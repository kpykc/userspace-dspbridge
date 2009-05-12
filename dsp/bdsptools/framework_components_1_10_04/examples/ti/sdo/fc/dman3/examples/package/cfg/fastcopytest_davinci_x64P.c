/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 */
#define __nested__

#define __config__

#ifndef xdc_std__include
#ifndef __nested__
#define __nested__
#include <xdc/std.h>
#undef __nested__
#else
#include <xdc/std.h>
#endif
#endif


/*
 * ======== instance types ========
 */

/*
 * ======== declarations ========
 */

/*
 * ======== module templates ========
 */

/* module xdc.cfg.Program */

/*
 *  ======== __PLAT__ ========
 *  Define the name of the platform that can run this executable.  This
 *  symbol is used by platform independent "exec" commands
 */
char* __PLAT__ = "@(#)__PLAT__ = ti.platforms.sim6xxx";

/*
 *  ======== __ISA__ ========
 *  Define the ISA of this executable.  This symbol is used by platform
 *  specific "exec" commands that support more than one ISA; e.g., gdb
 */
char* __ISA__ = "@(#)__ISA__ = 64P";


/* module ti.sdo.fc.dman3.DMAN3 */


/* C code contribution from ti/sdo/fc/dman3/DMAN3.xdt */

#include <xdc/std.h>
#include <ti/bios/include/std.h>
#ifndef IDMA3_USEFULLPACKAGEPATH
#define IDMA3_USEFULLPACKAGEPATH
#endif
#include <ti/sdo/fc/dman3/dman3.h>
#include <ti/sdo/fc/acpy3/acpy3.h>

#if 20 > DMAN3_MAXGROUPS
#error too many values in "numTccGroup" array, please fix your .cfg
#endif

#if 20 > DMAN3_MAXGROUPS
#error too many values in numPaRamGroup array, please fix your .cfg
#endif

far Uns DMAN3_QDMA_CHANNELS[8] = {
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7
};

extern Void  _DMAN3_initInternal();
extern Void  _DMAN3_initImpl(Void);

/*
 *  ======== DMAN3_init  ========
 */
Void DMAN3_init(Void)
{
    extern far DMAN3_Params DMAN3_PARAMS;

    /* BIOS heap labels */
    extern far Int L1DHEAP;
    extern far Int EXTERNALHEAP;

    DMAN3_PARAMS.heapInternal = L1DHEAP;
    DMAN3_PARAMS.heapExternal = EXTERNALHEAP;

    _DMAN3_initImpl();
}


/*
 *  ======== DMAN3_CE_init  ========
 *  Supplied to support legacy Codec Engine Frameworks. To be deprecated in the future.
 */
Void DMAN3_CE_init()
{
    DMAN3_init();
    ACPY3_init();
}

/*
 *  ======== DMAN3_CE_exit  ========
 *  Supplied to support legacy Codec Engine Frameworks. To be deprecated in the future.
 */
Void DMAN3_CE_exit()
{
    DMAN3_exit();
    ACPY3_exit();
}

/*
 *  ======== DMAN3_PARAMS ========
 *  Default module configuration structure for DMAN3 implementation.
 *  It is set at design time by the system integrator to adjust the behaviour
 *  of the module to be optimal for its execution environment.
 */
DMAN3_Params DMAN3_PARAMS = {

    /* qdmaPaRamBase: Physical base address for PARAM0 */
    (Uns *)0x1c04000,

    /* The maxPaRamEntries:  Total number of PARAM Table entries on the
     * hardware (eg, for IVA2 this is 128, for Himalaya, this is 256).
     */
    128,

    /*
     * paRamBaseIndex: (0>value>255) represents the first PARAM TABLE ENTRY
     * NUMBER that is assigned by configuration for exclusive DMAN3 allocation.
     */
    78,

    /*
     * numPaRamEntries:  number of PARAM Table entries starting at
     * DMAN3_PARAM_BASE_INDEX assigned by configuration for exclusive DMAN3
     * allocation.
     */
    48,

    /* maxQdmaChannels:  contains the total number of Physical QDMA channels
     * available on the hardware (eg, 8 for DaVinci, 4 for IVA2).
     */
    8,

    /*
     * numQdmaChannels: contains the number of Physical QDMA channels
     * that are assigned to DMAN3 via configuration.
     * qdmaChannels: array containing list of DMAN3_NUM_QDMA_CHANNELS
     * many Physical QDMA channels assigned to DMAN3 via configuration.
     */
    8,
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
    0xffffffff,
    0x0,

     /*
      * heapInternal: BIOS Heap ID for dynamic allocation of DMAN3 objects
      * must be allocated in L1D Internal RAM.
      * heapExternal: BIOS Heap ID for dyn allocation of private DMAN3
      * data structures.
      *
      * A value of -1 indicates that the heap is NOT DEFINED.
      */
    -1, //L1DHEAP,
    0,  //EXTERNALHEAP,

    /* numTccGroup[DMAN3_MAXGROUPS] */
    {
    4,
    3,
    2,
    0,
    0,
    5,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
    },

    /* numPaRamGroup[DMAN3_MAXGROUPS] */
    {
    4,
    4,
    2,
    0,
    0,
    5,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
    },

    /* idma3Internal */
    1,

    /* scratchAllocFxn */
    NULL,

    /* scratchFreeFxn */
    NULL,

    /* nullPaRamIndex */
    127,
};


/*
 * ======== module configuration ========
 */

/*
 * ======== module postInit ========
 */

/*
 * ======== program globals ========
 */

/*
 * ======== package stubs ========
 */
#include "xdc/package/package.c"
#include "xdc/rts/package/package.c"
#include "xdc/platform/package/package.c"
#include "xdc/cfg/package/package.c"
#include "xdc/shelf/package/package.c"
#include "xdc/spec/package/package.c"
#include "xdc/spec/jserver/package/package.c"
#include "xdc/utils/package/package.c"
#include "ti/catalog/c6000/package/package.c"
#include "ti/platforms/sim6xxx/package/package.c"
#include "ti/targets/rts6000/package/package.c"
#include "ti/psl/package/package.c"
#include "ti/rtdx/package/package.c"
#include "xdc/bld/package/package.c"
#include "ti/bios/package/package.c"
#include "ti/sdo/fc/dman3/package/package.c"
#include "ti/sdo/fc/acpy3/package/package.c"
#include "ti/sdo/fc/utils/package/package.c"
#include "ti/sdo/fc/utils/api/package/package.c"
#include "ti/sdo/fc/dman3/examples/package/package.c"
