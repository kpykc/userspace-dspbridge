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
 *  ======== mgs3/dma.c ========
 *  This file contains the queueing mechanism used for dma jobs, the
 *  associated fxns to manage the queue and the programming of the OMAP
 *  (C55x GDM) dma device.
 *
 */

#pragma CODE_SECTION(DMA_init, ".text:init")
#pragma CODE_SECTION(DMA_exit, ".text:exit")

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

#include <dma.h>
#include <ti/sdo/fc/dman/dman.h>
#include "_dma.h"
#include "_dmaOmaphw.h"

/* Interrupt masks */
#define DMAN_INT0               18
#define DMAN_INT1               9
#define DMAN_INT2               20
#define DMAN_INT3               21
#define DMAN_INT4               14
#define DMAN_INT5               15


/* These globals are in dman.h55 */
extern Int DMAN_channel[];

/* The following functions are in dman_isr.s55 module */
extern Void dmanIsr0();
extern Void dmanIsr1();
extern Void dmanIsr2();
extern Void dmanIsr3();
extern Void dmanIsr4();
extern Void dmanIsr5();

Fxn _dmaIsrFxn[] = {(Fxn)dmanIsr0,
                    (Fxn)dmanIsr1,
                    (Fxn)dmanIsr2,
                    (Fxn)dmanIsr3,
                    (Fxn)dmanIsr4,
                    (Fxn)dmanIsr5,};

extern _DMA_Queue _DMA_jobQueue[_DMA_MAX55CHANNELS];

Int _dmaIntId[] = {DMAN_INT0, DMAN_INT1, DMAN_INT2 ,DMAN_INT3, DMAN_INT4,
                   DMAN_INT5,};

/* 5510 DMA channel control registers 0..5 */
ioport DDMA_ChanRegs * dmaChan_dman[] =
{
    (DDMA_ChanRegs *)0xc00,    /* DMA ch 0 */
    (DDMA_ChanRegs *)0xc20,    /*    "   1 */
    (DDMA_ChanRegs *)0xc40,    /*    "   2 */
    (DDMA_ChanRegs *)0xc60,    /*    "   3 */
    (DDMA_ChanRegs *)0xc80,    /*    "   4 */
    (DDMA_ChanRegs *)0xca0     /*    "   5 */
};

/* define address of IER0, IER1 */
Uns * DMAN_IER0 = (Uns *) 0;      /* from c5500/csl_chiphal.h */
Uns * DMAN_IER1 = (Uns *) 0x45;   /* IER1 */

/* 5510 DMA global reg */
ioport unsigned * DMAN_GCR_REG = (ioport unsigned *)0xe00;

/* 5510 DMA global software incompatible register */
ioport unsigned * DMAN_GSCR_REG = (ioport unsigned *)0xe02;


/* Local fxn declarations */
static Void dmaintr(Int dchan, Bool enable);

static Void initQueue(_DMA_Queue queue, Int n, Int channelNum);

static Void initChannel(Int channel);


/*
 *  ======== DMA_exit ========
 *  Module finalization method.
 */
Void DMA_exit(Void)
{
    Int  i;

    /* Free up all the Queues */
    for (i = 0; i < _DMA_MAX55CHANNELS; i++) {
        if (DMAN_channel[i] != DMAN_NOTUSED) {
            dmaintr(i, FALSE);
            //C55_plug(_dmaIntId[i], (Fxn)HWI_unused);
        }
    }
}

/*
 *  ======== DMA_init ========
 *  Module initialization method
 */
Bool DMA_init(Void)
{
    Bool ret = TRUE;
    Int  i;

    /*
     *  Set up the Global registers for DMA
     *  Global DMA settings: free = 0, EHPI EXCL = 0, EHPI PRI = 0
     *  free = 1 -> DMA transfers continue uninterrupted if a brkpt occurs.
     */
    *DMAN_GCR_REG = *DMAN_GCR_REG | 0;
    /* Separate source and destination element/frame index; ie
     * dest index is different than the source
     */
    *DMAN_GSCR_REG = SRC_DEST_INDEX;

    /* Plug the interrupts */
    for (i = 0; i < _DMA_MAX55CHANNELS; i++) {
        if (DMAN_channel[i] != DMAN_NOTUSED) {
            /* Plug the Interrupts */
#ifndef _RTS_MODE
            C55_plug(_dmaIntId[i], (Fxn)(_dmaIsrFxn[i]));
#endif
            /* Initilaize the DMA channels */
            initChannel(i);
            initQueue(_DMA_jobQueue[i], _DMA_QUEUELEN, i);
        }
    }
    return (ret);
}

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
 *  ======== dmaintr ========
 *   Given DMA[0..5] channel Set/Clear proper DMACx Bit in IER0[1] register.
 */
static Void dmaintr(Int dchan, Bool enable)
{
    Uns DMACxBit = 0;    /* dma chan's INTR enable bit */

    switch(dchan) {
        case 1:
            DMACxBit = (Uns)IER0_DMAC1;
            break;
        case 4:
            DMACxBit = (Uns)IER0_DMAC4;
            break;
        case 5:
            DMACxBit = (Uns)IER0_DMAC5;
            break;
    }

    /* if chan 1,4,5 then set/clear IER 0 DMACx bit */
    if (DMACxBit) {
        *DMAN_IER0 = (enable) ? *DMAN_IER0 | DMACxBit : *DMAN_IER0 & ~DMACxBit;
    }
    else {
        switch(dchan) {
            case 0:
                DMACxBit = (Uns)IER1_DMAC0;
                break;
            case 2:
                DMACxBit = (Uns)IER1_DMAC2;
                break;
            case 3:
                DMACxBit = (Uns)IER1_DMAC3;
                break;
        }
        DBC_assert(DMACxBit);

        /* if chan 0,2,3 then IER1 */
        *DMAN_IER1 = (enable) ? *DMAN_IER1 | DMACxBit : *DMAN_IER1 & ~DMACxBit;
    }
}

/*
 *  ======== initChannel ========
 *  Initialize the Channel
 */
static Void initChannel(Int channel)
{
    ioport DDMA_ChanRegs * regs;
    Uns dummyRd;
    Bool hiPri = TRUE; /* In the future this can come from DMAN Config */

    /* Enable the IER register */
    regs = dmaChan_dman[channel];

    /* read channel status register to clear */
    dummyRd = regs->CSR;
    /* A dummy operation.  Ignore. */
    dummyRd |= dummyRd;

    /* enable DMACx dma control intr to CPU */
    dmaintr(channel, TRUE);

    /* turn on BLOCK IE to signal block completion and TIMEOUT IE bit. */
    regs->CICR = CICR_BLOCKIE | CICR_TIMEOUTIE;

    /* frame and element index not used; By default we do post increment */
    regs->CSFI = 0;
    regs->CSEI = 0;
    regs->CDFI = 0;
    regs->CDEI = 0;

    regs->CCR &= ~CCR_HIGHPRIO; /* clear hi pri field */

    /*
     * DST AMODE = auto post incr.
     * SRC AMODE = auto post incr.
     * repeat and autoinit = 0,
     * channel enabled: EN = 0, no sync.
     * set hi pri if hiPri == TRUE for any open call.
     */
   regs->CCR = CCR_AMODEAUTOINC | (hiPri ? CCR_HIGHPRIO : 0);
   return;
}


/*
 *  ======== initQueue ========
 *  Initialize the queue.
 */
static Void initQueue(_DMA_Queue queue, Int n, Int channelNum)
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
    queue->curHandle = 0;           // this should have already been done by cdb
    queue->reqPending = 0;          // this should have already been done by cdb
    queue->channelNum = channelNum; // this should have already been done by cdb
    buffer = queue->writePtr;
    
    if (!buffer)
        return;   // Should never happen! 

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

