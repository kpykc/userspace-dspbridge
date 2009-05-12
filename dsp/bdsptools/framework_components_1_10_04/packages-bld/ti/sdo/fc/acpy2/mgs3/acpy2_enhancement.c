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
 *  ======== mgs3/acpy2_enhancement ========
 *  Implementaion of ACPY_initChannel and ACPY_getChanObjSize.
 *
 */

#include <std.h>
#ifndef _RTS_MODE
#include <dbc.h>
#else
#include <dbc_rts.h>
#endif
#include <ti/xdais/acpy2.h>
#include "_dma.h"
#include "_dmaOmaphw.h"

#pragma CODE_SECTION(ACPY2_initChannel, ".text:ACPY2_initChannel")
#pragma CODE_SECTION(ACPY2_getChanObjSize, ".text:ACPY2_getChanObjSize")
#pragma CODE_SECTION(ACPY2_setNumFrames, ".text:ACPY2_setNumFrames")
#pragma CODE_SECTION(ACPY2_setSrcFrameIndex, ".text:ACPY2_setSrcFrameIndex")
#pragma CODE_SECTION(ACPY2_setDstFrameIndex, ".text:ACPY2_setDstFrameIndex")
#pragma CODE_SECTION(ACPY2_delistHandle, ".text:ACPY2_delistHandle")

extern _DMA_Queue _DMA_jobQueue[_DMA_MAX55CHANNELS];

IDMA2_Handle _DMA_firstHandle = NULL;
IDMA2_Handle _DMA_lastHandle = NULL;

/*
 *  ======== ACPY2_initChannel ========
 *  Initialize the IDMA2 channel object passed in.  Set the jobQueue level
 *  based on the physical channel represented by qid.
 */
Void ACPY2_initChannel(IDMA2_Handle handle, Int qid)
{
    _DMA_Queue jobQueue;

    /* Assert if handle is null */
     DBC_require(handle != NULL);

    /*
     *  qid mapping to physical channel is specified in the cdb
     *  Here, we use that info to queue a logical channel's requests
     *  into a particular queue
     */
     handle->numJobs = 0x0;
         /*
          * If this qid is reserved, use the substitute queue
          */
     jobQueue = _DMA_jobQueue[qid];
         if (jobQueue->channelNum != qid)
         {
             qid = jobQueue->channelNum;
         jobQueue = _DMA_jobQueue[qid];
         }
     handle->jobQueue = jobQueue;
     handle->channel = qid;
         if (_DMA_firstHandle == NULL)
         {
             _DMA_firstHandle = handle;
                 handle->prevHandle = NULL;
         }
         else
         {
                 handle->prevHandle = _DMA_lastHandle;
             _DMA_lastHandle->nextHandle = handle;
         }
         _DMA_lastHandle = handle;

     return;

}

/*
 *  ======== ACPY2_getChanObjSize ========
 *  Get the size of the IDMA2 channel object.
 */
Uns ACPY2_getChanObjSize(Void)
{
    return (sizeof(IDMA2_Obj));
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
    handle->cfn = value;
}

/*
 *  ======== ACPY2_setSrcFrameIndex ========
 *  Set the Source Frame Index.
 */
Void ACPY2_setSrcFrameIndex(IDMA2_Handle handle, Int value)
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
    handle->csfi = value + 1;
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
    handle->cdfi = value + 1;
}


/*
 *  ======== ACPY2_delistHandle ========
 *  Remove handle from list of handles
 *  Returns TRUE is successfully delisted.
 *  Returns FALSE if it had not been listed.
 */
Bool ACPY2_delistHandle (IDMA2_Handle handle)
{
        if (handle->prevHandle != NULL)
        {
                        if (handle->prevHandle->nextHandle != handle)
                            return (FALSE);
            /* remove current handle out of middle of the list */
            handle->prevHandle->nextHandle = handle->nextHandle;
        }
        else
        {
                        if (_DMA_firstHandle != handle)
                            return (FALSE);
            /* remove current handle from top of the list */
            _DMA_firstHandle = handle->nextHandle;
        }
        if (handle->nextHandle == NULL)
        {
            if (_DMA_lastHandle != handle)
                            return (FALSE);
            /* remove current handle from bottom of the list */
            _DMA_lastHandle = handle->prevHandle;
        }
                return (TRUE);
}

/*
 *  @(#) ti.sdo.fc.acpy2; 1,0,0,20; 5-10-2007 10:49:06; /db/atree/library/trees/fc-b12x/src/
 */

