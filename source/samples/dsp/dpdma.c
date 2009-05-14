/*
 *  Copyright 2001-2008 Texas Instruments - http://www.ti.com/
 * 
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 *  ======== dpdma.c ========
 *  DMA Pipe Driver.
 *
 *  This pipe device is a software device used to stream data between two
 *  DSP/BIOS-II tasks.  This driver is based upon DBP, but uses DMA to copy
 *  the data.
 */

#include <string.h>

#include <std.h>

#include <dbc.h>
#include <dbg.h>
#include <dpdma.h>
#include <dev.h>
#include <mem.h>
#include <que.h>
#include <sem.h>
#include <sys.h>
#include <tsk.h>

#include <ti/xdais/acpy2.h>
#include <ti/sdo/fc/acpy2/acpy2_omap.h>
#include <ti/sdo/fc/dman/dman.h>

#include <stdlib.h>     /* needed for atoi() */


#ifdef _DBG_
#define Static
#else
#define Static static
#endif

#define DMACHANNEL      0       /* Logical DMA Channel number to use */


/*
 * *ONE* shared pipe object (SDMAPipeObj) is allocated for each pipe,
 * (e.g. "/dmapipe0").  Readers and writers adjust their semaphore pointers
 * (in DPDMA_open) to use the common pair residing in SDMAPipeObj.
 */
typedef struct SDMAPipeObj {
    QUE_Elem        link;           /* MUST be first element of structure */
    SEM_Handle      dataSem;        /* sem posted by output side */
    SEM_Handle      freeSem;        /* sem posted by input side */
    SEM_Handle      readySem[2];    /* ready sem for each device */
    DEV_Handle      device[2];      /* handle for in and out devices */
    Int             id;             /* pipe id (e.g., '1' for /dbpipe1 */
} SDMAPipeObj;

/*
 * One DMAPipeObj is allocated for each open device.
 */
typedef struct DMAPipeObj {
    SEM_Handle      toSem;          /* sem posted on DPDMA_issue */
    SEM_Handle      fromSem;        /* sem pended on by DPDMA_reclaim */
    SDMAPipeObj     *sPipe;
    IDMA2_Handle    dmaChannel;     /* logical DMA channel */
} DMAPipeObj;


static void onDmaComplete(Arg arg0);

/*
 *  Driver function table.
 */
Static Int      DPDMA_close(DEV_Handle dev);
Static Int      DPDMA_idle(DEV_Handle device, Bool flush);
Static Int      DPDMA_issue(DEV_Handle dev);
Static Int      DPDMA_open(DEV_Handle dev, String name);
Static Bool     DPDMA_ready(DEV_Handle dev, SEM_Handle sem);
Static Int      DPDMA_reclaim(DEV_Handle dev);

DEV_Fxns DPDMA_FXNS = {
    DPDMA_close,          /* close */
    DEV_CTRL,             /* ctrl */
    DPDMA_idle,           /* idle */
    DPDMA_issue,          /* issue */
    DPDMA_open,           /* open */
    DPDMA_ready,          /* ready */
    DPDMA_reclaim,        /* reclaim */
};

DPDMA_Params DPDMA_PARAMS = {
    0,      /* dummy */
};

Static SEM_Handle DPDMA_mutex;    /* for mutual exclusion in open and close */
Static QUE_Handle sharedPipeList; /* list of all shared pipe objects */

Static SDMAPipeObj *mkSharedPipe(DEV_Handle dev);


/*
 *  ======== DPDMA_close ========
 */
Static Int DPDMA_close(DEV_Handle dev)
{
    DMAPipeObj      *pipe = (DMAPipeObj *)dev->object;
    SDMAPipeObj     *sPipe = pipe->sPipe;


    DMAN_freeLogicalDmaChannel(pipe->dmaChannel);
    MEM_free(0, pipe, sizeof (DMAPipeObj));

    /* make sure have exclusive access to shared pipe list */
    SEM_pend(DPDMA_mutex, SYS_FOREVER);

    sPipe->device[dev->mode] = NULL;
    sPipe->readySem[dev->mode] = NULL;

    if (sPipe->device[DEV_INPUT] == NULL &&
                sPipe->device[DEV_OUTPUT] == NULL) {
        /* delete all shared pipe sub-objects */
        SEM_delete(sPipe->dataSem);
        SEM_delete(sPipe->freeSem);

        /* remove sPipe obj from sharedPipeList */
        QUE_remove(&sPipe->link);

        /* delete sPipe object itself */
        MEM_free(0, sPipe, sizeof (SDMAPipeObj));
    }

    SEM_post(DPDMA_mutex);

    return (SYS_OK);
}

/*
 * ======== DPDMA_idle ========
 */
Static Int DPDMA_idle(DEV_Handle device, Bool flush)
{
    DMAPipeObj    *pipe = (DMAPipeObj *)device->object;

    if (flush) {
        /*
         * Wait for all DMA transfers to complete first. We can't really
         * flush, since there is no way to stop the DMA transfers that have
         * already been queued. Therefore, the flush case is the same as
         * the non-flush case.
         */
        while (!ACPY2_complete(pipe->dmaChannel)) {
            ;
        }
    }
    else {
        /* Wait for DMA transfers to complete */
        while (!ACPY2_complete(pipe->dmaChannel)) {
            ;
        }
    }
    return (SYS_OK);
}

/*
 *  ======== DPDMA_init ========
 */
Void DPDMA_init(Void)
{
    /* Make sure the initialization happens only once for the DPDMA driver.*/
    static Bool curInit = FALSE;

    if ( curInit ) {
        return;
    }
    curInit = TRUE;

    DPDMA_mutex = SEM_create(1, NULL);
    sharedPipeList = QUE_create(NULL);

    if (DPDMA_mutex == NULL || sharedPipeList == NULL) {
        SYS_abort("DPDMA");
    }
}


/*
 *  ======== DPDMA_issue ========
 */
Static Int DPDMA_issue(DEV_Handle dev)
{
    DMAPipeObj      *pipe = (DMAPipeObj *)dev->object;
    SDMAPipeObj     *sPipe = pipe->sPipe;
    DEV_Handle      otherdev = sPipe->device[dev->mode ^ 0x1];
    DEV_Frame       *otherframe;
    DEV_Frame       *frame;
    DEV_Frame       *srcframe;
    DEV_Frame       *dstframe;
    LgUns           size;

    /*
     * Atomically check that each side has a frame so we can do an
     * exchange.  We can't be sure that a frame is on the
     * dev->todevice queue (just put there by SIO) since a task
     * switch to the task on the other side might intervene and
     * take the frame from this side.
     */
    TSK_disable();
    if (otherdev != NULL &&
        !QUE_empty(dev->todevice) && !QUE_empty(otherdev->todevice)) {

        otherframe = QUE_get(otherdev->todevice);
        frame = QUE_get(dev->todevice);

        /*
         *  Transfer buffers to fromdevice queue, but semaphores won't be
         *  posted until DMA transfer is complete.
         */
        QUE_put(dev->fromdevice, frame);
        QUE_put(otherdev->fromdevice, otherframe);

        /* done with atomic stuff */
        TSK_enable();

        if (dev->mode == DEV_INPUT) {
            dstframe = frame;
            srcframe = otherframe;
        }
        else {
            dstframe = otherframe;
            srcframe = frame;
        }
        size = dstframe->size = srcframe->size;
        dstframe->arg = srcframe->arg;

        /*
         * frames reclaimed from an output device must have size 0.
         */
        if (dev->mode != DEV_INPUT) {
            frame->size = 0;
        }
        else {
            otherframe->size = 0;
        }

        /* Now do the DMA transfer to initialize the dst buffer */
        ACPY2_start(pipe->dmaChannel, IDMA2_ADRPTR((LgUns)srcframe->addr),
                IDMA2_ADRPTR((LgUns)dstframe->addr), size);
    }
    else {
        /* done with atomic stuff */
        TSK_enable();
    }

    return (SYS_OK);
}


/*
 *  ======== DPDMA_open ========
 */
Static Int DPDMA_open(DEV_Handle dev, String name)
{
    DMAPipeObj      *pipe;
    SDMAPipeObj     *sPipe, *tmpPipe;
    IDMA2_Params    params;

    DBG_printf2(TRACE, "DPDMA_open: entry dev->devid = %x, &name = %x",
        dev->devid, (LgUns)name);

    /* decode and validate devid */
    /*
     * NOTE: type cast dev->devid as Short to workaround C55x CDB generation
     * that results in 0x0000ffff instead of 0xffffffff for -1.  Note that
     * Short is 16-bits for both 54x and 55x.
     */
    if ((Short) dev->devid < 0) {
        dev->devid = atoi(name);
    }
    DBG_printf1(TRACE, "DPDMA_open: dev->devid is now = %x", (Uns) dev->devid);

    /* make sure have exclusive access to shared pipe list */
    SEM_pend(DPDMA_mutex, SYS_FOREVER);

    /* search pipe list for previously opened pipe with same id */
    sPipe = MEM_ILLEGAL;
    if (!QUE_empty(sharedPipeList)) {
        tmpPipe = (SDMAPipeObj *)QUE_head(sharedPipeList);
        do {
            if (tmpPipe->id == dev->devid) {
                sPipe = tmpPipe;
                break;
            }
            tmpPipe = (SDMAPipeObj *)QUE_next((&tmpPipe->link));
        } while (tmpPipe != (SDMAPipeObj *)sharedPipeList);
    }

    /* allocate DMAPipeObj */
    pipe = MEM_alloc(0, sizeof (DMAPipeObj), 0);
    if (pipe == MEM_ILLEGAL) {  /* on fail release DPDMA_mutex, return error */
        SEM_post(DPDMA_mutex);
        SYS_abort("MEM_alloc() failed\n");
        return (SYS_EALLOC);
    }

    pipe->dmaChannel = DMAN_getLogicalDmaChannel(DMACHANNEL);

    if (pipe->dmaChannel == NULL) {
        SEM_post(DPDMA_mutex);                         /* release mutex */
        MEM_free(0, pipe, sizeof(DMAPipeObj));
        SYS_abort("MEM_alloc() failed\n");
        return (SYS_EALLOC);
    }

    params.xType = IDMA2_1D1D;
    params.elemSize = IDMA2_ELEM16;
    params.numFrames = 1;

    ACPY2_configure(pipe->dmaChannel, &params);

    /* Set the callback function to onDmaComplete */
    ACPY2_OMAP_setCallbackNotification(pipe->dmaChannel,
            (ACPY2_OMAP_CallbackFxn)onDmaComplete, (Arg)dev);

    if (sPipe == MEM_ILLEGAL) {
        /*
         * Allocate and initialize SDMAPipeObj on first open.
         */
        sPipe = mkSharedPipe(dev);
        if (sPipe == MEM_ILLEGAL) {
            SEM_post(DPDMA_mutex);                         /* release mutex */
            DMAN_freeLogicalDmaChannel(pipe->dmaChannel);

            MEM_free(0, pipe, sizeof (DMAPipeObj)); /* free DMAPipeObj */
            SYS_abort("MEM_alloc() failed\n");
            return (SYS_EALLOC);
        }
        QUE_put(sharedPipeList, &sPipe->link);
        DBG_printf1(TRACE, "DPDMA_open: allocated new SDMAPipeObj = %x",
            (LgUns) sPipe);
    }
    else {      /* sPipe was found already on the list */
        if (sPipe->device[dev->mode] != NULL) {
            DBG_printf1(TRACE, "DPDMA_open: BUSY %x", (LgUns)
                &(sPipe->device[dev->mode]) );
            /*
             * Only one input and one output device allowed for each pipe:
             */
            SEM_post(DPDMA_mutex);                         /* release mutex */
            DMAN_freeLogicalDmaChannel(pipe->dmaChannel);
            MEM_free(0, pipe, sizeof (DMAPipeObj)); /* free pipe object */
            SYS_abort("Device already in use\n");
            return (SYS_EBUSY);
        }
    }

    sPipe->device[dev->mode] = dev;     /* attach device to shared pipe obj */

    SEM_post(DPDMA_mutex);

    /*
     * Criss-cross SEM handles so both sides are referencing
     * the same physical objects.
     */
    if (dev->mode == DEV_INPUT) {
        pipe->fromSem = sPipe->dataSem;
        pipe->toSem = sPipe->freeSem;
    }
    else {
        pipe->toSem = sPipe->dataSem;
        pipe->fromSem = sPipe->freeSem;
    }

    pipe->sPipe = sPipe;                /* back reference shared obj */
    dev->object = (Ptr)pipe;            /* set device object */

    return (SYS_OK);
}


/*
 *  ======== DPDMA_ready ========
 *  called by SIO_select(), returns TRUE if device is ready.
 */
Static Bool DPDMA_ready(DEV_Handle dev, SEM_Handle sem)
{
    DMAPipeObj  *pipe = (DMAPipeObj *)dev->object;

    pipe->sPipe->readySem[dev->mode] = sem;

    return !(QUE_empty(dev->fromdevice));
}


/*
 *  ======== DPDMA_reclaim ========
 */
Static Int DPDMA_reclaim(DEV_Handle dev)
{
    DMAPipeObj  *pipe = (DMAPipeObj *)dev->object;

    if (SEM_pend(pipe->fromSem, dev->timeout)) {
        return (SYS_OK);
    }
    else {
        return (SYS_ETIMEOUT);
    }
}


/*
 *  ======== mkSharedPipe ========
 */
Static SDMAPipeObj *mkSharedPipe(DEV_Handle dev)
{
    /* allocate and zero-file a shared pipe object */
    SDMAPipeObj *sPipe = MEM_calloc(0, sizeof (SDMAPipeObj), 0);

    if (sPipe != MEM_ILLEGAL) {
        sPipe->dataSem = SEM_create(0, NULL);
        sPipe->freeSem = SEM_create(0, NULL);
        if ((sPipe->dataSem == NULL) || (sPipe->freeSem == NULL)) {
            goto e1;
        }

        sPipe->id = dev->devid;

        return (sPipe);
    }
    else {
        return (MEM_ILLEGAL);
    }

e1:
    if (sPipe) {
        if (sPipe->dataSem) {
            SEM_delete(sPipe->dataSem);
        }
        if (sPipe->freeSem) {
            SEM_delete(sPipe->freeSem);
        }

        /* delete sPipe object itself */
        MEM_free(0, sPipe, sizeof (SDMAPipeObj));
    }

    return (MEM_ILLEGAL);
}

/*
 *  ======== onDmaComplete ========
 */
static void onDmaComplete(Arg arg0)
{
    DEV_Handle      dev = (DEV_Handle)arg0;
    DMAPipeObj      *pipe = (DMAPipeObj *)dev->object;
    SDMAPipeObj     *sPipe = pipe->sPipe;
    SEM_Handle      otherReady = sPipe->readySem[dev->mode ^ 0x1];

    SEM_post(pipe->toSem);
    SEM_post(pipe->fromSem);

    if (otherReady != NULL) {
        SEM_post(otherReady);
    }
}
