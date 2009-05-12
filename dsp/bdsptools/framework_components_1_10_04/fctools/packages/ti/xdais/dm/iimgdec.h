/* 
 *  Copyright 2006
 *  Texas Instruments Incorporated
 *
 *  All rights reserved.  Property of Texas Instruments Incorporated
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 * 
 */

/**
 *  @file       ti/xdais/dm/iimgdec.h
 *
 *  @brief      This header defines all types, constants, and functions
 *              shared by all implementations of the image decoder
 *              algorithms.
 */
/**
 *  @defgroup   ti_xdais_dm_IIMGDEC   xDM 0.9 Image Decoder Interface
 *
 *  This is the xDM 0.9 Image Decoder Interface.
 */

#ifndef ti_xdais_dm_IIMGDEC_
#define ti_xdais_dm_IIMGDEC_

#include <ti/xdais/ialg.h>
#include <ti/xdais/xdas.h>
#include "xdm.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup    ti_xdais_dm_IIMGDEC */
/*@{*/

#define IIMGDEC_CID      "ti.xdais.dm.IIMGDEC"  /**< IIMGDEC Class ID */

#define IIMGDEC_ERUNTIME  XDM_ERUNTIME          /**< @copydoc XDM_ERUNTIME */
#define IIMGDEC_EOK       XDM_EOK               /**< @copydoc XDM_EOK */
#define IIMGDEC_EFAIL     XDM_EFAIL             /**< @copydoc XDM_EFAIL */

/**
 *  @brief      This must be the first field of all IIMGDEC
 *              instance objects.
 */
typedef struct IIMGDEC_Obj {
    struct IIMGDEC_Fxns *fxns;
} IIMGDEC_Obj;


/**
 *  @brief      Opaque handle to an IIMGDEC objects.
 */
typedef struct IIMGDEC_Obj  *IIMGDEC_Handle;


/**
 *  @brief      Defines the creation time parameters for
 *              all IIMGDEC instance objects.
 *
 *  @remarks    The application should set the parameters to 0 to use
 *              the algorithm's default values.
 *
 *  @remarks    This structure may be extended by individual codec
 *              implementation allowing customization with vendor
 *              specific parameters.
 *
 *  @remarks    The @c size field @b must be correctly set by the caller.
 */
typedef struct IIMGDEC_Params {
    XDAS_Int32 size;            /**< Size of this structure in bytes.
                                 *   Because this structure can be extended,
                                 *   this field @b must be correctly set
                                 *   by the caller.
                                 */
    XDAS_Int32 maxHeight;       /**< Maximum height. */
    XDAS_Int32 maxWidth;        /**< Maximum width. */
    XDAS_Int32 maxScans;        /**< Maximum number of scans. */
    XDAS_Int32 dataEndianness;  /**< Endianness of output data.
                                 *   See #XDM_DataFormat.
                                 */
    XDAS_Int32 forceChromaFormat;/**< Force decode in given Chroma format.
                                  *   See #XDM_ChromaFormat.
                                  */
}IIMGDEC_Params;

/**
 *  @brief      This structure defines the codec parameters that can be
 *              modified after creation via control() calls.
 *
 *  @remarks    It is not necessary that given implementation support all
 *              dynamic parameters to be configurable at run time.  If a
 *              particular algorithm does not support run-time updates to
 *              a parameter that the application is attempting to change
 *              at runtime, it may indicate this as an error.
 *
 *  @remarks    The @c size field @b must be correctly set by the caller.
 */
typedef struct IIMGDEC_DynamicParams {
    XDAS_Int32 size;            /**< Size of this structure in bytes.
                                 *   Because this structure can be extended,
                                 *   this field @b must be correctly set
                                 *   by the caller.
                                 */
    XDAS_Int32 numAU;           /**< Number of Access unit to decode.
                                 *   Set to #XDM_DEFAULT to decode the
                                 *   entire frame.
                                 */
    XDAS_Int32 decodeHeader;    /**< Decode mode.  See #XDM_DecMode */
    XDAS_Int32 displayWidth;    /**< Pitch.  If set to zero, imageWidth is
                                 *   used; else this field (assuming it's
                                 *   larger than imageWidth) is used.
                                 */
} IIMGDEC_DynamicParams;

/**
 *  @brief      Defines the input arguments for all IIMGDEC instance
 *              process function.
 *
 *  @remarks    This structure may be extended by individual codec
 *              implementation allowing customization with vendor
 *              specific parameters.
 *
 *  @remarks    The @c size field @b must be correctly set by the caller.
 */
typedef struct IIMGDEC_InArgs {
    XDAS_Int32 size;            /**< Size of this structure in bytes.
                                 *   Because this structure can be extended,
                                 *   this field @b must be correctly set
                                 *   by the caller.
                                 */
    XDAS_Int32 numBytes;        /**< Size of valid input data (in bytes) to
                                 *   be processed.
                                 */
} IIMGDEC_InArgs;

/**
 *  @brief      Defines instance status parameters (read-only).
 *
 *  @remarks    This structure may be extended by individual codec
 *              implementation allowing customization with vendor
 *              specific parameters.
 *
 *  @remarks    The @c size field @b must be correctly set by the caller.
 */
typedef struct IIMGDEC_Status {
    XDAS_Int32 size;            /**< Size of this structure in bytes.
                                 *   Because this structure can be extended,
                                 *   this field @b must be correctly set
                                 *   by the caller.
                                 */
    XDAS_Int32 extendedError;   /**< @copydoc XDM_ErrorBit. */
    XDAS_Int32 outputHeight;    /**< Output Height. */
    XDAS_Int32 outputWidth;     /**< Output Width.  Padding should be taken
                                 *   care of by the decoder.
                                 */
    XDAS_Int32 imageWidth;      /**< Image Width. */
    XDAS_Int32 outChromaformat; /**< Output chroma format.
                                 *   See #XDM_ChromaFormat.
                                 */
    XDAS_Int32 totalAU;         /**< Total number of Access Units. */
    XDAS_Int32 totalScan;       /**< Total number of scans. */
    XDM_AlgBufInfo  bufInfo;    /**< Input and output buffer information.
                                 *   See #XDM_AlgBufInfo.
                                 */
} IIMGDEC_Status;


/**
 *  @brief      Defines the run time output arguments for
 *              all IIMGDEC instance objects.
 *
 *  @remarks    This structure may be extended by individual codec
 *              implementations allowing customization with vendor
 *              specific parameters.
 *
 *  @remarks    The @c size field @b must be correctly set by the caller.
 */
typedef struct IIMGDEC_OutArgs {
    XDAS_Int32 size;            /**< Size of this structure in bytes.
                                 *   Because this structure can be extended,
                                 *   this field @b must be correctly set
                                 *   by the caller.
                                 */
    XDAS_Int32 extendedError;   /**< @copydoc XDM_ErrorBit. */
    XDAS_Int32 bytesconsumed;   /**< Number of bytes consumed during the
                                 *   process() call.
                                 */
    XDAS_Int32 currentAU;       /**< Current Access Unit Number. */
    XDAS_Int32 currentScan;     /**< Current Scan Number. */
} IIMGDEC_OutArgs;


/**
 *  @brief      Defines the control commands for the IIMGDEC module.
 *
 *  @remarks    This ID can be extended in IMOD interface for
 *              additional controls.
 *
 *  @sa         XDM_CmdId
 */
typedef  IALG_Cmd IIMGDEC_Cmd;


/**
 *  @brief      Defines all of the operations on IIMGDEC objects.
 */
typedef struct IIMGDEC_Fxns{
    IALG_Fxns   ialg;             /**< Traditional xDAIS algorithm interface. */

/**
 *  @brief      Basic image decoding call.
 *
 *  @param[in]  handle          Handle to an algorithm instance.
 *  @param[in]  inBufs          Input buffer descriptors.
 *  @param[in,out] outBufs      Output buffer descriptors.  The algorithm
 *                              may modify the output buffer pointers.
 *  @param[in]  inArgs          Input arguments.  This is a required
 *                              parameter.
 *  @param[out] outArgs         Ouput results.  This is a required parameter.
 *
 *  @retval     IALG_EOK        Success.
 *  @retval     IALG_EFAIL      Failure.  See IIMGDEC_OutArgs#extendedError
 *                              for more detailed further error conditions.
 */
    XDAS_Int32 (*process)(IIMGDEC_Handle handle, XDM_BufDesc *inBufs,
            XDM_BufDesc *outBufs, IIMGDEC_InArgs *inArgs,
            IIMGDEC_OutArgs *outArgs);

/**
 *  @brief      Control behaviour of an algorithm.
 *
 *  @param[in]  handle          Handle to an algorithm instance.
 *  @param[in]  id              Command id.  See #XDM_CmdId.
 *  @param[in]  params          Dynamic parameters.  This is a required
 *                              parameter.
 *  @param[out] status          Output results.  This is a required parameter.
 *
 *  @retval     IALG_EOK        Success.
 *  @retval     IALG_EFAIL      Failure.  See IIMGDEC_Status#extendedError
 *                              for more detailed further error conditions.
 */
    XDAS_Int32 (*control)(IIMGDEC_Handle handle, IIMGDEC_Cmd id,
            IIMGDEC_DynamicParams *params, IIMGDEC_Status *status);
}IIMGDEC_Fxns;

/*@}*/  /* ingroup */

#ifdef __cplusplus
}
#endif

#endif  /* ti_xdais_dm_IIMGDEC_ */
/*
 *  @(#) ti.xdais.dm; 1, 0, 1, 0,46; 10-18-2006 19:11:35; /db/wtree/library/trees/dais-g07x/src/
 */

