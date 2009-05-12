%%{
    var numEnabled = 0;
    var enabledMask = 0;
    var priMask = 0;
    var heapId = 0;  // Not exposed to user.

    for (var i = 0; i < this.NUMCHANNELS; i++) {
        if (this.instance("DMAN24XX_CH" + i).enabled == true) {
            numEnabled++;
            enabledMask |= (1 << i);
            if (this.instance("DMAN24XX_CH" + i).priority == "high") {
                priMask |= (1 << i);
            }
        }
    }
%%}

%if (numEnabled) {
/* DMAN24XX C source output */
#include <ti/xdais/idma2.h>
#include <ti/sdo/fc/dman/dma4/_dman.h>

#pragma DATA_SECTION(_DMAN_channel, ".dman")

/*
 * ======== _DMAN_channel ========
 *
 *  Array of logical channels used by DMAN, with number of IDMA2 channels
 *  mapped to each logical channel.
 */
DmanChannel _DMAN_channel[`numEnabled`] = {
%for (var i = 0; i < numEnabled; i++) {
    {0, 0},
%}
};

#pragma DATA_SECTION(_DMAN_defaultQueueMap, ".dman")

/*
 * ======== _DMAN_defaultQueueMap ========
 *
 *  QueueId to channel map: Initialize to {illegal channel, 0 assignments made}
 */
QueueIdMap _DMAN_defaultQueueMap[`numEnabled`] = {
%for (var i = 0; i < numEnabled; i++) {
    {(Uns)-1, 0},
%}
};

#pragma DATA_SECTION(_DMA_channel, ".dman")

/*
 * ======== _DMA_channel ========
 * Array of logical channel numbers used by DMAN
 */
Int _DMA_channel[`numEnabled`] = {
%for (var i = 0; i < numEnabled; i++) {
    0,
%}
};

#pragma DATA_SECTION(DMAN_heapId, ".dman")
#pragma DATA_SECTION(DMAN_numChannels, ".dman")
#pragma DATA_SECTION(DMAN_numEnabled, ".dman")
#pragma DATA_SECTION(DMAN_irq, ".dman")
#pragma DATA_SECTION(DMAN_enabledMask, ".dman")
#pragma DATA_SECTION(DMAN_priMask, ".dman")
#pragma DATA_SECTION(DMA_queueLen, ".dman")
#pragma DATA_SECTION(_DMA_jobs, ".dman")
#pragma DATA_SECTION(_DMA_jobQueue, ".dman")

Int DMAN_heapId = `heapId`;
Int DMAN_numChannels = `this.NUMCHANNELS`;
Int DMAN_numEnabled = `numEnabled`;
Int DMAN_irq = `this.DMANIRQ`;
LgUns DMAN_enabledMask = `utils.toHex(enabledMask)`;
LgUns DMAN_priMask = `utils.toHex(priMask)`;

Int DMA_queueLen = `this.QUEUELEN`;

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
    struct _DMA_Job *nextJob;  /* next in job queue (last points to first) */
} _DMA_Job;

/*
 *  ======== _DMA_QueueObj ========
 */
typedef struct _DMA_QueueObj {
    _DMA_Handle     curHandle;  /* Points to the current hdl being processed */
    Int             reqPending; /* Number of outstanding reqs to be serviced */
    Int             len;        /* length of queue buffer (# of elements) */
    _DMA_Job       *readPtr;    /* current read pointer in the job buffer */
    _DMA_Job       *writePtr;   /* current write pointer in the job buffer */
} _DMA_QueueObj, *_DMA_Queue;

static _DMA_Job _DMA_jobs[`numEnabled` * `this.QUEUELEN`];

_DMA_QueueObj _DMA_jobQueue[`numEnabled`] = {
%for (var i = 0; i < numEnabled; i++) {
    {0, 0, `this.QUEUELEN`, 0, &_DMA_jobs[`this.QUEUELEN * i`]},
%}
};

%}  // if (numEnabled > 0)
