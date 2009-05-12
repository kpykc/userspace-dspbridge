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
 *  @file       ti/xdais/dm/iauddec.h
 *
 *  @brief      This header defines all types, constants, and functions
 *              shared by all implementations of the audio decoder
 *              algorithms.
 */
/**
 *  @defgroup   ti_xdais_dm_IAUDDEC   xDM 0.9 Audio Decoder Interface
 *
 *  This is the xDM 0.9 Audio Decoder Interface.
 */

#ifndef ti_xdais_dm_IAUDDEC_
#define ti_xdais_dm_IAUDDEC_

#include <ti/xdais/ialg.h>
#include <ti/xdais/xdas.h>
#include "xdm.h"
#include "iaudio.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup    ti_xdais_dm_IAUDDEC */
/*@{*/

#define IAUDDEC_CID      "ti.xdais.dm.IAUDDEC"  /**< IAUDDEC Class ID */

#define IAUDDEC_ERUNTIME  XDM_ERUNTIME          /**< @copydoc XDM_ERUNTIME */
#define IAUDDEC_EOK       XDM_EOK               /**< @copydoc XDM_EOK */
#define IAUDDEC_EFAIL     XDM_EFAIL             /**< @copydoc XDM_EFAIL */

/**
 *  @brief      This must be the first field of all IAUDDEC
 *              instance objects.
 */
typedef struct IAUDDEC_Obj {
    struct IAUDDEC_Fxns *fxns;
} IAUDDEC_Obj;


/**
 *  @brief      Opaque handle to an IAUDDEC objects.
 */
typedef struct IAUDDEC_Obj  *IAUDDEC_Handle;


/**
 *  @brief      Defines the creation time parameters for
 *              all IAUDDEC instance objects.
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
typedef struct IAUDDEC_Params {
    XDAS_Int32 size;            /**< Size of this structure in bytes.
                                 *   Because this structure can be extended,
                                 *   this field @b must be correctly set
                                 *   by the caller.
                                 */
    XDAS_Int32 maxSampleRate;   /**< Maximum sampling frequency in Hz.
                                 *   For example, if the maximum sampling
                                 *   frequency is 44.1 kHz, this field should
                                 *   be 44100.
                                 */
    XDAS_Int32 maxBitrate;      /**< Maximum bit rate in bits per second.
                                 *   For example, if the maximum bit rate
                                 *   is 128 kbps, set this field to
                                 *   128000.
                                 */
    XDAS_Int32 maxNoOfCh;       /**< Maximum Channels.
                                 *   See #IAUDIO_ChannelId.
                                 */
    XDAS_Int32 dataEndianness;  /**< Endianness of input data.
                                 *   See #XDM_DataFormat.
                                 */
} IAUDDEC_Params;

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
typedef struct IAUDDEC_DynamicParams {
    XDAS_Int32 size;            /**< Size of this structure in bytes.
                                 *   Because this structure can be extended,
                                 *   this field @b must be correctly set
                                 *   by the caller.
                                 */
    XDAS_Int32 outputFormat;    /**< Output PCM format.
                                 *   See #IAUDIO_PcmFormat.
                                 */
} IAUDDEC_DynamicParams;

/**
 *  @brief      Defines the input arguments for all IAUDDEC instance
 *              process function.
 *
 *  @remarks    This structure may be extended by individual codec
 *              implementation allowing customization with vendor
 *              specific parameters.
 *
 *  @remarks    The @c size field @b must be correctly set by the caller.
 */
typedef struct IAUDDEC_InArgs {
    XDAS_Int32 size;            /**< Size of this structure in bytes.
                                 *   Because this structure can be extended,
                                 *   this field @b must be correctly set
                                 *   by the caller.
                                 */
    XDAS_Int32 numBytes;        /**< Size of valid input data (in bytes) to
                                 *   be processed.
                                 */
} IAUDDEC_InArgs;

/**
 *  @brief      Defines instance status parameters (read-only).
 *
 *  @remarks    This structure may be extended by individual codec
 *              implementation allowing customization with vendor
 *              specific parameters.
 *
 *  @remarks    The @c size field @b must be correctly set by the caller.
 */
typedef struct IAUDDEC_Status {
    XDAS_Int32 size;            /**< Size of this structure in bytes.
                                 *   Because this structure can be extended,
                                 *   this field @b must be correctly set
                                 *   by the caller.
                                 */
    XDAS_Int32 extendedError;   /**< @copydoc XDM_ErrorBit. */
    XDAS_Int32 bitRate;         /**< Average bit rate, in bits per second. */
    XDAS_Int32 sampleRate;      /**< Sampling frequency, in Hz.
                                 *   For example, if the sampling
                                 *   frequency is 44.1 kHz, this field will
                                 *   be 44100.
                                 */
    XDAS_Int32 numChannels;     /**< Number of Channels.
                                 *   See #IAUDIO_ChannelId.
                                 */
    XDAS_Int32 numLFEChannels;  /**< Number of LFE (Low Frequency Effects)
                                 *   channels in the stream.
                                 */
    XDAS_Int32 outputFormat;    /**< The output PCM format.
                                 *   See #IAUDIO_PcmFormat.
                                 */
    XDAS_Int32 autoPosition;    /**< Flag to indicate support for random
                                 *   position decoding, which means that a
                                 *   stream can be decoded from any point.
                                 *   0 indicates this feature is not supported,
                                 *   1 indicates this feature is supported.
                                 */
    XDAS_Int32 fastFwdLen;      /**< Recommended fast forward length in bytes
                                 *   in case of random position decoding.
                                 */
    XDAS_Int32 frameLen;        /**< Number of samples decoded per process()
                                 *   call.
                                 */
    XDAS_Int32 outputBitsPerSample; /**< Number of bits per output sample.
                                 *   For example, if the value of the field is
                                 *   16, there are 16 output bits per PCM
                                 *   sample.
                                 */
    XDM_AlgBufInfo  bufInfo;    /**< Input and output buffer information.
                                 *   See #XDM_AlgBufInfo.
                                 */
} IAUDDEC_Status;


/**
 *  @brief      Defines the run time output arguments for
 *              all IAUDDEC instance objects.
 *
 *  @remarks    This structure may be extended by individual codec
 *              implementations allowing customization with vendor
 *              specific parameters.
 *
 *  @remarks    The @c size field @b must be correctly set by the caller.
 */
typedef struct IAUDDEC_OutArgs {
    XDAS_Int32 size;            /**< Size of this structure in bytes.
                                 *   Because this structure can be extended,
                                 *   this field @b must be correctly set
                                 *   by the caller.
                                 */
    XDAS_Int32 extendedError;   /**< @copydoc XDM_ErrorBit. */
    XDAS_Int32 bytesConsumed;   /**< Number of bytes consumed during the
                                 *   process() call.
                                 */
} IAUDDEC_OutArgs;


/**
 *  @brief      Defines the control commands for the IAUDDEC module.
 *
 *  @remarks    This ID can be extended in IMOD interface for
 *              additional controls.
 *
 *  @sa         XDM_CmdId
 */
typedef  IALG_Cmd IAUDDEC_Cmd;


/**
 *  @brief      Defines all of the operations on IAUDDEC objects.
 */
typedef struct IAUDDEC_Fxns{
    IALG_Fxns   ialg;             /**< Traditional xDAIS algorithm interface. */

/**
 *  @brief      Basic audio decoding call.
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
 *  @retval     IALG_EFAIL      Failure.  See IAUDDEC_OutArgs#extendedError
 *                              for more detailed further error conditions.
 */
    XDAS_Int32   (*process)(IAUDDEC_Handle handle, XDM_BufDesc *inBufs,
        XDM_BufDesc *outBufs, IAUDDEC_InArgs *inArgs,
        IAUDDEC_OutArgs *outArgs);

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
 *  @retval     IALG_EFAIL      Failure.  See IAUDDEC_Status#extendedError
 *                              for more detailed further error conditions.
 */
    XDAS_Int32   (*control)(IAUDDEC_Handle handle, IAUDDEC_Cmd id,
        IAUDDEC_DynamicParams *params, IAUDDEC_Status *status);
} IAUDDEC_Fxns;

/*@}*/

#ifdef __cplusplus
}
#endif

#endif  /* ti_xdais_dm_IAUDDEC_ */
/*
 *  @(#) ti.xdais.dm; 1, 0, 1, 0,46; 10-18-2006 19:11:35; /db/wtree/library/trees/dais-g07x/src/
 */

