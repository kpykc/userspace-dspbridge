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
 *  ======== acpy3.c ========
 */

#include <std.h>
#include <dbc.h>

#include <acpy3.h>
#include <acpy3_util.h>
#include <acpy3_qdma.h>
#include <ti/xdais/idma3.h>
#include <ti/sdo/fc/dman3/dman3.h>


#define PARAMSIZE (ACPY3_PARAMENTRYSIZE * sizeof(Uns))

/*
 *  Cache PaRams must be aligned on 32 bytes.
 */
#define PARAMREGS_ALIGN 32

static Bool ACPY3_deInitEnv(IDMA3_Handle handle);
static Bool ACPY3_initEnv(IDMA3_Handle handle);
static Void ACPY3_getEnvMem(IDMA3_ChannelRec *dmaTab, IDMA3_MemRec *memRec);

#define FIELD_OFFSET(c,f) ((Int)(&((c *)0)->f))

#define verifyOffset(offset, structName, field) \
        (offset == FIELD_OFFSET(structName,field))

static Void initUsingDMAN3Configuration();
static Void initQdmaSettings(Void);
static Void _ACPY3_init();
static Void _ACPY3_exit();
static far Int refCount  = 0;

#pragma DATA_SECTION(_ACPY3_qchLockTbl, ".far:internal:ACPY3")
IDMA3_Handle  _ACPY3_qchLockTbl[_ACPY3_NUMQDMACHANNELS];

/*
** This is used to ensure that routines that were previously compiled
** with inline ACPY3 functions are compatible with the current version
** of ACPY3.
** Therefore, whenever the ACPY3 code is modified such that it is no
** longer compatible with the previous inline versions, the final
** two digits of __ACPY3_inlineCompatibilityKey_NN should be incremented.
*/
Bool ACPY3_INLINE_COMPATIBILITY_KEY_NAME;
/* 
 * Make sure the compiler setting for _ACPY3_DIRTY_BITS is the same for 
 * previously compiled inline and non-inline 
 */
Bool ACPY3_DIRTY_BITS_VERSION_NAME;

#ifndef _ACPY3_CPUCOPY_
/*
 *  ======== ACPY3_QDMA ========
 *  This static initialization defines the default parameters used to
 *  initialize the ACPY3 library with the DMAN3 configuration settings.
 */
ACPY3_QDMA_Config ACPY3_QDMA = {
    /*
     *  DMAN3_NUM_QDMA_CHANNELS: contains the number of Physical QDMA 
     *  channels that are assigned to DMAN3 via configuration.
     */
    0,
    /*
     *  DMAN3_PARAMS.qdmaChannels: array containing list of
     *  DMAN3_NUM_QDMA_CHANNELS many Physical QDMA channels assigned to
     *  DMAN3 via configuration.
     */
    NULL
};
#endif

/*
 *  ======== ACPY3_PROTOCOL ========
 *  ACPY3 channel access protocol for initializing environment.
 */
IDMA3_ProtocolObj ACPY3_PROTOCOL = {
    "acpy3",
    ACPY3_getEnvMem,
    ACPY3_initEnv,
    ACPY3_deInitEnv,
};


/*
 *  ======== _ACPY3_exit ========
 *  Finalize the ACPY3 module
 */
static Void _ACPY3_exit(Void)
{
    refCount--;

#if 0 /* uncomment when the block is populated */
    if (refCount == 0) {
        
    }
#endif    
}

/*
 *  ======== _ACPY3_init ========
 *  Initialize the ACPY3 module
 */
static void _ACPY3_init(Void)
{
    if (refCount == 0) {
        initUsingDMAN3Configuration();
        initQdmaSettings();
     }

     /* count references even if no allocation is done */
     /* so that memory free is done only when last node requires ACPY3 */
    refCount++;

    DBC_ensure(verifyOffset(ACPY3_PARAMFIELD_ELEMENTSIZE, 
                            ACPY3_PaRamRegs, acnt));
    DBC_ensure(verifyOffset(ACPY3_PARAMFIELD_NUMELEMENTS, 
                            ACPY3_PaRamRegs, bcnt));
    DBC_ensure(verifyOffset(ACPY3_PARAMFIELD_ELEMENTINDEX_SRC, 
                            ACPY3_PaRamRegs, srcElementIndex));
    DBC_ensure(verifyOffset(ACPY3_PARAMFIELD_ELEMENTINDEX_DST, 
                            ACPY3_PaRamRegs, dstElementIndex));
    DBC_ensure(verifyOffset(ACPY3_PARAMFIELD_FRAMEINDEX_SRC, 
                            ACPY3_PaRamRegs, srcFrameIndex));
    DBC_ensure(verifyOffset(ACPY3_PARAMFIELD_FRAMEINDEX_DST, 
                            ACPY3_PaRamRegs, dstFrameIndex));
    DBC_ensure(verifyOffset(ACPY3_PARAMFIELD_NUMFRAMES, 
                            ACPY3_PaRamRegs, ccnt));
    DBC_ensure(verifyOffset(ACPY3_PARAMFIELD_SRCADDR, 
                            ACPY3_PaRamRegs, src));
    DBC_ensure(verifyOffset(ACPY3_PARAMFIELD_DSTADDR, 
                            ACPY3_PaRamRegs, dst));
}

/*
 *  ======== ACPY3_initQdmaSettings ========
 */
static Void initQdmaSettings(Void)
{
#ifndef _ACPY3_CPUCOPY_
    Int     i;
    Uns     chan;
    Uns     qeesr_value;

    /*--------------------------------------------------------*/
    /* Event queue to TC channel mapping, Event queue 0 maps  */
    /* to TC channel 0. Event Queue 1 maps to TC channel 1.   */
    /*--------------------------------------------------------*/    
    *ACPY3_QUETCMAP = (0x1 << 4) | (0x0);

    /*--------------------------------------------------------*/
    /* Make QDMA lower priority than other EDMA traffic by    */
    /* setting priority of queue 1 to be 7 and that of        */
    /* queue 0 to be 0x3.                                     */
    /*--------------------------------------------------------*/
    /* Should note that '3' is an arbitrary decision          */
    /* that should be friendly  to VPSS and other critical    */
    /* DaVinci traffic.                                       */
    /*--------------------------------------------------------*/
    *ACPY3_QUEPRI   = (0x7 << 4) | (0x3);
             
    /*--------------------------------------------------------*/
    /*  QDMA region access enable registers, register0 for    */
    /*  GEM. Enable all 8 bits. Programming of this           */
    /*  register is only needed if interrupts are being       */
    /*  used, so that when there are multiple CPU's, each     */
    /*  CPU understands which interrupt, it needs to          */
    /*  respond to. It is the current thinking, that the      */
    /*  programming of this register needs to be done         */
    /*  before programming the Queue Event Enable Set         */
    /*  register.                                             */
    /*--------------------------------------------------------*/   
    *ACPY3_QRAE0    = 0xFF;

    /*--------------------------------------------------------*/
    /* QDMA to queue mapping. Map all  QDMA's to queue 1.     */
    /* Davinci has two queues. Queue0 is intended for MCBSP   */
    /* and all other transfers that do not go to DDR. This    */
    /* library will be primarily used by all algorithms       */
    /* in moving data around. As a matter of good design,     */
    /* it is better to restrict all such transfers to the     */
    /* same queue and assign them the lowest priority.        */
    /*--------------------------------------------------------*/
    for (i = 0; i < ACPY3_QDMA.numQdmaChannels; i++) {
        chan = ACPY3_QDMA.qdmaChannels[i];
        *ACPY3_QDMAQNUM = ((*ACPY3_QDMAQNUM) | (0x1 << (4 * chan)));
    }

    /*--------------------------------------------------------*/
    /* The Queue Event Enable Set register is the only way    */
    /* to enable all the four channels. To avoid the problem  */
    /* of read modify write, separate set and clear registers */
    /* are provided for enabling the QDMA channels.           */
    /*--------------------------------------------------------*/
    qeesr_value = 0x0;
    for (i = 0; i < ACPY3_QDMA.numQdmaChannels; i++) {
        qeesr_value |= (0x1 << ACPY3_QDMA.qdmaChannels[i]);
    }

    *ACPY3_QEESR = qeesr_value;

    /* unlock all qdma channels */
    for (i = 0; i < _ACPY3_NUMQDMACHANNELS; i++) {
        _ACPY3_qchLockTbl[i] = 0;
    }
#endif
}

/*
 *  ======== initUsingDMAN3Configuration ========
 */
static Void initUsingDMAN3Configuration() 
{
    DBC_require(DMAN3_PARAMS.numPaRamEntries > 0);
    DBC_require(DMAN3_PARAMS.numQdmaChannels > 0);

#ifndef _ACPY3_CPUCOPY_
    ACPY3_QDMA.numQdmaChannels = DMAN3_PARAMS.numQdmaChannels;
    ACPY3_QDMA.qdmaChannels = &DMAN3_PARAMS.qdmaChannels[0];
    ACPY3_QDMA.nullPaRamIndex = DMAN3_PARAMS.nullPaRamIndex;
#endif
}


/*
 *  ======== ACPY3_deInitEnv ========
 */
static Bool ACPY3_deInitEnv(IDMA3_Handle handle)
{
    Void   *scratchEnv;

    DBC_require(handle->protocol == &ACPY3_PROTOCOL);

    scratchEnv = ((ACPY3_MoreEnv *)(handle->env))->scratchEnv;

    if (scratchEnv != NULL) {
        scratchEnv = (Void *)((Uns)scratchEnv - handle->numPaRams * PARAMSIZE);
    }

    /*
     *  Restore original values of numTccs and numPaRams, since
     *  ACPY3_setFinal() may have changed them.
     */
    handle->numTccs = ((ACPY3_MoreEnv *)(handle->env))->numTccs;
    handle->numPaRams = ((ACPY3_MoreEnv *)(handle->env))->numPaRams;

    /* Restore env pointer */
    handle->env = (Void *)((Uns)handle->env - handle->numPaRams * PARAMSIZE);

    /* Restore scratch env pointer */
    *((LgUns *)(handle->env)) = (LgUns)scratchEnv;

    _ACPY3_exit();

    return (TRUE);
}

/*
 *  ======== ACPY3_getEnvMem ========
 */
static Void ACPY3_getEnvMem(IDMA3_ChannelRec *dmaTab, IDMA3_MemRec *memRec)
{
    /*
     *  Be aware that _ACPY3_init() may not have been called at this 
     *  point!
     */

    memRec->memType = IDMA3_INTERNAL;

    /*
     *  Size of env:
     *     numTransfers cache PaRam registers
     *     + 2 MdUns for numTransfers and numTccs
     *     + 1 MdInt for prevWaitIdOfFinal
     *     + numTransfers words for dirty bits.
     */
    /***** TO DO: Make size a multiple of 4 bytes *****/
#if _ACPY3_DIRTY_BITS
    memRec->size = (sizeof(ACPY3_MoreEnv) - sizeof(SmUns)) +
                   (dmaTab->numTransfers * (sizeof(ACPY3_PaRamRegs) + 
                                           (sizeof(SmUns))));
#else
    memRec->size = sizeof(ACPY3_MoreEnv) +
                   (dmaTab->numTransfers * (sizeof(ACPY3_PaRamRegs)));
#endif
    memRec->align = PARAMREGS_ALIGN;
}

/*
 *  ======== ACPY3_initEnv ========
 */
static Bool ACPY3_initEnv(IDMA3_Handle handle)
{
    Uns     tcc;
    Uns     maskL;
    Uns     maskH;
    Void   *scratchEnv;
    Int     i;

    DBC_require(handle->protocol == &ACPY3_PROTOCOL);

    _ACPY3_init();

    /*
     *  *(handle->env) has been initialized to a scratch buffer in internal
     *  memory, while handle->env is the persistent env.
     */
    scratchEnv = (Void *)(*(LgUns *)(handle->env));

    /*
     *  Move env to be immediately after paRam cache.
     */
    handle->env = (Void *)((Uns)handle->env + handle->numPaRams * PARAMSIZE);

    if (scratchEnv != NULL) {
        scratchEnv = (Void *)((Uns)scratchEnv + handle->numPaRams * PARAMSIZE);
    }

    ((ACPY3_MoreEnv *)(handle->env))->scratchEnv = scratchEnv;
    ((ACPY3_MoreEnv *)(handle->env))->persistEnv = handle->env;
    
    /*
     * Fill in the mask values for the final TCC.
     */
    tcc = handle->tccTable[0];
    if (tcc >= 32) {
#ifndef _ACPY3_CPUCOPY_
        ((ACPY3_MoreEnv *)(handle->env))->iprRegister = (Uns *)ACPY3_QDMA_IPRH;
#endif
        ((ACPY3_MoreEnv *)(handle->env))->iprMask = 0x1 << (tcc - 32);
    }
    else {
#ifndef _ACPY3_CPUCOPY_
        ((ACPY3_MoreEnv *)(handle->env))->iprRegister = (Uns *)ACPY3_QDMA_IPR;
#endif
        ((ACPY3_MoreEnv *)(handle->env))->iprMask = 0x1 << tcc;
    }
    
    /*
     * Fill in the mask values for clearing the whole set of TCCs.
     */
    maskH = maskL = 0;
    for (i = 0; i < handle->numTccs; i++) {
        tcc = handle->tccTable[i];
        if (tcc >= 32) 
            maskH |= 0x1 << (tcc - 32);
        else
            maskL |= 0x1 << tcc;
    }
    ((ACPY3_MoreEnv *)(handle->env))->icrlMask = maskL;
    ((ACPY3_MoreEnv *)(handle->env))->icrhMask = maskH;

    /*
     *  Save a copy of original values of numTccs and numPaRams, since
     *  ACPY3_setFinal() will need to change them.
     */
    ((ACPY3_MoreEnv *)(handle->env))->numTccs = handle->numTccs;
    ((ACPY3_MoreEnv *)(handle->env))->numPaRams = handle->numPaRams;
    ((ACPY3_MoreEnv *)(handle->env))->prevWaitIdOfFinal = -1; /* i.e. none */

    return (TRUE);
}

/*
 *  @(#) ti.sdo.fc.acpy3; 1, 0, 0,66; 5-10-2007 10:49:18; /db/atree/library/trees/fc-b12x/src/
 */

