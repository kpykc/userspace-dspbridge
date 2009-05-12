/* 
 *  Copyright 2007
 *  Texas Instruments Incorporated
 *
 *  All rights reserved.  Property of Texas Instruments Incorporated
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 * 
 */
/**
 *  @file       acpy2_omap.h
 *
 *  @brief      XDAIS ACPY2 OMAP Extension Definitions (C55x) - Provides ACPY2
 *              add-on utilities to manage system DMA resources.
 */

/**
 *  @defgroup   DSPACPY2_OMAP ACPY2 OMAP Extension (C55x)
 *
 *              The ACPY2 Extension for OMAP1xxx and OMAP242x (C55x based
 *              platforms) provides add-on utility functions to help users
 *              manipulate system DMA resources.  Its services include:
 *              - Configuration of DMA pack/burst mode.
 *              - Configuration of source and destination address endianism.
 *              - Configuration of callback functions that are invoked by DMA
 *                ISRs on completion of data transfers.
 *
 *              For detailed descriptions on the ACPY2 interface, please refer
 *              to the TMS320 DSP Algorithm Standard API Reference (SPRU360).
 */

#ifndef ACPY2_OMAP_
#define ACPY2_OMAP_

/** @ingroup    DSPACPY2_OMAP */
/*@{*/


#ifdef __cplusplus
extern "C" {
#endif


#include <ti/xdais/idma2.h>


/**
 *  @brief ACPY2 endianism types
 */
typedef enum ACPY2_OMAP_EndianismType {
    ACPY2_OMAP_LITTLEENDIAN,    /**< ACPY2 Little Endian Type. */
    ACPY2_OMAP_BIGENDIAN        /**< ACPY2 Big Endian Type. */
} ACPY2_OMAP_EndianismType;

/**
 *  @brief ACPY2 callback function prototype.
 */
typedef void (*ACPY2_OMAP_CallbackFxn)(Arg callbackArg);


/**
 *  @brief      Set the destination port endianism type for the given logical
 *              channel.
 *
 *  @param[in]  handle          Logical DMA channel handle.
 *  @param[in]  endianType  Port endianism type (little or big endian).
 *
 *  @pre        A valid logical DMA channel handle.
 *
 *  @sa         ACPY2_OMAP_getDstPortEndianism()
 */
extern Void ACPY2_OMAP_setDstPortEndianism(IDMA2_Handle handle,
    ACPY2_OMAP_EndianismType endianType);


/**
 *  @brief      Set the source port endianism type for the given logical
 *              channel.
 *
 *  @param[in]  handle          Logical DMA channel handle.
 *  @param[in]  endianType  Port endianism type (little or big endian).
 *
 *  @pre        A valid logical DMA channel handle.
 *
 *  @sa         ACPY2_OMAP_getSrcPortEndianism()
 */
extern Void ACPY2_OMAP_setSrcPortEndianism(IDMA2_Handle handle,
    ACPY2_OMAP_EndianismType endianType);


/**
 *  @brief      Get the destination port endianism setting of the given logical
 *              channel.
 *
 *  @param[in]  handle          Logical DMA channel handle.
 *
 *  @retval     Destination port endianims type of the given logical channel.
 *
 *  @pre        A valid logical DMA channel handle.
 *
 *  @sa         ACPY2_OMAP_setDstPortEndianism()
 */
ACPY2_OMAP_EndianismType ACPY2_OMAP_getDstPortEndianism(IDMA2_Handle handle);


/**
 *  @brief      Get the source port endianism setting of the given logical
 *              channel.
 *
 *  @param[in]  handle          Logical DMA channel handle.
 *
 *  @retval     Source port endianism type of the given logical channel.
 *
 *  @pre        A valid logical DMA channel handle.
 *
 *  @sa         ACPY2_OMAP_setSrcPortEndianism()
 */
ACPY2_OMAP_EndianismType ACPY2_OMAP_getSrcPortEndianism(IDMA2_Handle handle);


/**
 *  @brief      Enable Packed and Burst mode for the given logical channel.
 *
 *  @param[in]  handle          Logical DMA channel handle.
 *
 *  @pre        A valid logical DMA channel handle.
 *
 *  @sa         ACPY2_OMAP_disablePackedBurst()
 */
Void ACPY2_OMAP_enablePackedBurst(IDMA2_Handle handle);


/**
 *  @brief      Disable Packed and Burst mode for the given logical channel.
 *
 *  @param[in]  handle          Logical DMA channel handle.
 *
 *  @pre        A valid logical DMA channel handle.
 *
 *  @sa         ACPY2_OMAP_enablePackedBurst()
 */
Void ACPY2_OMAP_disablePackedBurst(IDMA2_Handle handle);


/**
 *  @brief      Set the callback function called by the DMA ISR.
 *
 *  @param[in]  handle  Logical DMA channel handle.
 *  @param[in]  cbFxn   Callback function pointer used in notification of
 *                      completed DMA transfer.
 *  @param[in]  cbArg   Argument passed to the callback function.
 *
 *  @pre        A valid logical DMA channel handle.
 *
 *  @remarks    ACPY2_OMAP_setCallbackNotification sets the callback function
 *              and corresponding function arguments that is called by the DMA
 *              ISR when a physical DMA transfer completes on the logical
 *              channel.  The callback function is not called if the input
 *              function and args are NULL.
 */
Void ACPY2_OMAP_setCallbackNotification(IDMA2_Handle handle,
    ACPY2_OMAP_CallbackFxn cbFxn, Arg cbArg);


/**
 *  @brief      Get the physical channel assigned to the logical DMA channel
 *              represented by the handle.
 *
 *  @param[in]  handle      Logical DMA channel handle.
 *
 *  @retval     Physical channel assigned to the specified logical DMA channel.
 *
 *  @pre        A valid logical DMA channel handle.
 */
Uns ACPY2_OMAP_getPhysicalChannel(IDMA2_Handle handle);


#ifdef __cplusplus
}
#endif /* extern "C" */


/*@}*/ /* ingroup ACPY2 */


#endif /* ACPY2_OMAP_ */
/*
 *  @(#) ti.sdo.fc.acpy2; 1,0,0,20; 5-10-2007 10:49:05; /db/atree/library/trees/fc-b12x/src/
 */

