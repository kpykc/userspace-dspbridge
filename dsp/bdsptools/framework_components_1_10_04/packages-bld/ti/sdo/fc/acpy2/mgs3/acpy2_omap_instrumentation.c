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
 *  ======== mgs3/acpy2_omap_instrumentation.c ========
 *  DMA bandwidth instrumentation data structures and APIs.
 *
 */

#include <std.h>
#include "_dma.h"
#include "_dmaOmaphw.h"
#include <acpy2_omap_instrumentation.h>
#include <dbc.h>

#if _INSTR_BANDWIDTH

struct _ChanStat  _ChanStats[_DMA_MAX55CHANNELS];
    
#pragma CODE_SECTION(ACPY2_OMAP_updateChannelStats, ".text:ACPY2_OMAP_instrumentation")
#pragma CODE_SECTION(ACPY2_OMAP_resetChannelStats, ".text:ACPY2_OMAP_instrumentation")
#pragma CODE_SECTION(ACPY2_OMAP_getChannelStats, ".text:ACPY2_OMAP_instrumentation")

/*
 *  ======== ACPY2_OMAP_getChannelStats ========
 *  Get channel stats regarding DMA activity on the phys channel
 */
Void ACPY2_OMAP_getChannelStats(Uns chanId, ACPY2_OMAP_ChanStat * chanStat)
{
    *chanStat = _ChanStats[chanId];
}

/*
 *  ======== ACPY2_OMAP_resetChannelStats ========
 *  Get channel stats regarding DMA activity on the phys channel
 */
Void ACPY2_OMAP_resetChannelStats(Uns chanId)
{
    _ChanStats[chanId].bytesTransfered = 0;
    _ChanStats[chanId].bytesPending = 0;
    _ChanStats[chanId].numTransfersStarted = 0;
    _ChanStats[chanId].numTransfersFinished = 0;
}

/*
 *  ======== ACPY2_OMAP_getChannelStats ========
 *  Get channel stats regarding DMA activity on the phys channel
 */
Void ACPY2_OMAP_updateChannelStats(IDMA2_Handle handle, Uns cnt)
{
    Uns chanId = 0;

    /* 
     * Can be called from Four possible contexts 
     *     Cases 1,2: DMA completion ISR
     *     Cases 3,4: from ACPY2_start
     */
    if (handle == NULL) {
        /*
         * CASE 1 -- Called from DMA ISR
         *     when:: Xfer completed and S/W queue for channel is empty
         *     with args::  handle == NULL, cnt <- chanId
         *     Action ==> update #bytesTransfered
         *     Action ==> reset #bytesPending to 0
         *     Action ==> increment #numTransfersFinished
         */

        /* cnt argument used to pass chanId when handle==NULL */
        chanId = (Uns)cnt;

        DBC_assert (chanId < _DMA_MAX55CHANNELS);

        _ChanStats[chanId].bytesTransfered += _ChanStats[chanId].bytesPending;
        _ChanStats[chanId].bytesPending = 0;   
        _ChanStats[chanId].numTransfersFinished++;
    
    } else { /* handle NOT NULL */
        /* 
         * handle->cfn : number of frames
         * cnt         : when >0 => number of elements in each frame
         * handle->csdp: {0,1,2} for element-size={8bit, 16bit, 32bit}
         */
        chanId = (Uns)handle->channel;

        DBC_assert (chanId < _DMA_MAX55CHANNELS);

        if ((cnt > 0)) {
            /* CASES 2 & 3 have overlaps */ 
            /*
             * CASE 2 -- Called from DMA ISR
             *     when:: Xfer completed and Start new job from S/W queue
             *     ::  handle: NOT NULL, cnt: > 0 (actual count) 
             *     Action ==> update #bytesTransfered
             *     Action ==> set #bytesPending
             */

            /*
             * CASE 3 -- Called from ACPY2_start
             *     when:: Start Xfer directly without queueing into S/W queue
             *     ::  handle: NOT NULL, cnt: > 0 (actual count) 
             *     Action ==> set #bytesPending
             *     Action ==> increment #numTransfersStarted
             */

            /* 
             * if  bytesPending > 0, (while handle NOT NULL) 
             * ==> we are called from DMA ISR && s/w queue is empty 
             *                                && physical channel is idle.
             */
             if (_ChanStats[chanId].bytesPending) {
                 _ChanStats[chanId].bytesTransfered += 
                     _ChanStats[chanId].bytesPending;
                 _ChanStats[chanId].numTransfersFinished++;
             } else {
                 _ChanStats[chanId].numTransfersStarted ++;
             }

            /*
             * set bytesPending to reflect the Xfer just started from Queue.
             */
            _ChanStats[chanId].bytesPending = cnt * handle->cfn * 
                    (0x01 << (handle->csdp & CSDP_DATATYPE));

        } else {    /* (handle NOT NULL) and (cnt == 0) */
            /*
             * CASE 4 -- Called from ACPY2_start
             *     when::  channel busy => current request queued into S/W queue
             *     ::  handle: NOT NULL, cnt: == 0 
             *     Action ==> increment #numTransfersStarted
             */
             _ChanStats[chanId].numTransfersStarted ++;
        }
    }
}

#endif // if _INSTR_BANDWIDTH

/*
 *  @(#) ti.sdo.fc.acpy2; 1,0,0,20; 5-10-2007 10:49:06; /db/atree/library/trees/fc-b12x/src/
 */

