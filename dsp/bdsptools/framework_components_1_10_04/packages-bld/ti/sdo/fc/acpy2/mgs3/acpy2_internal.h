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
 *  ======== mgs3/acpy2_internal.h ========
 *  This file contains object declarations and prototypes used 
 *  internally by TI
 *
 */


#ifndef _ACPY2_INTERNAL_
#define _ACPY2_INTERNAL_

#include <ti/xdais/idma2.h>
#include <dman.h>

Bool ACPY2_delistHandle (IDMA2_Handle handle);

Uns ACPY2_OMAP_getPhysicalChannel(IDMA2_Handle handle);

/*
 *  ======== _ACPY2_reservePhysicalChannel ========
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
extern Int _ACPY2_reservePhysicalChannel (Int reservedChan, Fxn cbFxn, Arg cbArg);

/*
 *  ======== _ACPY2_unreservePhysicalChannel ========
 *  Free up a DMAN channel that had been reserved for exclusive non-ACPY use.
 *  Parameters: channelNum
 *  Return:     1: The channel was freed
 *              0: That channel is invalid or was not reserved
 */
extern Bool _ACPY2_unreservePhysicalChannel (Int channelNum);


#endif /* _ACPY2_INTERNAL_ */
/*
 *  @(#) ti.sdo.fc.acpy2; 1,0,0,20; 5-10-2007 10:49:06; /db/atree/library/trees/fc-b12x/src/
 */

