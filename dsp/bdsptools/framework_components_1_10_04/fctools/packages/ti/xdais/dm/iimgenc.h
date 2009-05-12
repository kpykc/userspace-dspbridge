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
 *  @file       ti/xdais/dm/iimgenc.h
 *
 *  @brief      This header defines all types, constants, and functions
 *              shared by all implementations of the image encoder
 *              algorithms.
 */
/**
 *  @defgroup   ti_xdais_dm_IIMGENC   xDM 0.9 Image Encoder Interface
 *
 *  This is the xDM 0.9 Image Encoder Interface.
 */

#ifndef ti_xdais_dm_IIMGENC_
#define ti_xdais_dm_IIMGENC_

#include <ti/xdais/ialg.h>
#include <ti/xdais/xdas.h>
#include "xdm.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup    ti_xdais_dm_IIMGENC */
/*@{*/

#define IIMGENC_CID      "ti.xdais.dm.IIMGENC"  /**< IIMGENC Class ID */

#define IIMGENC_ERUNTIME  XDM_ERUNTIME          /**< @copydoc XDM_ERUNTIME */
#define IIMGENC_EOK       XDM_EOK               /**< @copydoc XDM_EOK */
#define IIMGENC_EFAIL     XDM_EFAIL             /**< @copydoc XDM_EFAIL */

/**
 *  @brief      This must be the first field of all IIMGENC
 *              instance objects.
 */
typedef struct IIMGENC_Obj {
    struct IIMGENC_Fxns *fxns;
} IIMGENC_Obj;


/**
 *  @brief      Opaque handle to an IIMGENC objects.
 */
typedef struct IIMGENC_Obj  *IIMGENC_Handle;


/**
 *  @brief      Defines the creation time parameters for
 *              all IIMGENC instance objects.
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
typedef struct IIMGENC_Params {
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
    XDAS_Int32 forceChromaFormat;/**< Force encoding in given Chroma format.
                                  *   See #XDM_ChromaFormat.
                                  */
}IIMGENC_Params;

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
typedef struct IIMGENC_DynamicParams {
    XDAS_Int32 size;            /**< Size of this structure in bytes.
                                 *   Because this structure can be extended,
                                 *   this field @b must be correctly set
                                 *   by the caller.
                                 */
    XDAS_Int32 numAU;           /**< Number of Access unit to encode.
                                 *   Set to #XDM_DEFAULT to decode the
                                 *   entire frame.
                                 */
    XDAS_Int32 inputChromaFormat;/**< Input chroma format.
                                 *   See #XDM_ChromaFormat.
                                 */
    XDAS_Int32 inputHeight;     /**< Input height. */
    XDAS_Int32 inputWidth;      /**< Input width. */
    XDAS_Int32 captureWidth;    /**< Pitch.  If set to zero, imageWidth is
                                 *  used; else, this field (assuming it's
                                 *  larger than imageWidth) is used.
                                 */
    XDAS_Int32 generateHeader;  /**< Header generation mode.
                                 *   See #XDM_DecMode.
                                 */
    XDAS_Int32 qValue;          /**< Q value compression factor for encoder. */
} IIMGENC_DynamicParams;

/**
 *  @brief      Defines the input arguments for all IIMGENC instance
 *              process function.
 *
 *  @remarks    This structure may be extended by individual codec
 *              implementation allowing customization with vendor
 *              specific parameters.
 *
 *  @remarks    The @c size field @b must be correctly set by the caller.
 */
typedef struct IIMGENC_InArgs {
    XDAS_Int32 size;            /**< Size of this structure in bytes.
                                 *   Because this structure can be extended,
                                 *   this field @b must be correctly set
                                 *   by the caller.
                                 */
} IIMGENC_InArgs;


/**
 *  @brief      Defines instance status parameters (read-only).

 *  @remarks    This structure may be extended by individual codec
 *              implementation allowing customization with vendor
 *              specific parameters.
 *
 *  @remarks    The @c size field @b must be correctly set by the caller.
 */
typedef struct IIMGENC_Status {
    XDAS_Int32 size;            /**< Size of this structure in bytes.
                                 *   Because this structure can be extended,
                                 *   this field @b must be correctly set
                                 *   by the caller.
                                 */
    XDAS_Int32 extendedError;   /**< @copydoc XDM_ErrorBit. */
    XDAS_Int32 totalAU;         /**< Total number of Access Units. */
    XDM_AlgBufInfo  bufInfo;    /**< Input and output buffer information.
                                 *   See #XDM_AlgBufInfo.
                                 */
} IIMGENC_Status;


/**
 *  @brief      Defines the run time output arguments for
 *              all IIMGENC instance objects.
 *
 *  @remarks    This structure may be extended by individual codec
 *              implementations allowing customization with vendor
 *              specific parameters.
 *
 *  @remarks    The @c size field @b must be correctly set by the caller.
 */
typedef struct IIMGENC_OutArgs {
    XDAS_Int32 size;            /**< Size of this structure in bytes.
                                 *   Because this structure can be extended,
                                 *   this field @b must be correctly set
                                 *   by the caller.
                                 */
    XDAS_Int32 extendedError;   /**< @copydoc XDM_ErrorBit. */
    XDAS_Int32 bytesGenerated;  /**< Number of bytes generated during the
                                 *   process() call.
                                 */
    XDAS_Int32 currentAU;       /**< Current Access Unit Number. */
} IIMGENC_OutArgs;


/**
 *  @brief      Defines the control commands for the IIMGENC module.
 *
 *  @remarks    This ID can be extended in IMOD interface for
 *              additional controls.
 *
 *  @sa         XDM_CmdId
 */
typedef  IALG_Cmd IIMGENC_Cmd;


/**
 *  @brief      Defines all of the operations on IIMGENC objects.
 */
typedef struct IIMGENC_Fxns{
    IALG_Fxns    ialg;

/**
 *  @brief      Basic image encoding call.
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
 *  @retval     IALG_EFAIL      Failure.  See IIMGENC_OutArgs#extendedError
 *                              for more detailed further error conditions.
 */
    XDAS_Int32  (*process)(IIMGENC_Handle handle, XDM_BufDesc *inBufs,
        XDM_BufDesc *outBufs, IIMGENC_InArgs *inArgs,
        IIMGENC_OutArgs *outArgs);

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
 *  @retval     IALG_EFAIL      Failure.  See IIMGENC_Status#extendedError
 *                              for more detailed further error conditions.
 */
    XDAS_Int32  (*control)(IIMGENC_Handle handle, IIMGENC_Cmd id,
        IIMGENC_DynamicParams *params, IIMGENC_Status *status);
} IIMGENC_Fxns;


/*@}*/  /* ingroup */

#ifdef __cplusplus
}
#endif

#endif  /* IIMGENC_ */
/*
 *  @(#) ti.xdais.dm; 1, 0, 1, 0,46; 10-18-2006 19:11:35; /db/wtree/library/trees/dais-g07x/src/
 */

