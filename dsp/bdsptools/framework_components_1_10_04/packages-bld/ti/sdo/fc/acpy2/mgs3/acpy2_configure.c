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
 *  ======== mgs3/acpy2_configure ========
 *  Implementaion of ACPY2_configure.
 *
 */

#include <std.h>
#ifndef _RTS_MODE
#include <dbc.h>
#else
#include <dbc_rts.h>
#endif
#include <ti/xdais/acpy2.h>
#include <ti/xdais/idma2.h>
#include "_dmaOmaphw.h"
#include "_dma.h"

#pragma CODE_SECTION(ACPY2_configure, ".text:ACPY2_configure")

/*
 *  ======== ACPY2_configure ========
 *  Configure the logical channel by copying the params into the IDMA2 logical
 *  channel object.
 *   Note: if ACPY2_OMAP APIs are used explicitly, each ACPY2_configure
 *       call resets the port endianism and packed/burst mode settings to
 *       ACPY2 defaults.
 */
Void ACPY2_configure(IDMA2_Handle handle, IDMA2_Params *params)
{
    Uns amode = CCR_END_AUTOINIT | CCR_HIGHPRIO;
    Uns elemIndexOffset;

    DBC_require(handle != NULL);
    DBC_require(params != NULL);

    /*
     *  Wait until all outstanding transfer submitted using this handle
     *  are completed.
     *  This is a trade-off for memory. Otherwise we have to store all
     *  config parameters within each DMA job in the s/w queue.
     */
    ACPY2_wait(handle);


    /*
     *   OMAPS00005223 and CQ7474:
     *   DEFAULT port endianism setting is BIG ENDIAN.
     *   This ensures backward compatibility for 1610 with 1510.
     *   For 1510,this register setting is ignored.
     *   
     */
    handle->ccr2 = 0; /* resets port enndianism to Big Endian */

#if _DISABLE_8BIT_PACKBURST

    /*
     *  This specific 8 bit DMA configuration allows us to get
     *  correct 8 bit data from external memory to internal
     *  memory.DSP DMA cannot actually make 8 bit reads but can make
     *  8 bit writes. Internally an 8 bit DMA read is converted into
     *  a 16 bit DMA read, this actually results in a byte swapped
     *  data. This issue exists on 1510 also, algorithms adjusted to this
     *  byte swapped 8 bit DMA. Temporarily we are disabling this code,
     *  to make 1610 backward compatible with 1610.
     *
     *  The below configuration prevents byte swap
     *
     *  Endianism is enabled only in the case of 8 bit transfers.
     *  This is due to the default byte swapping by EMIF.
     *  Since OMAP endianism word swap is on, 16/32 bit transfers
     *  are big endian DMA transfers by default
     *
     *  Note:
     *      CCR2 configuration does not have an affect, when
     *      SRC or DST is DSP internal memory
     */

    switch (params->elemSize) {
        case IDMA2_ELEM8:
            handle->ccr2 = CCR2_LE_SRC | CCR2_LE_DST;
            break;
        default:
            handle->ccr2 = 0;
    }

#endif

#ifdef  _OMAPS00005636_
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
#endif  /* _OMAPS00005636 */

    /* Enable Packed/Burst mode by default */
    handle->csdp = params->elemSize | CSDP_SRCDSTPKBEN_ENA ;

    handle->cfn  = params->numFrames;
    handle->csfi = 0;
    handle->cdfi = 0;
    handle->csei = 0;
    handle->cdei = 0;


    /* Pre-compute the ccr values */
    switch (params->xType) {
        case IDMA2_1D1D:
            amode |= CCR_AMODEAUTOINC;
            break;

        case IDMA2_1D2D:
            amode |= SRC_AMODE_PINC | DST_AMODE_SNDX | DST_AMODE_DNDX;
#ifdef      _OMAPS00005636_
            /* After swithing to xDAIS IDMA2 Based index usage */
            handle->cdei = params->dstElementIndex - elemIndexOffset;
            handle->cdfi = params->dstFrameIndex + 1;
#else
            /* GDMA based index definitions usage */
            handle->cdei = params->dstElementIndex;
            handle->cdfi = params->dstFrameIndex;
#endif
            break;

        case IDMA2_2D1D:
            amode |= DST_AMODE_PINC | SRC_AMODE_SNDX | SRC_AMODE_DNDX;
#ifdef      _OMAPS00005636_
            handle->csei = params->srcElementIndex - elemIndexOffset;
            handle->csfi = params->srcFrameIndex + 1;
#else
            handle->csei = params->srcElementIndex;
            handle->csfi = params->srcFrameIndex;
#endif
            break;

        case IDMA2_2D2D:
            amode |= DST_AMODE_SNDX | DST_AMODE_DNDX | SRC_AMODE_SNDX |
                SRC_AMODE_DNDX;
#ifdef      _OMAPS00005636_
            handle->cdei = params->dstElementIndex - elemIndexOffset;
            handle->cdfi = params->dstFrameIndex + 1;
            handle->csei = params->srcElementIndex - elemIndexOffset;
            handle->csfi = params->srcFrameIndex + 1;
#else
            handle->cdei = params->dstElementIndex;
            handle->cdfi = params->dstFrameIndex;
            handle->csei = params->srcElementIndex;
            handle->csfi = params->srcFrameIndex;
#endif
            break;

        default:
            DBC_assert(FALSE);
            break;
    }

    handle->ccr = amode;
    handle->cbFxn = NULL;
    handle->cbArg = NULL;
}
/*
 *  @(#) ti.sdo.fc.acpy2; 1,0,0,20; 5-10-2007 10:49:06; /db/atree/library/trees/fc-b12x/src/
 */

