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
 *  ======== dma4/acpy2_configure.c ========
 *  Implementaion of ACPY2_configure.
 */

#include <std.h>
#ifndef _RTS_MODE
#include <dbc.h>
#else
#include <dbc_rts.h>
#endif
#include <ti/xdais/acpy2.h>
#include <ti/xdais/idma2.h>
#include "_dma.h"
#include "_dma4_omap.h"

#pragma CODE_SECTION(ACPY2_configure, ".text:ACPY2_configure")

/*
 *  ======== ACPY2_configure ========
 *  Configure the logical channel by copying the params into the IDMA2 logical
 *  channel object.
 */
Void ACPY2_configure(IDMA2_Handle handle, IDMA2_Params *params)
{
    Uns amode;
    Uns elemIndexOffset;
    _DMA_Handle dmaHandle = (_DMA_Handle)handle;

    DBC_require(dmaHandle != NULL);
    DBC_require(params != NULL);

    /*
     *  Wait until all outstanding transfer submitted using this handle
     *  are completed.
     *  This is a trade-off for memory. Otherwise we have to store all
     *  config parameters within each DMA job in the s/w queue.
     */
    ACPY2_wait(handle);


    /* 
     * XDAIS Spec definition of FRAME INDEXes (srcFrameIndex or dstFrameIndex)
     * is the size of the gap in 8-bit bytes between two consecutive frames
     * frames within a block. 
     * Megastar GDMA definition is the size of the gap in bytes PLUS 1.
     *
     * XDAIS Spec definition of ELEMENT INDEX (for src & dst)
     * is the size of the gap between two consecutive elements
     * within a frame PLUS ONE ELEMENT SIZE measured in 8-bit bytes.
     * Megastar GDMA definition is still the size of the gap in bytes PLUS 1.
     * 
     */
    switch (params->elemSize) {
        case  IDMA2_ELEM8:
            elemIndexOffset = 0;
            break;
        case IDMA2_ELEM16:
            elemIndexOffset = 1;
            break;
        case IDMA2_ELEM32:
            elemIndexOffset = 3;
    }

    dmaHandle->csdp = params->elemSize;
    dmaHandle->cfn  = params->numFrames;
    dmaHandle->csfi = 0;
    dmaHandle->cdfi = 0;
    dmaHandle->csei = 0;
    dmaHandle->cdei = 0;


    /* Pre-compute the ccr values */
    switch (params->xType) {
        case IDMA2_1D1D:
            amode = DMA4_CCR_AMODEAUTOINC;
            break;

        case IDMA2_1D2D:
            amode = DMA4_CCR_SRC_AMODE_PINC | DMA4_CCR_DST_AMODE_DNDX;
            dmaHandle->cdei = params->dstElementIndex - elemIndexOffset;
            dmaHandle->cdfi = params->dstFrameIndex + 1;
            break;

        case IDMA2_2D1D:
            amode = DMA4_CCR_DST_AMODE_PINC | DMA4_CCR_SRC_AMODE_DNDX;
            dmaHandle->csei = params->srcElementIndex - elemIndexOffset;
            dmaHandle->csfi = params->srcFrameIndex + 1;
            break;

        case IDMA2_2D2D:
            amode = DMA4_CCR_DST_AMODE_DNDX | DMA4_CCR_SRC_AMODE_DNDX;
            dmaHandle->cdei = params->dstElementIndex - elemIndexOffset;
            dmaHandle->cdfi = params->dstFrameIndex + 1;
            dmaHandle->csei = params->srcElementIndex - elemIndexOffset;
            dmaHandle->csfi = params->srcFrameIndex + 1;
            break;

        default:
            DBC_assert(FALSE);
            break;
    }

    dmaHandle->ccr = amode;
}
/*
 *  @(#) ti.sdo.fc.acpy2; 1,0,0,20; 5-10-2007 10:49:05; /db/atree/library/trees/fc-b12x/src/
 */

