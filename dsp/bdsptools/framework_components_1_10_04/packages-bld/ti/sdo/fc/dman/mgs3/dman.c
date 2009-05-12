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
 *  ======== mgs3/dman.c ========
 *  DMAN Module - an implementation of DMAN_ functions used by DSP/BIOS Bridge
 *  for the OMAP (C55x GDMA device) applications to grant DMA resources to the
 *  XDAIS algorithm.
 *
 */

#include <std.h>

#ifndef _RTS_MODE
#include <sys.h>
#include <dbc.h>
#include <mem.h>
#else
#include <dbc_rts.h>
#include <stdlib.h>
#endif
#include <hwi.h>
#include <ti/xdais/idma2.h>
#include <ti/xdais/acpy2.h>
#include <ti/sdo/fc/acpy2/acpy2_omap.h>
#include "_dman.h"

#define DMAN_LARGE_NUMBER 10000

Uns ACPY2_OMAP_getPhysicalChannel(IDMA2_Handle handle);
Bool ACPY2_delistHandle (IDMA2_Handle handle);

static Int qidUsageTable[DMAN_MAXPHYCHANNEL] = {DMAN_LARGE_NUMBER,};
static Int defaultTable[DMAN_MAXQID] = {ILLEGAL_CHANNEL,};
static Int defaultTableCnt[DMAN_MAXPHYCHANNEL] = {0};
static Int refCount  = 0;

/* These globals are defined in dman cdb or could come from a header file */
extern Int DMAN_heapId;
extern Int DMAN_NUMDMAN;
extern Int DMAN_channel[];
extern DMAN_Obj * DMAN_A_TABBEG;

/* local functions */
static Bool allocChannels(IDMA2_Fxns *dmaFxns,IDMA2_ChannelRec dmaTab[], Int n);
static Void freeChannels(IDMA2_Fxns *dmaFxns, IDMA2_ChannelRec dmaTab[], Int n);
static Uns getPhysicalChannel(IDMA2_Fxns *dmaFxns, Int queueId);
static Int getLeastUsedChannel();


/*
 *  ======== DMAN_addAlg ========
 *  Grant logical channel resources to an algorithm instance
 */
Uns DMAN_addAlg(IALG_Handle algHandle, IDMA2_Fxns *dmaFxns)
{
    Int numChan;
    Int status = DMAN_SOK; /* return code */
    IDMA2_ChannelRec dmaTab[DMAN_MAXDMARECS];

    DBC_require(algHandle != NULL);
    DBC_require(dmaFxns != NULL);
    DBC_require(dmaFxns->implementationId ==
        algHandle->fxns->implementationId);

    numChan = dmaFxns->dmaGetChannelCnt();
    DBC_require(numChan <= DMAN_MAXDMARECS);

    numChan = dmaFxns->dmaGetChannels(algHandle, dmaTab);
    DBC_require(numChan >= 0 && numChan <= DMAN_MAXDMARECS);

    if (allocChannels(dmaFxns, dmaTab, numChan)) {
        if (dmaFxns->dmaInit(algHandle, dmaTab) != IALG_EOK) {
            freeChannels(dmaFxns, dmaTab, numChan);
            status = DMAN_EFAIL;
        }
    }
    else {
            /*
         * Don't free channels twice if allocChannels() fails since its done
         * in allocChannels.
         */
        status = DMAN_EOUTOFMEMORY;
    }

    DBC_ensure((status == DMAN_SOK) || (status == DMAN_EOUTOFMEMORY) ||
        (status == DMAN_EFAIL));

    return (status);
}

/*
 *  ======== DMAN_getLogicalDmaChannel ========
 *  Create and initialize a logical dma channel  and return its handle.
 */
IDMA2_Handle DMAN_getLogicalDmaChannel(Int queueId)
{
    IDMA2_ChannelRec dmaTab;
    dmaTab.queueId = queueId;

    /*
     *  Allocate and initialize a new logical dma channel structure via a call
     *  to allocChannels. Passing NULL IDMA2_Fxns argument effectively uses
     *  the default allocation policy for physical channel assigment.
     *  The queueId argument can be assigned by the non-XDAIS node to any value
     *  and allocChannels ensures that logical DMA channels created with the
     *  same queueIds are mapped to the same physical channel.
     */
    if (allocChannels((IDMA2_Fxns *)NULL, &dmaTab, 1)) {

        /* now handle is ready to be used to configure the channel
         * and start using it as per SPRU352, SPRU360 & SPRA445
         */
        return (dmaTab.handle);

    }
    else {
        /*  only possible reason: DMAN_EOUTOFMEMORY */
        return (NULL);
    }
}

/*
 *  ======== DMAN_freeLogicalDmaChannel ========
 *  Free the resources used by the logical dma channel and its handle.
 */
Void DMAN_freeLogicalDmaChannel(IDMA2_Handle  handle)
{
    Int chanObjSize     = ACPY2_getChanObjSize();
    Uns physicalChannel;

    if (handle != NULL) {
        /* decrement the qidUsageTable entry for the physical channel */
        physicalChannel = ACPY2_OMAP_getPhysicalChannel(handle);
        qidUsageTable[physicalChannel]--;

        ACPY2_delistHandle (handle);

        /* Free the memory allocated for the handle object */
#ifndef _RTS_MODE
        MEM_free(DMAN_heapId, handle, chanObjSize);
#else
        free(handle);
#endif
    }
}

/*
 *  ======== DMAN_exit ========
 *  Finalization method of the DMAN module
 */
Void DMAN_exit(Void)
{
    refCount--;
    if (refCount == 0) {
        ACPY2_exit();
    }
}

/*
 *  ======== DMAN_init ========
 *  Initialize the DMAN module
 */
Void DMAN_init(Void)
{
    Int i;
    Int configured = 0;

    if (refCount == 0) {
        ACPY2_init();

        for (i = 0; i < DMAN_MAXQID; i++) {
            defaultTable[i] = ILLEGAL_CHANNEL;
        }

        for (i = 0; i < DMAN_MAXPHYCHANNEL; i++) {
            if (DMAN_channel[i] != DMAN_NOTUSED ) {
                configured++;
            }
            qidUsageTable[i] = defaultTableCnt[i] = 0;
        }

        /* DMAN is not configured for any channel. we should assert here */
        DBC_assert(configured > 0);
    }

    refCount++;
}

/*
 *  ======== DMAN_removeAlg ========
 *  Remove logical channel resources from an algorithm instance
 */
Void DMAN_removeAlg(IALG_Handle algHandle, IDMA2_Fxns *dmaFxns)
{
    IDMA2_ChannelRec dmaTab[DMAN_MAXDMARECS];
    Int numChan;

    DBC_require(algHandle != NULL);
    DBC_require(dmaFxns != NULL);
    DBC_require(dmaFxns->implementationId ==
        algHandle->fxns->implementationId);

    numChan = dmaFxns->dmaGetChannelCnt();
    DBC_require(numChan <= DMAN_MAXDMARECS);

    numChan = dmaFxns->dmaGetChannels(algHandle, dmaTab);
    DBC_require(numChan >= 0 && numChan <= DMAN_MAXDMARECS);

    freeChannels(dmaFxns, dmaTab, numChan);

    return;
}


/*
 *  ======== allocChannels ========
 *  Allocate and initialize logical channels (IDMA2_Obj's) requested in
 *  a dmaTab[].
 */
static Bool allocChannels(IDMA2_Fxns *dmaFxns, IDMA2_ChannelRec dmaTab[], Int n)
{
    Int     i;
    Int     physicalChannel;
    Bool    status = TRUE;
    Int     chanObjSize = ACPY2_getChanObjSize();
    Uns     state;

    DBC_require(dmaTab != NULL);

    for (i = 0; i < n; i++) {
#ifndef _RTS_MODE
        dmaTab[i].handle = (IDMA2_Handle)
        MEM_calloc(DMAN_heapId, chanObjSize, 0);
#else
        dmaTab[i].handle = (IDMA2_Handle)calloc(1, chanObjSize);
#endif
        if (dmaTab[i].handle == NULL) {
            freeChannels(dmaFxns, dmaTab, i);
            status = FALSE;
            break;
        }

        if (dmaTab[i].queueId >= DMAN_MAXQID) {
            dmaTab[i].queueId = DMAN_MAXQID - 1;
        }

        /* Get the correct DMAN object based on the IDMA2_handle */
        physicalChannel = getPhysicalChannel(dmaFxns, dmaTab[i].queueId);

        state = HWI_disable();

        if (physicalChannel == ILLEGAL_CHANNEL) {
            if (defaultTable[dmaTab[i].queueId] == ILLEGAL_CHANNEL) {
                /*
                 * Possibly a dynamically loaded node or no config object
                 * Apply a default policy. Get the least used channel
                 * and apply.
                 * The rationale behind applying a default policy is because
                 * in the event of system integrator not able to specify,the
                 * alg developer's inputs may not be relevant.
                 * All we have to care is same queueId goes to the same
                 * channel.
                 */
                physicalChannel = getLeastUsedChannel();

                /* Update the defaultTable */
                defaultTable[dmaTab[i].queueId] = physicalChannel;
            }
            else {
                /* Use the one from the default Table */
                physicalChannel = defaultTable[dmaTab[i].queueId];
            }

            defaultTableCnt[physicalChannel]++;
        }

        DBC_ensure(physicalChannel != ILLEGAL_CHANNEL);
        /*
         * update dmaTab[i].queueId with physical channel
         * this will be used in DMAN_removeAlg to update
         * the qidUsageTable.
         */
        dmaTab[i].queueId = physicalChannel;

        /* Update the global usage table as well */
        qidUsageTable[physicalChannel]++;

        HWI_restore(state);

        /* Initialize channel object */
        ACPY2_initChannel(dmaTab[i].handle, physicalChannel);
    }

    return (status);
}


/*
 *  ======== getPhysicalChannel ========
 *  Given the dmaFxn pointer find the DMAN Object that contains it and
 *  retrieve the queueId mapping.
 */
static Uns getPhysicalChannel(IDMA2_Fxns *dmaFxns, Int queueId)
{
    Int i = 0;
    Int phyChan = ILLEGAL_CHANNEL;
    DMAN_Obj * dmanObj = (DMAN_Obj *)DMAN_A_TABBEG;

    for (i = 0; i < DMAN_NUMDMAN; i++, dmanObj++) {
        if ((Ptr)dmaFxns == (Ptr)dmanObj->idmaFxn) {
            phyChan = dmanObj->queueId[queueId];
            /*
             * Assert if the channel is greater than MAX channel or
             * illegal channel. dmanObj->queueId can not specify
             * an illegal channel.
             */
            DBC_require(phyChan < DMAN_MAXPHYCHANNEL);
            DBC_assert(phyChan != ILLEGAL_CHANNEL);
            break;
        }
    }

    return (phyChan);
}


/*
 *  ======== getLeastUsedChannel ========
 *  This function will be called by allocChannels for those nodes or algorithm
 *  which do not have a config object. Search through the qidUsageTable and
 *  get the least used channel.
 */
static Int getLeastUsedChannel()
{
    Int i;
    Int leastUsed = DMAN_LARGE_NUMBER;
    Int chanId = ILLEGAL_CHANNEL;

    for (i = 0; i < DMAN_MAXPHYCHANNEL; i++) {

        /* consider only channels that can be used */
        if (DMAN_channel[i] != DMAN_NOTUSED) {

            if (leastUsed > qidUsageTable[i]) {
                leastUsed = qidUsageTable[i];
                chanId = i;
            }
        }
    }

    /* Assert if DMAN is not configured for any channel. */
    DBC_assert(chanId != ILLEGAL_CHANNEL);

    return(chanId);
}


/*
 *  ======== freeChannels ========
 *  Reclaim logical channel resources (IDMA2_Obj's).
 */
static Void freeChannels(IDMA2_Fxns *dmaFxns, IDMA2_ChannelRec dmaTab[], Int n)
{
    Int          i;
    Int          physicalChannel;
    Int          chanObjSize = ACPY2_getChanObjSize();
    Uns          state;

    DBC_require(dmaTab != NULL);

    for (i = 0; i < n; i++) {
        if (dmaTab[i].queueId >= DMAN_MAXQID) {
            dmaTab[i].queueId = DMAN_MAXQID - 1;
        }
        physicalChannel = getPhysicalChannel(dmaFxns, dmaTab[i].queueId);

        state = HWI_disable();

        if (physicalChannel == ILLEGAL_CHANNEL) {
            physicalChannel = defaultTable[dmaTab[i].queueId];
            defaultTableCnt[physicalChannel]--;
            if (defaultTableCnt[physicalChannel] == 0) {
                /* Reset the default Table */
                defaultTable[dmaTab[i].queueId] = ILLEGAL_CHANNEL;
            }
        }

        /* update the qidUsageTable by decrementing it */
        qidUsageTable[physicalChannel]--;
        if (dmaTab[i].handle != NULL) {
            ACPY2_delistHandle (dmaTab[i].handle);

#ifndef _RTS_MODE
            MEM_free(DMAN_heapId, dmaTab[i].handle, chanObjSize);
#else
            free(dmaTab[i].handle);
#endif
        }

        HWI_restore(state);
    }
}

/*
 *  @(#) ti.sdo.fc.dman; 1,0,0,20; 5-10-2007 10:49:28; /db/atree/library/trees/fc-b12x/src/
 */

