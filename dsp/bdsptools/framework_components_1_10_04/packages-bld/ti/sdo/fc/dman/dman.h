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
 *  @file       dman.h
 *
 *  @brief      DSP Bridge DMAN Interface Definitions (C55x) - Application
 *              interface to the DMA Manager. Grants DMA resources to XDAIS
 *              algorithms.
 */

/**
 *  @defgroup   DSPDMAN DMAN (C55x)
 *
 *              DMAN is an application interface to the DMA Manager (dman.c).
 *              It provides routines for granting and reclaiming DMA resources
 *              used by algorithms.  This module is used by frameworks or
 *              applications which have XDAIS algorithms that implement the
 *              IDMA2 interface.
 */

#ifndef DMAN_
#define DMAN_

/** @ingroup DSPDMAN */
/*@{*/


#ifdef __cplusplus
extern "C" {
#endif

#include <ti/xdais/ialg.h>
#include <ti/xdais/idma2.h>

#define DMAN_MAXDMARECS     16      /**< Maximum number of DMA hadles allowed
                                       for a XDAIS algorithm. */

#define DMAN_NOTUSED        -1      /**< DMAN is not used. */
#define DMAN_DAISALG        1       /**< XDAIS algorithm define. */
#define DMAN_BIOSDRIVER     2       /**< BIOS driver define. */

/**
 *  @name Defines: DMAN Status Codes
 */
/*@{*/
#define DMAN_SOK            0       /**< DMAN Status Code: Success. */
#define DMAN_EOUTOFMEMORY   -1      /**< DMAN Status Code: Failed to allocate
                                       memory for the handle. */
#define DMAN_EFAIL          -2      /**< DMAN Status Code: Unspecified error. */
/*@}*/


/**
 *  @brief      Grant logical DMA channel resources to an algorithm instance.
 *
 *  @param[in]  algHandle       XDAIS algorithm interface handle.
 *  @param[in]  dmaFxns         IDMA2 function interface table.  Must be defined
 *                              by the algorithm according to the XDAIS IDMA2
 *                              specification.
 *
 *  @retval     DMAN_SOK        Success. Logical DMA channel resources granted.
 *  @retval     DMAN_EOUTMEM    Failed to allocate logical DMA channel
 *                              resources.
 *  @retval     DMAN_EFAIL      The call to the algorithm's IDMA2 interface
 *                              function @c dmaInit() failed.
 *
 *  @pre        @c algHandle and @c dmaFxns must be pointers to valid interface
 *              tables.
 *  @pre        The IDMA2 interface must be for the same algorithm instance as
 *              the IALG interface. <tt>algHandle->fxns->implementationId ==
 *              dmaFxns->implementationId</tt>.
 *  @pre        The number of channels returned by algorithm's
 *              @c dmaGetChannelCnt() and @c dmaGetChannels() is not greater
 *              than @c DMAN_MAXDMARECS.  In other words, this DMAN supports
 *              only @c DMAN_MAXDMARECS channels.
 *
 *  @remarks    DMAN_addAlg() will atetmpt to allocate logical channels for an
 *              algorithm instance based on the algorithm's requests, as
 *              determined through a call to @c dmaFxns->dmaGetChannels().
 *  @remarks    DMAN_addAlg() will coerce any @c queueId requests which are
 *              greater than @c DMAN_MAXQID (defined in dman.h) to @c
 *              DMAN_MAXQID.
 *
 *  @remarks    The number of channels returned by @c
 *              dmaFxns->dmaGetChannelCnt() and @c dmaFxns->dmaGetChannels()
 *              must be less than or equal to DMAN_MAXDMARECS (defined in
 *              dman.h).
 *
 *  @sa         DMAN_removeAlg().
 */
extern Uns DMAN_addAlg(IALG_Handle algHandle, IDMA2_Fxns *dmaFxns);


/**
 *  @brief      Finalize the use of the DMAN module.
 *
 *  @remarks    DMAN_exit() must be called after using the DMA Manager,
 *              typically from the node's delete phase.
 */
extern Void DMAN_exit(Void);


/**
 *  @brief      Allocate resources for a logical DMA channel and return a
 *              handle to it.
 *
 *  @param[in]  queueId     Logical queueId associated with a DMA channel, as
 *                          specified by the node.
 *
 *  @retval     Non-NULL    Success.  Logical DMA channel allocated and a valid
 *                          and initialized IDMA2_Handle is returned by the API.
 *  @retval     NULL        Failed to allocate logical DMA channel due to lack
 *                          of memory.
 *
 *  @remarks
 *
 *  @sa         DMAN_freeLogicalDmaChannel()
 */
extern IDMA2_Handle DMAN_getLogicalDmaChannel(Int queueId);


/**
 *  @brief      Free resources used by a logical DMA channel, including its
 *              IDMA2_Handle @c dmaHandle.
 *
 *  @param[in]  dmaHandle   Valid IDMA2_Handle interface handle.
 *
 *  @pre        A valid IDMA2_Handle returned by a previous successful call to
 *              DMAN_getLogicalDmaChannel().
 *
 *  @post       The logical DMA channel handle (descriptor) is freed and the
 *              physical channel usage stats table (qidUsageTable) is
 *              decremented.
 *
 *  @sa         DMAN_getLogicalDmaChannel()
 */
extern Void DMAN_freeLogicalDmaChannel(IDMA2_Handle dmaHandle);


/**
 *  @brief      Initialize the DMAN module.
 *
 *  @remarks    DMAN_init() must be called prior to using any other DMAN API,
 *              typically in the node's create phase.
 */
extern Void DMAN_init(Void);


/**
 *  @brief      Reclaim logical DMA channel resources used by an algorithm
 *              instance.
 *
 *  @param[in]  algHandle   XDAIS algorithm interface handle.
 *  @param[in]  dmaFxns     IDMA2 function interface table.  Must be defined
 *                          by the algorithm according to the XDAIS IDMA2
 *                          specification.
 *
 *  @pre        @c algHandle and @c dmaFxns must be pointers to valid interface
 *              tables.
 *  @pre        The IDMA2 interface must be for the same algorithm instance as
 *              the IALG interface. <tt>algHandle->fxns->implementationId ==
 *              dmaFxns->implementationId</tt>.
 *  @pre        The number of channels returned by algorithm's
 *              @c dmaGetChannelCnt() and @c dmaGetChannels() is not greater
 *              than @c DMAN_MAXDMARECS.  In other words, this DMAN supports
 *              only @c DMAN_MAXDMARECS channels.
 *
 *  @remarks    DMAN_removeAlg() will free any logical channel resources
 *              reserved for an algorithm by a previous successful call to
 *              DMAN_addAlg().
 *
 *  @sa         DMAN_addAlg().
 */
extern Void DMAN_removeAlg(IALG_Handle algHandle, IDMA2_Fxns *dmaFxns);


#ifdef __cplusplus
}
#endif /* extern "C" */


/*@}*/ /* ingroup DSPDMAN */

#endif /*DMAN_*/
/*
 *  @(#) ti.sdo.fc.dman; 1,0,0,20; 5-10-2007 10:49:27; /db/atree/library/trees/fc-b12x/src/
 */

