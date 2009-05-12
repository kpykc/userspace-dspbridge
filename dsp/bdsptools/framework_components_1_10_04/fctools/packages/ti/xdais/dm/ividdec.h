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
 *  @file       ti/xdais/dm/ividdec.h
 *
 *  @brief      This header defines all types, constants, and functions
 *              shared by all implementations of the video decoder
 *              algorithms.
 */
/**
 *  @defgroup   ti_xdais_dm_IVIDDEC   xDM 0.9 Video Decoder Interface
 *
 *  This is the 0.9 xDM Video Decoder Interface.
 */

#ifndef ti_xdais_dm_IVIDDEC_
#define ti_xdais_dm_IVIDDEC_

#include <ti/xdais/ialg.h>
#include <ti/xdais/xdas.h>
#include "xdm.h"
#include "ivideo.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup    ti_xdais_dm_IVIDDEC */
/*@{*/

#define IVIDDEC_CID      "ti.xdais.dm.IVIDDEC"  /**< IVIDDEC Class ID */

#define IVIDDEC_ERUNTIME  XDM_ERUNTIME          /**< @copydoc XDM_ERUNTIME */
#define IVIDDEC_EOK       XDM_EOK               /**< @copydoc XDM_EOK */
#define IVIDDEC_EFAIL     XDM_EFAIL             /**< @copydoc XDM_EFAIL */

/**
 *  @brief      This must be the first field of all IVIDDEC
 *              instance objects.
 */
typedef struct IVIDDEC_Obj {
    struct IVIDDEC_Fxns *fxns;
} IVIDDEC_Obj;


/**
 *  @brief      Opaque handle to an IVIDDEC objects.
 */
typedef struct IVIDDEC_Obj  *IVIDDEC_Handle;


/**
 *  @brief      Defines the creation time parameters for
 *              all IVIDDEC instance objects.
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
typedef struct IVIDDEC_Params {
    XDAS_Int32 size;            /**< Size of this structure in bytes.
                                 *   Because this structure can be extended,
                                 *   this field @b must be correctly set
                                 *   by the caller.
                                 */
    XDAS_Int32 maxHeight;       /**< Maximum video height in pixels. */
    XDAS_Int32 maxWidth;        /**< Maximum video width in pixels. */
    XDAS_Int32 maxFrameRate;    /**< Maximum frame rate in fps * 1000. */
    XDAS_Int32 maxBitRate;      /**< Maximum bit rate, bits per second.
                                 *   For example, if bit rate is 10 Mbps, set
                                 *   this field to 10000000
                                 */
    XDAS_Int32 dataEndianness;  /**< Endianness of input data.
                                 *   See #XDM_DataFormat.
                                 */
    XDAS_Int32 forceChromaFormat;/**< Force decode in given Chroma format.
                                  *   See #XDM_ChromaFormat.
                                  */
} IVIDDEC_Params;


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
typedef struct IVIDDEC_DynamicParams {
    XDAS_Int32 size;            /**< Size of this structure in bytes.
                                 *   Because this structure can be extended,
                                 *   this field @b must be correctly set
                                 *   by the caller.
                                 */
    XDAS_Int32 decodeHeader;    /**< Number of access unites to decode.
                                 *   See #XDM_DecMode
                                 */
    XDAS_Int32 displayWidth;    /**< Pitch.  If set to 0, use the decoded
                                 *   image width.  Else, use given display
                                 *   width in pixels.
                                 */
    XDAS_Int32 frameSkipMode;   /**< Frame skip mode.
                                 *   See #IVIDEO_FrameSkip.
                                 */
} IVIDDEC_DynamicParams;


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
typedef struct IVIDDEC_InArgs {
    XDAS_Int32 size;            /**< Size of this structure in bytes.
                                 *   Because this structure can be extended,
                                 *   this field @b must be correctly set
                                 *   by the caller.
                                 */
    XDAS_Int32 numBytes;        /**< Size of input data in bytes, provided
                                 *   to the algorithm for decoding.
                                 */
    XDAS_Int32 inputID;         /**< Application will pass this ID to
                                 *   algorithm and decoder will attach
                                 *   this ID with corresponding output
                                 *   frames.  This is useful in case
                                 *   re-ordering (e.g. B frames).  If there
                                 *   is no re-ordering,
                                 *   IVIDDEC_OutArgs#outputID will be same
                                 *   as this inputID field.
                                 */
} IVIDDEC_InArgs;


/**
 *  @brief      Defines instance status parameters (read-only).
 *
 *  @remarks    This structure may be extended by individual codec
 *              implementation allowing customization with vendor
 *              specific parameters.
 *
 *  @remarks    The @c size field @b must be correctly set by the caller.
 */
typedef struct IVIDDEC_Status {
    XDAS_Int32 size;            /**< Size of this structure in bytes.
                                 *   Because this structure can be extended,
                                 *   this field @b must be correctly set
                                 *   by the caller.
                                 */
    XDAS_Int32 extendedError;   /**< @copydoc XDM_ErrorBit. */
    XDAS_Int32 outputHeight;    /**< Output height in pixels. */
    XDAS_Int32 outputWidth;     /**< Output width in pixels. */
    XDAS_Int32 frameRate;       /**< Average frame rate in fps * 1000. */
    XDAS_Int32 bitRate;         /**< Average bit rate in bits/second. */
    XDAS_Int32 contentType;     /**< Video Content type.
                                 * See #IVIDEO_ContentType.
                                 */
    XDAS_Int32 outputChromaFormat; /**< Chroma format of output.
                                 *   See #XDM_ChromaFormat.
                                 */
    XDM_AlgBufInfo  bufInfo;    /**< Input and output buffer information.
                                 *   See #XDM_AlgBufInfo.
                                 */
} IVIDDEC_Status;


/**
 *  @brief      Defines the run time output arguments for
 *              all IVIDDEC instance objects.
 *
 *  @remarks    This structure may be extended by individual codec
 *              implementations allowing customization with vendor
 *              specific parameters.
 *
 *  @remarks    The @c size field @b must be correctly set by the caller.
 */
typedef struct IVIDDEC_OutArgs {
    XDAS_Int32 size;            /**< Size of this structure in bytes.
                                 *   Because this structure can be extended,
                                 *   this field @b must be correctly set
                                 *   by the caller.
                                 */
    XDAS_Int32 extendedError;   /**< @copydoc XDM_ErrorBit. */
    XDAS_Int32 bytesConsumed;   /**< Bytes consumed. */
    XDAS_Int32 decodedFrameType;/**< Decoded frame type.
                                 *   See #IVIDEO_FrameType. */
    XDAS_Int32 outputID;        /**< Output ID.  Refer to comment
                                 *   IVIDDEC_InArgs#inputID.
                                 */
    IVIDEO_BufDesc displayBufs; /**< The decoder fills this structure
                                 *   to denote the buffer pointers for
                                 *   current displayable frames.  In
                                 *   case of sequences having I & P
                                 *   frames only, these values are
                                 *   identical to outBufs passed in
                                 *   process().
                                 */
} IVIDDEC_OutArgs;


/**
 *  @brief      Defines the control commands for the IVIDDEC module.
 *
 *  @remarks    This ID can be extended in IMOD interface for
 *              additional controls.
 *
 *  @sa         XDM_CmdId
 */
typedef  IALG_Cmd IVIDDEC_Cmd;


/**
 *  @brief      Defines all of the operations on IVIDDEC objects.
 */
typedef struct IVIDDEC_Fxns {
    IALG_Fxns   ialg;             /**< Traditional xDAIS algorithm interface. */

/**
 *  @brief      Basic video decoding call.
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
 *  @retval     IALG_EFAIL      Failure.  See IVIDDEC_OutArgs#extendedError
 *                              for more detailed further error conditions.
 */
    XDAS_Int32 (*process)(IVIDDEC_Handle handle, XDM_BufDesc *inBufs,
        XDM_BufDesc *outBufs, IVIDDEC_InArgs *inArgs,
        IVIDDEC_OutArgs *outArgs);

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
 *  @retval     IALG_EFAIL      Failure.  See IVIDDEC_Status#extendedError
 *                              for more detailed further error conditions.
 */
    XDAS_Int32 (*control)(IVIDDEC_Handle handle, IVIDDEC_Cmd id,
        IVIDDEC_DynamicParams *params, IVIDDEC_Status *status);
} IVIDDEC_Fxns;


/*@}*/  /* ingroup */

#ifdef __cplusplus
}
#endif

#endif  /* ti_xdais_dm_IVIDDEC_ */
/*
 *  @(#) ti.xdais.dm; 1, 0, 1, 0,46; 10-18-2006 19:11:35; /db/wtree/library/trees/dais-g07x/src/
 */

