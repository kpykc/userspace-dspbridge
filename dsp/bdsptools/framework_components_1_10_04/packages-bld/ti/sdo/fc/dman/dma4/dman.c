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
 *  ======== dma4/dman.c ========
 *  DMAN Module - an implementation of DMAN_ functions used by DSP/BIOS Bridge
 *  for the OMAP (C55x DMA4 device) applications to grant DMA resources to the
 *  XDAIS algorithm.
 *
 */

#include <std.h>

#ifndef _RTS_MODE
#include <sys.h>
#include <dbc.h>
#include <mem.h>
#include <sem.h>
#else
#include <dbc_rts.h>
#include <stdlib.h>
#endif

#include <hwi.h>
#include <ti/xdais/idma2.h>
#include <ti/xdais/acpy2.h>
#include <ti/sdo/fc/dman/dman.h>
#include "_dman.h"
#include <ti/sdo/fc/acpy2/acpy2_omap.h>

#define DMAN_LARGE_NUMBER 10000

#define MAXQID              31      /* Max queueId supported */
#define ILLEGAL_CHANNEL     0xffff

/* local functions */
static Bool allocChannels(IDMA2_ChannelRec dmaTab[], Int n);
static Void freeChannels(IDMA2_ChannelRec dmaTab[], Int n);
static Uns getChannelIndex(Int queueId, Bool allocChannel);
static Uns getDefaultChannel(Int queueId, Bool allocChannel);

static SEM_Obj  dmanMutex;
static Int      refCount  = 0;

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
    DBC_require(dmaFxns->implementationId == algHandle->fxns->implementationId);

    /* Make sure DMAN channels are available */
    if (DMAN_numEnabled == 0) {
        return ((Uns)DMAN_EFAIL);
    }

    numChan = dmaFxns->dmaGetChannelCnt();
    DBC_require(numChan <= DMAN_MAXDMARECS);

    numChan = dmaFxns->dmaGetChannels(algHandle, dmaTab);
    DBC_require(numChan >= 0 && numChan <= DMAN_MAXDMARECS);

    if (allocChannels(dmaTab, numChan)) {
        if (dmaFxns->dmaInit(algHandle, dmaTab) != IALG_EOK) {
            freeChannels(dmaTab, numChan);
            status = DMAN_EFAIL;
        }
    }
    else {
            /* Don't free channels twice if allocChannels() fails since its done in allocChannels */
        status = DMAN_EOUTOFMEMORY;
    }

    return ((Uns)status);
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
 *  ======== DMAN_freeLogicalDmaChannel ========
 *  Free the resources used by the logical dma channel and its handle.
 */
Void DMAN_freeLogicalDmaChannel(IDMA2_Handle  handle)
{
    Int chanObjSize     = ACPY2_getChanObjSize();
    Uns channelIdx;
    Uns     state;

    DBC_assert(handle != NULL);

    if (handle != NULL) {
        /* Get the physical channel assigned to logical channel*/
        channelIdx = ACPY2_OMAP_getPhysicalChannel(handle);

        /* Update usage stats for this physical channel */
        state = HWI_disable();
        _DMAN_channel[channelIdx].used--;
        HWI_restore(state);

        /* Free the memory allocated for the handle object */
#ifndef _RTS_MODE
        MEM_free(DMAN_heapId, handle, chanObjSize);
#else
        free(handle);
#endif
    }
}

/*
 *  ======== DMAN_getLogicalDmaChannel ========
 *  Create and initialize a logical dma channel  and return its handle.
 */
IDMA2_Handle DMAN_getLogicalDmaChannel(Int queueId)
{
    IDMA2_ChannelRec dmaTab;

    if (DMAN_numEnabled == 0) {
        return (NULL);
    }

    dmaTab.queueId = queueId;

    /*
     *  Allocate and initialize a new logical dma channel structure via a call
     *  to allocChannels. Passing NULL IDMA2_Fxns argument effectively uses
     *  the default allocation policy for physical channel assigment.
     *  The queueId argument can be assigned by the non-XDAIS node to any value
     *  and allocChannels ensures that logical DMA channels created with the
     *  same queueIds are mapped to the same physical channel.
     */
    if (allocChannels(&dmaTab, 1)) {

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
 *  ======== DMAN_init ========
 *  Initialize the DMAN module
 */
Void DMAN_init(Void)
{
    Int     i;
    LgUns       mask = DMAN_enabledMask;
    Uns     chanId = 0;

    if (refCount == 0) {
        /*
         *  This function will be called (eventually) whether or not any
         *  channels have been enabled for DMAN. Therefore, we must check
         *  if channels are enabled first.
         */
        if (DMAN_numEnabled > 0) {
            ACPY2_init();

            DBC_assert(DMAN_numEnabled > 0);

            SEM_new(&dmanMutex, 1);

            /* Fill in usage table with Ids of channels alloted to DMAN */
            for (chanId = 0, i = 0; mask != 0; chanId++) {
                if (mask & 0x1) {
                    _DMAN_channel[i].id = chanId;
                    DBC_assert(_DMAN_defaultQueueMap[i].id == ILLEGAL_CHANNEL);
                    i++;
                }
                mask = mask >> 1;
                DBC_assert(i <= DMAN_numEnabled);
            }
        }
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

    freeChannels(dmaTab, numChan);
}


/*
 *  ======== allocChannels ========
 *  Allocate and initialize logical channels (IDMA2_Obj's) requested in
 *  a dmaTab[].
 */
static Bool allocChannels(IDMA2_ChannelRec dmaTab[], Int n)
{
    Int     i;
    Int     channelIdx;
    Bool    status = TRUE;
    Int     chanSize = ACPY2_getChanObjSize();
    Uns     state;

    DBC_require(dmaTab != NULL);

    for (i = 0; i < n; i++) {
#ifndef _RTS_MODE
        dmaTab[i].handle = (IDMA2_Handle)MEM_calloc(DMAN_heapId, chanSize, 0);
#else
        dmaTab[i].handle = (IDMA2_Handle)calloc(1, chanSize);
#endif
        if (dmaTab[i].handle == NULL) {
            freeChannels(dmaTab, i);
            status = FALSE;
            break;
        }

        /* Get the index of a channel in the _DMAN_channel table */
        channelIdx = getChannelIndex(dmaTab[i].queueId, TRUE);
        DBC_assert(channelIdx < DMAN_numEnabled);

        state = HWI_disable();

        /* Update usage for this channel */
        _DMAN_channel[channelIdx].used++;

        HWI_restore(state);

        /* Initialize channel object */
        ACPY2_initChannel(dmaTab[i].handle, channelIdx);
    }

    return (status);
}

/*
 *  ======== freeChannels ========
 *  Reclaim logical channel resources (IDMA2_Obj's).
 */
static Void freeChannels(IDMA2_ChannelRec dmaTab[], Int n)
{
    Int     i;
    Int     channelIdx;
    Int     chanObjSize = ACPY2_getChanObjSize();
    Uns     state;

    DBC_require(dmaTab != NULL);

    for (i = 0; i < n; i++) {
        DBC_assert(dmaTab[i].handle != NULL);

        /* Get the index of a channel in the _DMAN_channel table */
        channelIdx = getChannelIndex(dmaTab[i].queueId, FALSE);
        DBC_assert(channelIdx < DMAN_numEnabled);

        state = HWI_disable();

        /* update the qidUsageTable by decrementing it */
        _DMAN_channel[channelIdx].used--;

        DBC_assert(_DMAN_channel[channelIdx].used >= 0);

        if (dmaTab[i].handle != NULL) {
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
 *  ======== getChannelIndex ========
 *  Given a queue ID, get the index in the channelUsage table of the
 *  corresponding logical channel. Parameter allocChannels is used to
 *  determine how to update default queue Id to channel map.
 */
static Uns getChannelIndex(Int queueId, Bool allocChannels)
{
    Uns         channelIdx;
    Uns         channel;

    queueId = (queueId > MAXQID) ? MAXQID : queueId;
    queueId = (queueId < 0) ? 0 : queueId;

    if (DMAN_enabledMask & (1 << queueId)) {
        channel = queueId;

        /* Find the index of this logical channel in the table */
        for (channelIdx = 0; channelIdx < DMAN_numEnabled; channelIdx++) {
            if (_DMAN_channel[channelIdx].id == channel) {
                /* Found it */
                break;
            }
        }
    }
    else {
        /*
         *  The channel id requested is not enabled for DMAN - Get default
         *  channel.
         */
        channelIdx = getDefaultChannel(queueId, allocChannels);
    }

    DBC_assert(channelIdx < DMAN_numEnabled);
    return (channelIdx);
}

/*
 *  ======== getDefaultChannel ========
 *  Get the default channel assigned to queueId, if it's already been assigned,
 *  or the least used logical channel.
 */
static Uns getDefaultChannel(Int queueId, Bool allocChannels)
{
    Int i;
    Int leastUsed = DMAN_LARGE_NUMBER;
    Int chanIdx = 0;

    DBC_assert((queueId <= MAXQID) && (queueId >= 0));

    /* queueId mod (DMAN_numEnabled) */
    queueId = queueId - (queueId / DMAN_numEnabled) * DMAN_numEnabled;

    SEM_pend(&dmanMutex, SYS_FOREVER);

    /*
     *  Assert that if we're not allocating channels (ie, we are freeing),
     *  then the default queue map must have a channel for this queueId.
     */
    DBC_assert(allocChannels ||
            (_DMAN_defaultQueueMap[queueId].id != ILLEGAL_CHANNEL));

    if (_DMAN_defaultQueueMap[queueId].id != ILLEGAL_CHANNEL) {
        chanIdx = _DMAN_defaultQueueMap[queueId].id;
        if (allocChannels) {
            _DMAN_defaultQueueMap[queueId].used++;
        }
        else {
            _DMAN_defaultQueueMap[queueId].used--;
            DBC_assert(_DMAN_defaultQueueMap[queueId].used >= 0);

            if (_DMAN_defaultQueueMap[queueId].used == 0) {
                _DMAN_defaultQueueMap[queueId].id = ILLEGAL_CHANNEL;
            }
        }
    }
    else {
        /* Get least used channel */
        for (i = 0; i < DMAN_numEnabled; i++) {
            if (_DMAN_channel[i].used < leastUsed) {
                leastUsed = _DMAN_channel[i].used;
                chanIdx = i;
            }
        }

        _DMAN_defaultQueueMap[queueId].id = chanIdx;
        _DMAN_defaultQueueMap[queueId].used++;
    }

    SEM_post(&dmanMutex);

    return (chanIdx);
}
/*
 *  @(#) ti.sdo.fc.dman; 1,0,0,20; 5-10-2007 10:49:27; /db/atree/library/trees/fc-b12x/src/
 */

