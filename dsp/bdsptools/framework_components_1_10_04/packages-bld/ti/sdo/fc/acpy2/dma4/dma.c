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
 *  ======== dma4/dma.c ========
 *  This file contains the queueing mechanism used for dma jobs, the
 *  associated fxns to manage the queue and the programming of the OMAP
 *  (C55x DMA4) dma device.
 */

#pragma CODE_SECTION(DMA_init, ".text:init")
#pragma CODE_SECTION(DMA_exit, ".text:exit")

#include <std.h>
#include <ti/xdais/idma2.h>
#include <hwi.h>

#ifndef _RTS_MODE
#include <dbc.h>
#include <c55.h>
#else
#include <stdlib.h>
#include <dbc_rts.h>
#endif

#include <dma.h>
#include "_dma4_omap.h"
#include "_dma.h"

/* Externs defined by dman configuration */
#include <ti/sdo/fc/dman/dma4/_dman.h>  

#include <acpy2_omap_instrumentation.h>

extern Int DMA_queueLen;

#if !defined(DMA_OPT) 
#define DMA_OPT 0      // if not defined
#endif

#define _DMA_QUEUELEN        16

extern interrupt Void DMA_isr();

DMA4_Regs *DMA4_dmaRegs = (DMA4_Regs *)0x7e6000;

/* Local fxn declarations */
static Void enableChannel(Int channel);
static Void initChannel(Int channel);
static Void initQueue(_DMA_Queue queue, Int n);


/*
 *  ======== DMA_exit ========
 *  Module finalization method.
 */
Void DMA_exit(Void)
{
}

/*
 *  ======== DMA_init ========
 *  Module initialization method
 */
Bool DMA_init(Void)
{
    Bool       succeeded = TRUE;
    Int        chanIdx;
    Int        i;
    Uns        mask;
    Uns        intNum;
    _DMA_Queue queue;

    DBC_assert((DMAN_irq == DMA4_IRQL0) || (DMAN_irq == DMA4_IRQL1) ||
            (DMAN_irq == DMA4_IRQL2) || (DMAN_irq == DMA4_IRQL3));

    /* Initialize the channels enabled for DMAN */
    chanIdx = 0;
    queue = _DMA_jobQueue;
    for (i = 0; i < DMAN_numChannels; i++) {
        if (((LgUns)1 << i) & DMAN_enabledMask) {
            _DMA_channel[chanIdx++] = i;
            initChannel(i);
            initQueue(queue, DMA_queueLen);
            queue++;
        }
    }

    /* Enable the DMAN channels */
    DMA4_dmaRegs->irqEnable[DMAN_irq] |= DMAN_enabledMask;


    /* Plug the interrupts */
    intNum = (DMAN_irq == DMA4_IRQL0) ? DMA4_IRQ0INT :
        (DMAN_irq == DMA4_IRQL1) ? DMA4_IRQ1INT :
        (DMAN_irq == DMA4_IRQL2) ? DMA4_IRQ2INT : DMA4_IRQ3INT;

#ifndef _RTS_MODE
    C55_plug(intNum, (Fxn)DMA_isr);
#endif
    /* Enable the DMAN channels */
    for (i = 0; i < DMAN_numEnabled; i++) {
        chanIdx = _DMA_channel[i];
        enableChannel(chanIdx);
    }

    /* Enable the interrupt */
    mask = DMA4_ier0Mask(intNum);
    if (mask) {
        C55_enableIER0(mask);
    }
    else {
        mask = DMA4_ier1Mask(intNum);
        DBC_assert(mask != 0);
        C55_enableIER1(mask);
    }

    return (succeeded);
}

#if (!DMA_OPT)
/*
 *  ======== DMA_isr ========
 */
interrupt Void DMA_isr()
{
    volatile LgUns  dummy;
    _DMA_Job       *job;
    _DMA_Queue      jobQ;
    DMA4_ChnlRegs  *regs;
    _DMA_Handle     handle;
    IDMA2_AdrPtr    src;
    IDMA2_AdrPtr    dst;
    Uns             cnt;
    Uns             channelNum = 0;
    Int             channelIdx = 0; /* Channel index in _DMA_channel */
    LgUns           enabledMask = DMAN_enabledMask;

    dummy = DMA4_dmaRegs->irqStatus[DMAN_irq];

    /* Make sure this is one of the DMAN channels */
    if (!(dummy & DMAN_enabledMask)) {
        /* Someone else using DMAN irq? */
        return;
    }

    /*
     *  Find out which channel caused the interrupt and get its index
     *  among channels enabled for DMAN.
     *
     *  Question:  What would happen if interrupts had been held off
     *  for awhile and several channels completed?  Wouldn't the
     *  DMA4_dmaRegs->irqStatus[DMAN_irq] register have several bits set?
     *  Wouldn't the jobQ->reqPending variable never get decremented to 0
     *  for the higher channels, and hence put ACPY2_wait() into an 
     *  infinite loop?
     */
    while (!((dummy & 0x1) && (enabledMask & 0x1))) {
        if (enabledMask & 0x1) {
            channelIdx++;
        }
        enabledMask = enabledMask >> 1;
        dummy = dummy >> 1;
        channelNum++;
    }


    regs = &(DMA4_dmaRegs->channelRegs[channelNum]);

    /* Clear the status bit by writing a 1 to it */
    regs->csr |= DMA4_CICR_BLOCKIE;
    DMA4_dmaRegs->irqStatus[DMAN_irq] |= (1 << channelNum);

    jobQ = &(_DMA_jobQueue[channelIdx]);

    /* Decrement job count for both the logical channel and IDMA2 channel */
    jobQ->curHandle->numJobs--;
    jobQ->reqPending--;

    /* If the callback function is non-null, call it. */
    if ((jobQ->curHandle)->cbFxn) {
        ((jobQ->curHandle)->cbFxn)((jobQ->curHandle)->cbArg);
    }

    /* Pull out the next job from the queue if any */
    job = (jobQ->readPtr != jobQ->writePtr) ? jobQ->readPtr : NULL;

    if (job) {
        //DMA_load(job->handle, job->src, job->dst, job->cnt);
        handle = job->handle;
        src = job->src;
        dst = job->dst;
        cnt = job->cnt;

        /* update the read pointer */
        jobQ->readPtr = job->nextJob;

        /* Update the current handle */
        jobQ->curHandle = handle;

        regs = &(DMA4_dmaRegs->channelRegs[_DMA_channel[handle->channel]]);


        /* set CSDP w/ port values */
        /*
         *  Bits 21:18 - 21 Src endianess (0 LE, 1 BE)
         *               20 Src endianess lock (0 adapt, 1 lock)
         *               19 Dst endianess (0 LE, 1 BE)
         *               18 Dst endianess lock (0 adapt, 1 lock)
         *
         *  Bits 15:14 - Dst burst enable (10 -> 32 bytes)
         *       13    - Dst pack (1 -> packed)
         *       8:7   - Src burst enable (10 -> 32 bytes)
         *       6     - Src pack (1 -> packed)
         *
         *  CSDP_SRCDSTPKBEN_ENA = 0xa140
         */
        regs->csdp = handle->csdp; // | DMA4_CSDP_SRCDSTPKBEN_ENA;

        /* set source and destination (byte) addr */
        regs->cssa = (LgUns)src;
        regs->cdsa = (LgUns)dst;

        regs->cfn = handle->cfn;
        regs->cen = cnt;
        regs->csei = handle->csei;
        regs->csfi = handle->csfi;
        regs->cdei = handle->cdei;
        regs->cdfi = handle->cdfi;

        /* Clear the old amode before seting new value */
        regs->ccr &= ~DMA4_CCR_AMODE_MASK;
        regs->ccr |= handle->ccr;

        /* Enable this dma channnel */
        regs->ccr |= DMA4_CCR_ENABLE;
        
# if _INSTR_BANDWIDTH
        ACPY2_OMAP_updateChannelStats (handle, cnt);       // Case 2
# endif // if _INSTR_BANDWIDTH
    }
# if _INSTR_BANDWIDTH
    else
        ACPY2_OMAP_updateChannelStats (NULL, channelIdx);  // Case 1
# endif // if _INSTR_BANDWIDTH
}
#endif // if !DMA_OPT


/*
 *  ======== DMA_status ========
 *  Return tthe count of outstanding requests.
 *  This has to be atomic.
 */
Int DMA_status(_DMA_Queue queue)
{
    Int retVal;
    Uns state = HWI_disable();

    DBC_require(queue != NULL);

    retVal = queue->reqPending;

    HWI_restore(state);

    return (retVal);
}

/*
 *  ======== DMA_queueEmpty ========
 *  Return true if the queue is empty
 *  Queue writePtr and readPtr are same only if the queue is empty
 *  This has to be atomic.
 */
Bool DMA_queueEmpty(_DMA_Queue queue)
{
    DBC_require(queue != NULL);
   
    return ((queue->writePtr == queue->readPtr) ? TRUE : FALSE);
}

/*
 *  ======== DMA_queueFull ========
 *  Return true if the queue is Full
 *  If queue writePtr is at the end, then return TRUE to indicate
 *  Queue is Full. It is atomic operation.
 */
Bool DMA_queueFull(_DMA_Queue queue)
{
    Bool retVal;
    Uns state = HWI_disable();

    DBC_require(queue != NULL);
    retVal = (queue->writePtr->nextJob == queue->readPtr) ? TRUE : FALSE;

    HWI_restore(state);
    return (retVal);
}

/*
 *  ======== initChannel ========
 *  Initialize the Channel
 */
static Void initChannel(Int channel)
{
    DMA4_ChnlRegs *regs;
    Bool hiPri = TRUE; /* In the future this can come from DMAN Config */

    /* Enable the IER register */
    regs = &(DMA4_dmaRegs->channelRegs[channel]);

    /* Clear channel status register */
    regs->csr = 0;

    /* frame and element index not used; By default we do post increment */
    regs->csfi = 0;
    regs->csei = 0;
    regs->cdfi = 0;
    regs->cdei = 0;

    regs->ccr &= ~DMA4_CCR_HIGHPRIO; /* clear hi pri field */

    /*
     * DST AMODE = auto post incr.
     * SRC AMODE = auto post incr.
     * repeat and autoinit = 0,
     * channel enabled: EN = 0, no sync.
     * set hi pri if hiPri == TRUE for any open call.
     */
   regs->ccr = DMA4_CCR_AMODEAUTOINC | (hiPri ? DMA4_CCR_HIGHPRIO : 0);
   return;
}

/*
 *  ======== enableChannel ========
 *  Enable the Channel
 */
static Void enableChannel(Int channel)
{
    DMA4_ChnlRegs *regs;

    /* Enable the IER register */
    regs = &(DMA4_dmaRegs->channelRegs[channel]);

    /* Turn on BLOCK IE to signal block completion, and enable channel. */
    regs->cicr = DMA4_CICR_BLOCKIE;

    return;
}


/*
 *  ======== initQueue ========
 *  Initialize the queue.
 */
static Void initQueue(_DMA_Queue queue, Int n)
{
    Int       i;
    _DMA_Job *buffer;
    _DMA_Job *nextJob;

    if (!queue)  // Should never happen!
        return;
        
    /*
     * The only expectation for the queue on entry to this routine
     * is that if the queue doesn't exist it, the entry for it in
     * _DMA_jobQueue[] array of pointers is null, and otherwise points
     * to a job queue object.  The job queue object need only have
     * the writePtr element initialized to the first job in the buffer
     * of _DMA_QUEUELEN jobs.
     */

    queue->curHandle = 0;   // this should have already been done
    queue->reqPending = 0;  // this should have already been done
    buffer = queue->writePtr;
    
    if (!buffer)  // Should never happen! 
        return;

    queue->readPtr = buffer;
    /* Set up the circular link-list */
    nextJob = buffer;
    for (i = n; i > 0; i--) {
        buffer->dst = NULL;   // this is no longer necessary (but harmless)
        nextJob++;
        buffer->nextJob = nextJob;
        buffer = nextJob;
    }
    /* 
     * Point the last job back to the first.
     * This will make it a circular linked list.
     */
    nextJob--;
    nextJob->nextJob = queue->readPtr;
}

/*
 *  @(#) ti.sdo.fc.acpy2; 1,0,0,20; 5-10-2007 10:49:06; /db/atree/library/trees/fc-b12x/src/
 */

