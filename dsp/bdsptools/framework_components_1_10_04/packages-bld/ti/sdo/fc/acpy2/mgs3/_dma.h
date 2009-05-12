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
 *  ======== mgs3/_dma.h ========
 *  This file contains object declarations used by the acpy library. 
 *
 */

#ifndef _DMA_
#define _DMA_

#include <ti/xdais/idma2.h>
#include <dma.h>
#include <acpy2_omap.h>

#define _DMA_QUEUELEN        16
#define _DMA_MAX55CHANNELS   6
#define _DMA_QUEUEEMPTY      0

typedef struct _DMA_Job {
    Uns              cnt;     /* size of transfer in params.elemSize units */
    IDMA2_AdrPtr     src;     /* Source address */
    IDMA2_AdrPtr     dst;     /* destination address */
    IDMA2_Handle     handle;  /* algorithm DMA object */ 
    struct _DMA_Job *nextJob; /* next struct in job queue (last points to first) */
} _DMA_Job;

typedef struct _DMA_QueueObj {
    IDMA2_Handle          curHandle;       /* This points to the current hdl being processed */
    volatile Int          reqPending;      /* Number of outstanding reqs to be serviced */
    Int                   len;             /* length of the queue */
    _DMA_Job             *readPtr;         /* current read pointer */
    _DMA_Job             *writePtr;        /* current write pointer */
    Int                   channelNum;      /* channel number for this queue */
    Fxn                   cbFxn;           /* callback function for reserved channel ISR */
    Arg                   cbArg;           /* callback function argument for reserved channel ISR */
} _DMA_QueueObj;

//typedef _DMA_QueueObj *_DMA_Queue;

typedef struct IDMA2_Obj {
    volatile Int numJobs;  /* Number of outstanding jobs */
    _DMA_Queue   jobQueue; /* logical channel - physical channel mapping */
    Int          channel;  /* Physical channel to use */
    Uns          csdp;     /* Channel source/dest param register value */
    Uns          ccr;      /* Channel control register value */
    Int          cfn;      /* Channel Frame number register value */
    Int          csei;     /* Channel source element index register value */
    Int          csfi;     /* Channel source frame index register value */
    Int          cdei;     /* Channel dest element index register value */
    Int          cdfi;     /* Channel dest frame index register value */
    Int          ccr2;     /* Channel endianism register value */
    ACPY2_OMAP_CallbackFxn cbFxn; /* Callback function pointer for */
                                  /* notification of completed DMA transfer */
    Arg          cbArg;    /* Argument to be passed to callback function */
    struct IDMA2_Obj  *prevHandle; /* pointer to prev handle (NULL if first in chain) */
    struct IDMA2_Obj  *nextHandle; /* pointer to next handle (NULL if last in chain) */
} IDMA2_Obj;

/*
 *  ======== DMA_cisr ========
 *  DMAN Interrupt routine. Referenced in acpy_isr.s55.
 *  This is a common isr for all the dma interrupts. The
 *  interrupt flag for each device is cleared in the acpy_isr.
 *  If there are more jobs queued, load those jobs.
 *  Parameters:
 *  chanNum : channel on which the interrupt occured
 *
 *  Returns:
 *  : Nothing. 
 */
extern Void DMA_cisr(Int chanNum);

#endif /* _DMA_ */


/*
 *  @(#) ti.sdo.fc.acpy2; 1,0,0,20; 5-10-2007 10:49:06; /db/atree/library/trees/fc-b12x/src/
 */

