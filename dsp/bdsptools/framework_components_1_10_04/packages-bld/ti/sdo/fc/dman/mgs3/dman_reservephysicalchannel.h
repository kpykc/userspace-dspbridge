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
 *  ======== mgs3/dman_reservephysicalchannel.h ========
 */
#ifndef _DMAN_RESERVEPHYSICALCHANNEL_
#define _DMAN_RESERVEPHYSICALCHANNEL_

#include <ti/xdais/ialg.h>
#include <ti/xdais/idma2.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#define DMAN_RESERVE_CHANNEL_OUT_OF_RANGE       -1
#define DMAN_RESERVE_NOT_ACPY                   -2
#define DMAN_RESERVE_ALREADY_RESERVED           -3
#define DMAN_RESERVE_NO_RESERVABLE_CHANNELS     -4
#define DMAN_RESERVE_PENDING_JOBS               -5

/*
 *  ======== DMAN_reservePhysicalChannel ========
 *  Reserve a DMAN channel for exclusive non-ACPY use.
 *  Parameters: channelNum
 *                 if < 0, reserve the least used channel
 *                 if >= 0, reserve this DMAN channel
 *               cbFxn
 *                 Callback function to be called by interrupt service routine for reserved channel
 *                 If NULL, then no callback function is called
 *               cbArg
 *                 Argument for callback function
 *  Return:     >=0: The channel used
 *              -1:  The channel number was greater than 5
 *              -2:  The channel requested was not dedicated to ACPY
 *              -3:  The channel requested had already been reserved
 *              -4:  There was only 1 unreserved ACPY channel left
 *              -5:  There were pending jobs on the channel and interrupts were disabled
 */
extern Int DMAN_reservePhysicalChannel (Int channelNum, Fxn cbFxn, Arg cbArg);

/*
 *  ======== DMAN_unreservePhysicalChannel ========
 *  Free up a DMAN channel that had been reserved for exclusive non-ACPY use.
 *  Parameters: channelNum
 *  Return:     1: The channel was freed
 *              0: That channel is invalid or was not reserved
 */
extern Bool DMAN_unreservePhysicalChannel (Int channelNum);


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _DMAN_RESERVEPHYSICALCHANNEL_ */

/*
 *  @(#) ti.sdo.fc.dman; 1,0,0,20; 5-10-2007 10:49:28; /db/atree/library/trees/fc-b12x/src/
 */

