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
 *  ======== mgs3/dman_reservephysicalchannel.c ========
 *  This file contains the exposed functions to reserve and unreserve
 *  physical DMA channels on the (C55x GDMA) dma device.
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

#include "_dman.h"
#include <ti/sdo/fc/acpy2/mgs3/acpy2_internal.h>
#include "dman_reservephysicalchannel.h"

#define SUPPORT_RESERVE 1 // should be defined in package.bld

#ifndef SUPPORT_RESERVE
#define SUPPORT_RESERVE 0
#endif

#if SUPPORT_RESERVE


#pragma CODE_SECTION(DMAN_reservePhysicalChannel, ".text:reserve")
/*
 *  ======== DMAN_reservePhysicalChannel ========
 *  Reserve a DMAN channel for exclusive non-ACPY use.
 *  Parameters: channelNum
 *                if < 0, reserve the least used channel
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
Int DMAN_reservePhysicalChannel (Int channelNum, Fxn cbFxn, Arg cbArg)
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
     return (_ACPY2_reservePhysicalChannel (channelNum, cbFxn, cbArg));
}

#pragma CODE_SECTION(DMAN_unreservePhysicalChannel, ".text:reserve")
/*
 *  ======== DMAN_unreservePhysicalChannel ========
 *  Free up a DMAN channel that had been reserved for exclusive non-ACPY use.
 *  Parameters: channelNum
 *  Return:     1: The channel was freed
 *              0: That channel is invalid or was not reserved
 */
Bool DMAN_unreservePhysicalChannel (Int channelNum)
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
     return (_ACPY2_unreservePhysicalChannel (channelNum));
}

#endif // if SUPPORT_RESERVE
/*
 *  @(#) ti.sdo.fc.dman; 1,0,0,20; 5-10-2007 10:49:28; /db/atree/library/trees/fc-b12x/src/
 */

