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
 *  @file       ividenc.h
 *
 *  @brief      This header defines all types, constants, and functions
 *              shared by all implementations of the video encoder algorithms.
 */
/**
 *  @defgroup   ti_xdais_dm_IVIDENC   xDM 0.9 Video Encoder Interface
 *
 *  This is the xDM 0.9 Video Encoder Interface.
 */

#ifndef IVIDENC_
#define IVIDENC_

#include <ti/xdais/ialg.h>
#include <ti/xdais/xdas.h>
#include "xdm.h"
#include "ivideo.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup    ti_xdais_dm_IVIDENC */
/*@{*/

#define IVIDENC_CID      "ti.xdais.dm.IVIDENC"  /**< IVIDENC Class ID */

#define IVIDENC_ERUNTIME  XDM_ERUNTIME          /**< @copydoc XDM_ERUNTIME */
#define IVIDENC_EOK       XDM_EOK               /**< @copydoc XDM_EOK */
#define IVIDENC_EFAIL     XDM_EFAIL             /**< @copydoc XDM_EFAIL */

/**
 *  @brief      This must be the first field of all IVIDENC
 *              instance objects.
 */
typedef struct IVIDENC_Obj {
    struct IVIDENC_Fxns *fxns;
} IVIDENC_Obj;


/**
 *  @brief      Opaque handle to an IVIDENC objects.
 */
typedef struct IVIDENC_Obj  *IVIDENC_Handle;


/**
 *  @brief      Defines the creation time parameters for
 *              all IVIDENC instance objects.
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
typedef struct IVIDENC_Params {
    XDAS_Int32 size;            /**< Size of this structure in bytes.
                                 *   Because this structure can be extended,
                                 *   this field @b must be correctly set
                                 *   by the caller.
                                 */
    XDAS_Int32 encodingPreset;    /**< Encoding preset. */
    XDAS_Int32 rateControlPreset; /**< Rate control presets. */
    XDAS_Int32 maxHeight;       /**< Maximum height. */
    XDAS_Int32 maxWidth;        /**< Maximum width. */
    XDAS_Int32 maxFrameRate;    /**< Maximum Frame rate * 1000. */
    XDAS_Int32 maxBitRate;      /**< Maximum bit rate, bits per second. */
    XDAS_Int32 dataEndianness;  /**< Endianness of output data.
                                 *   See #XDM_DataFormat.
                                 */
    XDAS_Int32 maxInterFrameInterval;/**< I to P frame distance. e.g. = 1 if
                                      *   no B frames, 2 to insert one B frame.
                                      */
    XDAS_Int32 inputChromaFormat; /**< See #XDM_ChromaFormat. */
    XDAS_Int32 inputContentType; /**< Content Type.
                                  *  See #IVIDEO_ContentType.
                                  */
} IVIDENC_Params;

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
typedef struct IVIDENC_DynamicParams {
    XDAS_Int32 size;            /**< Size of this structure in bytes.
                                 *   Because this structure can be extended,
                                 *   this field @b must be correctly set
                                 *   by the caller.
                                 */
    XDAS_Int32 inputHeight;     /**< Input frame height. */
    XDAS_Int32 inputWidth;      /**< Input frame width. */
    XDAS_Int32 refFrameRate;    /**< Reference or input frame rate * 1000. */
    XDAS_Int32 targetFrameRate; /**< Target frame rate * 1000. */
    XDAS_Int32 targetBitRate;   /**< Target bit rate in bits per second. */
    XDAS_Int32 intraFrameInterval;/**< I frame interval.  For example,
                                 *   30.
                                 */
    XDAS_Int32 generateHeader;  /**< #XDM_ENCODE_AU,
                                 *   #XDM_GENERATE_HEADER
                                 */
    XDAS_Int32 captureWidth;    /**< DEFAULT(0): use imagewidth as
                                 *   pitch else use given capture
                                 *   width for pitch provided it
                                 *   is greater than image width.
                                 */
    XDAS_Int32 forceIFrame;     /**< Force the current frame to be encoded
                                 *   as an I-frame. */
} IVIDENC_DynamicParams;

/**
 *  @brief      Defines the input arguments for all IVIDDEC instance
 *              process function.
 *
 *  @remarks    This structure may be extended by individual codec
 *              implementation allowing customization with vendor
 *              specific parameters.
 *
 *  @remarks    The @c size field @b must be correctly set by the caller.
 */
typedef struct IVIDENC_InArgs {
    XDAS_Int32 size;            /**< Size of this structure in bytes.
                                 *   Because this structure can be extended,
                                 *   this field @b must be correctly set
                                 *   by the caller.
                                 */
} IVIDENC_InArgs;

/**
 *  @brief      Defines instance status parameters (read-only).
 *
 *  @remarks    This structure may be extended by individual codec
 *              implementation allowing customization with vendor
 *              specific parameters.
 *
 *  @remarks    The @c size field @b must be correctly set by the caller.
 */
typedef struct IVIDENC_Status {
    XDAS_Int32 size;            /**< Size of this structure in bytes.
                                 *   Because this structure can be extended,
                                 *   this field @b must be correctly set
                                 *   by the caller.
                                 */
    XDAS_Int32 extendedError;   /**< @copydoc XDM_ErrorBit. */
    XDM_AlgBufInfo bufInfo;     /**< Input & output buffer information.
                                 *   See #XDM_AlgBufInfo.
                                 */
} IVIDENC_Status;

/**
 *  @brief      Defines the run time output arguments for all IVIDENC
 *              instance objects.
 *
 *  @remarks    This structure may be extended by individual codec
 *              implementations allowing customization with vendor
 *              specific parameters.
 *
 *  @remarks    The @c size field @b must be correctly set by the caller.
 */
typedef struct IVIDENC_OutArgs {
    XDAS_Int32 size;            /**< Size of this structure in bytes.
                                 *   Because this structure can be extended,
                                 *   this field @b must be correctly set
                                 *   by the caller.
                                 */
    XDAS_Int32 extendedError;   /**< @copydoc XDM_ErrorBit. */
    XDAS_Int32 bytesGenerated;  /**< Bytes generated by the encode. */
    XDAS_Int32 encodedFrameType; /**< Frame Types.  See #IVIDEO_FrameType. */
    XDAS_Int32 inputFrameSkip;  /**< Frame Skip Mode.  See #IVIDEO_SkipMode. */
    IVIDEO_BufDesc reconBufs;   /**< Reconstruction Frames. */
} IVIDENC_OutArgs;


/**
 *  @brief      Defines the control commands for the IVIDENC module.
 *
 *  @remarks    This ID can be extended in IMOD interface for
 *              additional controls.
 *
 *  @sa         XDM_CmdId
 */
typedef  IALG_Cmd IVIDENC_Cmd;


/**
 *  @brief      Defines all of the operations on IVIDENC objects.
 */
typedef struct IVIDENC_Fxns {
    IALG_Fxns   ialg;             /**< Traditional xDAIS algorithm interface. */

/**
 *  @brief      Basic video encoding call.
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
 *  @retval     IALG_EFAIL      Failure.  See IVIDENC_OutArgs#extendedError
 *                              for more detailed further error conditions.
 */
    XDAS_Int32 (*process)(IVIDENC_Handle handle, XDM_BufDesc *inBufs,
        XDM_BufDesc *outBufs, IVIDENC_InArgs *inArgs,
        IVIDENC_OutArgs *outArgs);

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
 *  @retval     IALG_EFAIL      Failure.  See IVIDENC_Status#extendedError
 *                              for more detailed further error conditions.
 */
    XDAS_Int32 (*control)(IVIDENC_Handle handle, IVIDENC_Cmd id,
        IVIDENC_DynamicParams *params, IVIDENC_Status *status);
} IVIDENC_Fxns;


/*@}*/  /* ingroup */

#ifdef __cplusplus
}
#endif

#endif  /* IVIDENC_ */
/*
 *  @(#) ti.xdais.dm; 1, 0, 1, 0,46; 10-18-2006 19:11:35; /db/wtree/library/trees/dais-g07x/src/
 */

