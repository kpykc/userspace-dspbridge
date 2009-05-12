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
 *  ======== dma4/acpy2_start.c ========
 */

#include <std.h>
#include <ti/xdais/acpy2.h>
#include <ti/xdais/ialg.h>
#include <ti/xdais/idma2.h>

#ifndef _RTS_MODE
#include <dbc.h>
#include <tsk.h>
#include <mem.h>
#include <c55.h>
#else
#include <stdlib.h>
#include <dbc_rts.h>
#endif
#include <hwi.h>

#include <dma.h>
#include "_dma4_omap.h"
#include "_dma.h"

#include <acpy2_omap_instrumentation.h>

#ifndef DMA_OPT
#define DMA_OPT 0
#endif

#if (!DMA_OPT)

/* DMA4 control registers */
static DMA4_Regs *dmaRegs = (DMA4_Regs *)0x7e6000;

#pragma CODE_SECTION(ACPY2_start, ".text:ACPY2_start")

/*
 *  ======== ACPY2_start ========
 *  Start a DMA transfer; If the jobQueue is  not empty place the job
 *  jobQueue.If the queue is empty load the device & start the transfer.
 */
Void ACPY2_start(IDMA2_Handle handle, IDMA2_AdrPtr src, IDMA2_AdrPtr dst,
        Uns cnt)
{
    Uns             state;
    Uns             pending;
    _DMA_Queue      queue;
    DMA4_ChnlRegs  *regs;
    Uns            *ptr = (Uns *)0x3;  /* Points to ST1 register */
    _DMA_Handle     dmaHandle = (_DMA_Handle)handle;

#ifdef MAKE_HWI_CALL
    state = HWI_disable();
#else
#ifdef _55_
    state = *ptr & 0x800;           /* extract the interrupt bit */
    asm("       BSET ST1_INTM");    /* Disable interrupts */
    asm("       nop");
    asm("       nop");
    asm("       nop");
    asm("       nop");
    asm("       nop");
#endif
#endif
    queue = dmaHandle->jobQueue;
    pending = queue->reqPending;

    /* Increment job count on this logical channel. */
    dmaHandle->numJobs++;

    /*
     *  Increment the count of pending transfers issued & not-completed
     *  on this physical channel.
     */
    queue->reqPending++;

    if (pending == 0) {

        /* From the job find out the channel to use */


        /* Update the current handle */
        queue->curHandle = dmaHandle;

        regs = &(dmaRegs->channelRegs[_DMA_channel[dmaHandle->channel]]);

        /* set CSDP w/ port values */
        regs->csdp = dmaHandle->csdp; // | CSDP_SRCDSTPKBEN_ENA;

        /*
         *  Make sure the Start_Address for src and dst is aligned on the
         *  boundary of data type (elemSize) moved
         */

        /* Set up Src packing and bursting here  Or set up that during init */

        /* set source (byte) addr lower and upper */
        regs->cssa = (LgUns)src;

        /* set destination (byte) addr lower and upper */
        regs->cdsa = (LgUns)dst;

        regs->cfn = dmaHandle->cfn;
        regs->cen = cnt;
        regs->csei = dmaHandle->csei;
        regs->csfi = dmaHandle->csfi;
        regs->cdei = dmaHandle->cdei;
        regs->cdfi = dmaHandle->cdfi;

        /* Clear the old amode before seting new value */
        regs->ccr &= ~DMA4_CCR_AMODE_MASK;
        regs->ccr |= dmaHandle->ccr;

        /* enable this dma chan */
        regs->ccr |= DMA4_CCR_ENABLE;
#if _INSTR_BANDWIDTH
        ACPY2_OMAP_updateChannelStats (dmaHandle, cnt); // Case 3
#endif // if _INSTR_BANDWIDTH
    }
    else {

        /*
         * If queue->reqPending >= queue->len, then turn ISRs back on and wait
         * until queue->reqPending is less than queue->len before placing
         * this new request on the queue.  Note that the old value of 
         * queue->reqPending is in T1.
         */
        if (queue->reqPending > queue->len)
                {
                    Int len = queue->len;
#ifdef _55_
            asm("  BCLR ST1_INTM");          /* temporaily enable interrupts */
#endif
                        while (queue->reqPending > len);
#ifdef _55_
            asm("  BSET ST1_INTM");          /* Disable interrupts again */
#endif
                }

        /*
         * Otherwise put the transfer request into DMA Job Queue.
         */
        //DMA_put(handle, src, dst, cnt); /* put job on queue */

        queue->writePtr->src = src;
        queue->writePtr->dst = dst;
        queue->writePtr->cnt = cnt;
        queue->writePtr->handle = dmaHandle;
        /* update write pointer */
        queue->writePtr = queue->writePtr->nextJob;
        
#if _INSTR_BANDWIDTH
        ACPY2_OMAP_updateChannelStats (dmaHandle, 0);   // Case 4
#endif // if _INSTR_BANDWIDTH
    }

#ifdef MAKE_HWI_CALL
    HWI_restore(state);
#else
#ifdef _55_
    if (!state) {
        /* Enable interrupts */
        asm("       BCLR ST1_INTM");
    }
#endif
#endif

#ifndef _RTS_MODE
    //TSK_enable();
#endif
}
#endif /* #if !DMA_OPT */
/*
 *  @(#) ti.sdo.fc.acpy2; 1,0,0,20; 5-10-2007 10:49:06; /db/atree/library/trees/fc-b12x/src/
 */

