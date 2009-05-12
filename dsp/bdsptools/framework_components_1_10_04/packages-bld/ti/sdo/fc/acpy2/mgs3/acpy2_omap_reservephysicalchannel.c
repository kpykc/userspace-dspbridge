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
 *  ======== mgs3/acpy2_omap_reservephysicalchannel.c ========
 *  This file contains the underlying hidden functions to reserve 
 *  and unreserve physical DMA channels on the (C55x GDMA) dma device.
 *
 */

#include <std.h>
#include <ti/xdais/ialg.h>
#include <ti/xdais/idma2.h>
#include <hwi.h>

#ifndef _RTS_MODE
#include <dbc.h>
#include <mem.h>
#include <c55.h>
#else
#include <stdlib.h>
#include <dbc_rts.h>
#endif

#include <ti/sdo/fc/dman/dman.h>
#include <ti/sdo/fc/dman/mgs3/dman_reservephysicalchannel.h>
#include "_dma.h"
#include "_dmaOmaphw.h"

#define SUPPORT_RESERVE 1 // should be defined in package.bld

#ifndef SUPPORT_RESERVE
#define SUPPORT_RESERVE 0
#endif


#if SUPPORT_RESERVE

/* These globals are in dman.h55 */
extern Int DMAN_channel[];

extern IDMA2_Handle _DMA_firstHandle;
extern IDMA2_Handle _DMA_lastHandle;

extern _DMA_Queue _DMA_jobQueue[_DMA_MAX55CHANNELS];

extern Fxn _dmaIsrFxn[];
extern Int _dmaIntId[];

extern Void _ACPY2_reservedIsrStub (Void);
extern Void _ACPY2_reservedIsr0 (Void);
extern Void _ACPY2_reservedIsr1 (Void);
extern Void _ACPY2_reservedIsr2 (Void);
extern Void _ACPY2_reservedIsr3 (Void);
extern Void _ACPY2_reservedIsr4 (Void);
extern Void _ACPY2_reservedIsr5 (Void);

Fxn reservedIsr[] = { (Fxn)_ACPY2_reservedIsr0, 
                      (Fxn)_ACPY2_reservedIsr1,
                      (Fxn)_ACPY2_reservedIsr2,
                      (Fxn)_ACPY2_reservedIsr3,
                      (Fxn)_ACPY2_reservedIsr4,
                      (Fxn)_ACPY2_reservedIsr5};

#pragma CODE_SECTION(_ACPY2_reservePhysicalChannel, ".text:reserve")
/*
 *  ======== _ACPY2_reservePhysicalChannel ========
 *  Reserve a DMAN channel for exclusive non-ACPY use.
 *  Parameters: channelNum
 *                if < 0, reserve least used channel
 *                if >= 0, reserve this DMAN channel
 *              cbFxn
 *                Callback function to be called by interrupt service routine for reserved channel
 *                If NULL, then no callback function is called
 *              cbArg
 *                Argument for callback function
 *  Return:     >=0: The channel used
 *              -1:  The channel number was greater than 5
 *              -2:  The channel requested was not dedicated to ACPY
 *              -3:  The channel requested had already been reserved
 *              -4:  There was only 1 unreserved ACPY channel left
 *              -5:  There were pending jobs on the channel and interrupts were disabled
 */
Int _ACPY2_reservePhysicalChannel (Int channelNum, Fxn cbFxn, Arg cbArg)
{
    /*
     * Count the number of unreserved ACPY channels.  If less than 2,
     * return DMAN_RESERVE_NO_RESERVABLE_CHANNELS.
     * If the channelNum argument is >= 0, then check to see that it
     * is less than 6. If not return DMAN_RESERVE_CHANNEL_OUT_OF_RANGE.
     * If the channelNum argument is >= 0, also make sure that it is 
     * designated as an ACPY channel. If not then return DMAN_RESERVE_NOT_ACPY.
     * If the channelNum argument is >= 0, also make sure that it is 
     * not already reserved.  If not, then return DMAN_RESERVE_ALREADY_RESERVED.
     * Of the unreserved ACPY channels, find which queue is least used
     * (i.e. the minimum reqPending value).  
     * If the channelNum arg was -1, then choose this as the channel to
     * be reserved.  If the channelNum arg was not -1, then check if 
     * this channel with the minimum is the same as the channelNum
     * argument.  If not, choose this channel as the substitute channel
     * that the reserved channel queue will be redirected to.
     * If the channel with the minimum queue was the reserved channel,
     * or if the input argument is less than 0, then find the channel
     * with the 2nd smallest queue.  In these two cased, this will be 
     * the substitute channel that the reserved channel queue will be
     * redirected to.
     *
     * Were there jobs pending on the reserved channel?  If so, then 
     * this function needs to wait until there are no jobs running and
     * the queue is empty for the reserved channel.  ISRs will 
     * have to be temporarily re-enabled.  However, if ISRs had been 
     * disabled upon entry the error code DMAN_RESERVE_PENDING_JOBS is 
     * returned. Otherwise, ISR's are enabled.  Upon an empty queue, 
     * ISRs will be disabled again.
     *
     * The queue for the reserved channel is marked as "reserved".
     * This is done by setting its channel number to 
     * the substitute channel number (not itself).
     *
     * Walk through all the allocated handles.  For any handle
     * that points to the reserved queue, redirect it to the
     * substitute queue.
     *
     * Unplug the ACPY2 ISR entry point from the interrupt vector.
     * If cbFxn is null, plug in a stub ISR instead.
     * But if cbFxn is non-null, plug in a shell ISR which calls
     * the callback function with the cbArg argument.
     */
    IDMA2_Handle  handle;
    _DMA_Queue    queue;
    _DMA_Queue    reservedQueue;
    _DMA_Queue    substituteQueue;
    Int           channel1, channel2;
    Int           jobs1, jobs2;
    Int           substituteChan;
    Uns           state;
    Uns          *ptr = (Uns *)0x3;  /* Points to ST1 register */
    Int           i;
    
     /* Determine 2 least used unreserved channels */
    channel1 = channel2 = -1;
    jobs1 = jobs2 = 10000;
    for (i = 0; i < _DMA_MAX55CHANNELS; i++)
    {
        /* Validate that channel is an ACPY channel */
        if (DMAN_channel[i] == DMAN_NOTUSED)
            continue;
        queue = _DMA_jobQueue[i];
        /* Validate that it is not a Reserved Channel */
        if (queue->channelNum != i)
            continue;
        /* is this the least used queue? */
        if (queue->reqPending < jobs1)
        {
            /* 
             * If so, mark as the old least used as 2nd least used,
             * and mark this as least used.
             */
            channel2 = channel1;
            channel1 = i;
            jobs2 = jobs1;
            jobs1 = queue->reqPending;
        }
        else if (queue->reqPending < jobs2)
        {
            /* 
             * This is less used than the 2nd least used queue.
             * Mark this as 2nd least used queue.
             */
            channel2 = i;
            jobs2 = queue->reqPending;
        }
    }

    /* Make sure that there are at least 2 unreserved channels */
    if (channel2 == -1)
        return (DMAN_RESERVE_NO_RESERVABLE_CHANNELS);

    if (channelNum >= 0)
    {
        /* Validate that channel number is legal */
        if (channelNum >= _DMA_MAX55CHANNELS)
            return (DMAN_RESERVE_CHANNEL_OUT_OF_RANGE);
        
        /* Validate that channel is an ACPY channel */
        if (DMAN_channel[channelNum] == DMAN_NOTUSED)
            return (DMAN_RESERVE_NOT_ACPY);

        reservedQueue = _DMA_jobQueue[channelNum];

        /* Validate that it is not already a Reserved Channel */
        if (reservedQueue->channelNum != channelNum)
            return (DMAN_RESERVE_ALREADY_RESERVED);

        /* 
         * Choose the substitute channel.  If this isn't channel1, 
         * then choose channel1 as the substitute channel.
         * Otherwise choose channel2 as the substitute channel.
         */
        substituteChan = (channelNum == channel1) ? channel2 : channel1;
    }
    else
    {
        channelNum = channel1;
        substituteChan = channel2;
        reservedQueue = _DMA_jobQueue[channelNum];
    }

    substituteQueue = _DMA_jobQueue[substituteChan];

    state = *ptr & 0x800;           /* extract the interrupt bit */

    if (reservedQueue->reqPending > 0)
    {
        /*
         * If there are pending jobs on this channel but interrupts
         * were disabled on entry, then we can't temporarily
         * enable interrupts safely.  Therefore, return an error.
         */
        if (state)
            return (DMAN_RESERVE_PENDING_JOBS);
        
        /*
         * Now wait until the reserved queue is empty and there
         * is no transfer in progress.
         */
        asm("   BCLR ST1_INTM"); /* Enable interrupts */
        while (reservedQueue->reqPending > 0);
    }    

    /* Disable interrupts */
    asm("   BSET ST1_INTM");    /* Disable interrupts */
    asm("   RPT #6-2");         /* Stall until interrupts are truly disabled */
    asm("    NOP");

    /* 
     * Mark the reserved queue as "reserved".
     * Do this by setting its channel number to 
     * the substitute channel number (not itself).
     */
    reservedQueue->channelNum = substituteChan;

    /*
     * Walk through all the allocated handles.  For any handle
     * that points to the reserved queue, redirect it to the
     * substitute queue.
     */
    
    for (handle = _DMA_firstHandle; handle != NULL; handle = handle->nextHandle)
    {
        if (handle->jobQueue == reservedQueue)
        {
            handle->channel = substituteChan;
            handle->jobQueue = substituteQueue;
        }
    }

    /*
     * Walk through all of the queues.
     * If there were any reserved queues using the currently reserved
     * channel as its substitute, change it to the new substitute.
     */
    for (i = 0; i < _DMA_MAX55CHANNELS; i++)
    {
        /* Validate that channel is an ACPY channel */
        if (DMAN_channel[i] == DMAN_NOTUSED)
            continue;
        queue = _DMA_jobQueue[i];
        /* Validate that it is not a Reserved Channel */
        if (queue->channelNum == channelNum)
            queue->channelNum = substituteChan;
    }

    if (!state)
        asm("       BCLR ST1_INTM"); /* Enable interrupts */

#ifndef _RTS_MODE
    reservedQueue->cbFxn = cbFxn;
    reservedQueue->cbArg = cbArg;
    if (cbFxn == NULL)
        C55_plug(_dmaIntId[channelNum], (Fxn)_ACPY2_reservedIsrStub);
    else
        C55_plug(_dmaIntId[channelNum], reservedIsr[channelNum]);
#endif    

    return (channelNum);
}




#pragma CODE_SECTION(_ACPY2_unreservePhysicalChannel, ".text:reserve")
/*
 *  ======== _ACPY2_unreservePhysicalChannel ========
 *  Free up a DMAN channel that had been reserved for exclusive non-ACPY use.
 *  Parameters: channelNum
 *  Return:     1: The channel was freed
 *              0: That channel is invalid or was not reserved
 */
Bool _ACPY2_unreservePhysicalChannel (Int channelNum)
{
    /*
     * Check if the channel was a reserved ACPY channel.  If not,
     * return 0.
     *
     * Plug the ACPY2 ISR entry point into the interrupt vector.
     * Redirect the virtual channel array for this channel back
     * to its own physical channel.
     *
     * Return 1
     */
     _DMA_Queue queue;

     /* Validate that channel number is legal */
     if ((channelNum < 0) || (channelNum >= _DMA_MAX55CHANNELS))
         return (0);

     /* Validate that channel is an ACPY channel */
     if (DMAN_channel[channelNum] == DMAN_NOTUSED)
         return (0);

     queue = _DMA_jobQueue[channelNum];

     /* Validate that it is a Reserved Channel */
     if (queue->channelNum == channelNum)
         return (0);

#ifndef _RTS_MODE
     /* Plug the ACPY ISR back in */
     C55_plug(_dmaIntId[channelNum], (Fxn)(_dmaIsrFxn[channelNum]));
#endif

     /* Mark channel as no longer reserved */
     queue->channelNum = channelNum;

    return (1);
}

#endif // if SUPPORT_RESERVE
/*
 *  @(#) ti.sdo.fc.acpy2; 1,0,0,20; 5-10-2007 10:49:06; /db/atree/library/trees/fc-b12x/src/
 */

