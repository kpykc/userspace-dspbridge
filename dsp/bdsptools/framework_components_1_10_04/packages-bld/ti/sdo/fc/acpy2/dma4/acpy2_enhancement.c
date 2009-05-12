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
 *  ======== dma4/acpy2_enhancement ========
 *  Implementaion of ACPY_initChannel and ACPY_getChanObjSize.
 */

#include <std.h>
#ifndef _RTS_MODE
#include <dbc.h>
#else
#include <dbc_rts.h>
#endif
#include <ti/xdais/acpy2.h>
#include "_dma.h"


#pragma CODE_SECTION(ACPY2_initChannel, ".text:ACPY2_initChannel")
#pragma CODE_SECTION(ACPY2_getChanObjSize, ".text:ACPY2_getChanObjSize")
#pragma CODE_SECTION(ACPY2_setNumFrames, ".text:ACPY2_setNumFrames")
#pragma CODE_SECTION(ACPY2_setSrcFrameIndex, ".text:ACPY2_setSrcFrameIndex")
#pragma CODE_SECTION(ACPY2_setDstFrameIndex, ".text:ACPY2_setDstFrameIndex")


/*
 *  ======== ACPY2_getChanObjSize ========
 *  Get the size of the IDMA2 channel object.
 */
Uns ACPY2_getChanObjSize(Void)
{
    return (sizeof(_DMA_Obj));
}

/*
 *  ======== ACPY2_initChannel ========
 *  Initialize the IDMA2 channel object passed in.  Set the jobQueue level
 *  based on the physical channel represented by qid.
 */
Void ACPY2_initChannel(IDMA2_Handle handle, Int channelId)
{
    _DMA_Handle dmaHandle = (_DMA_Handle)handle;

    /* Assert if dmaHandle is null */
     DBC_require(dmaHandle != NULL);

    /*
     *  qid mapping to physical channel is specified in the cdb
     *  Here, we use that info to queue a logical channel's requests
     *  into a particular queue
     */
     dmaHandle->numJobs = 0x0;

     /* To do: assign configurable job queue */
     dmaHandle->jobQueue = &(_DMA_jobQueue[channelId]);
     dmaHandle->channel = channelId;
}

/*
 *  ======== ACPY2_setNumFrames ========
 *  Rapidly sets the numFrames
 */
Void ACPY2_setNumFrames(IDMA2_Handle handle, Uns value)
{
    /* Assert if handle is null */
     DBC_require(handle != NULL);

     /*
      *  Wait until all outstanding transfer submitted using this handle
      *  are completed.
      *  This is a trade-off for memory. Otherwise we have to store all
      *  config parameters within each DMA job in the s/w queue.
      */
    ACPY2_wait(handle);
    ((_DMA_Handle)handle)->cfn = value;
}

/*
 *  ======== ACPY2_setSrcFrameIndex ========
 *  Set the Source Frame Index.
 */
Void ACPY2_setSrcFrameIndex(IDMA2_Handle handle, Int value)
{
    /* Assert if handle is null */
     DBC_require(handle !=NULL);

     /*
      *  Wait until all outstanding transfer submitted using this handle
      *  are completed.
      *  This is a trade-off for memory. Otherwise we have to store all
      *  config parameters within each DMA job in the s/w queue.
      */
    ACPY2_wait(handle);
    ((_DMA_Handle)handle)->csfi = value + 1;
}

/*
 *  ======== ACPY2_setDstFrameIndex ========
 *  Set the Destination Frame Index.
 */
Void ACPY2_setDstFrameIndex(IDMA2_Handle handle, Int value)
{
    /* Assert if handle is null */
     DBC_require(handle != NULL);

     /*
      *  Wait until all outstanding transfer submitted using this handle
      *  are completed.
      *  This is a trade-off for memory. Otherwise we have to store all
      *  config parameters within each DMA job in the s/w queue.
      */
    ACPY2_wait(handle);
    ((_DMA_Handle)handle)->cdfi = value + 1;
}
/*
 *  @(#) ti.sdo.fc.acpy2; 1,0,0,20; 5-10-2007 10:49:06; /db/atree/library/trees/fc-b12x/src/
 */

