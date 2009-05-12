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
 *  ======== dma4/_dma.h ========
 *  This file contains object declarations used by the acpy library. 
 */
#ifndef _DMA_
#define _DMA_

#include <ti/xdais/idma2.h>
#include <acpy2_omap.h>
#include <dma.h>

/*
 *  ======== _DMA_Handle ========
 */
typedef struct _DMA_Obj *_DMA_Handle;

/*
 *  ======== _DMA_Job ========
 */
typedef struct _DMA_Job {
    Uns              cnt;      /* size of transfer in params.elemSize units */
    IDMA2_AdrPtr     src;      /* Source address */
    IDMA2_AdrPtr     dst;      /* destination address */
    _DMA_Handle      handle;   /* algorithm DMA object */ 
    struct _DMA_Job *nextJob;  /* next struct in job queue (last points to first) */
} _DMA_Job;

/*
 *  ======== _DMA_QueueObj ========
 */
typedef struct _DMA_QueueObj {
    _DMA_Handle     curHandle;  /* Points to the current hdl being processed */
    Int             reqPending; /* Number of outstanding reqs to be serviced */
    Int             len;        /* length of queue buffer (Number of elements) */
    _DMA_Job       *readPtr;    /* current read pointer in the job buffer */
    _DMA_Job       *writePtr;   /* current write pointer in the job buffer */
} _DMA_QueueObj;//, *_DMA_Queue;


/*
 *  ======== _DMA_Obj ========
 *  The IDMA2 channel object.
 */
typedef struct _DMA_Obj {
    volatile Int numJobs;  /* Number of outstanding jobs */
    _DMA_Queue   jobQueue; /* Software queue of logical channel to use */
    Int          channel;  /* Index of logical channel to use */
    LgUns        csdp;     /* Channel source/dest param register value */
    Uns          ccr;      /* Channel control register value */
    Int          cfn;      /* Channel Frame number register value */
    Int          csei;     /* Channel source element index register value */
    Int          csfi;     /* Channel source frame index register value */
    Int          cdei;     /* Channel dest element index register value */
    Int          cdfi;     /* Channel dest frame index register value */
    Int          ccr2;     /* Channel endianism register value */

    /* Callback function pointer for notification of completed DMA transfer */
    ACPY2_OMAP_CallbackFxn cbFxn;
    Arg          cbArg;    /* Argument to be passed to callback function */
} _DMA_Obj;

/* Array of channel numbers enabled for DMAN */
extern Int _DMA_channel[];
extern _DMA_QueueObj _DMA_jobQueue[];


#endif /* _DMA_ */
/*
 *  @(#) ti.sdo.fc.acpy2; 1,0,0,20; 5-10-2007 10:49:05; /db/atree/library/trees/fc-b12x/src/
 */

