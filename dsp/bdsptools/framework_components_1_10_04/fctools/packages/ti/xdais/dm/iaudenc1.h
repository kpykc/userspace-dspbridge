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
 *  @file       ti/xdais/dm/iaudenc1.h
 *
 *  @brief      This header defines all types, constants, and functions
 *              shared by all implementations of the audio encoder
 *              algorithms.
 */
/**
 *  @defgroup   ti_xdais_dm_IAUDENC1   xDM 1.0 Audio Encoder Interface
 *
 *  This is the xDM 1.0 Audio Encoder Interface.
 */

#ifndef ti_xdais_dm_IAUDENC1_
#define ti_xdais_dm_IAUDENC1_

#include <ti/xdais/ialg.h>
#include <ti/xdais/xdas.h>
#include "xdm.h"
#include "iaudio.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup    ti_xdais_dm_IAUDENC1 */
/*@{*/

#define IAUDENC1_ERUNTIME XDM_ERUNTIME         /**< @copydoc XDM_ERUNTIME */
#define IAUDENC1_EOK      XDM_EOK              /**< @copydoc XDM_EOK */
#define IAUDENC1_EFAIL    XDM_EFAIL            /**< @copydoc XDM_EFAIL */
#define IAUDENC1_EUNSUPPORTED XDM_EUNSUPPORTED /**< @copydoc XDM_EUNSUPPORTED */

/**
 *  @brief      This must be the first field of all IAUDENC1
 *              instance objects.
 */
typedef struct IAUDENC1_Obj {
    struct IAUDENC1_Fxns *fxns;
} IAUDENC1_Obj;


/**
 *  @brief      Opaque handle to an IAUDENC1 objects.
 */
typedef struct IAUDENC1_Obj  *IAUDENC1_Handle;


/**
 *  @brief      Defines the creation time parameters for
 *              all IAUDENC1 instance objects.
 *
 *  @extensibleStruct
 */
typedef struct IAUDENC1_Params {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 sampleRate;      /**< Sampling Frequency in Hz.
                                 *   For example, if the sampling
                                 *   frequency is 44.1 kHz, this field will
                                 *   be 44100.
                                 *
                                 *   @sa    IAUDENC1_DynamicParams.sampleRate
                                 *   @sa    IAUDENC1_Status.sampleRate
                                 */
    XDAS_Int32 bitRate;         /**< Average bit rate, in bits per second.
                                 *
                                 *   @sa    IAUDENC1_DynamicParams.bitRate
                                 *   @sa    IAUDENC1_Status.bitRate
                                 */
    XDAS_Int32 channelMode;     /**< Input Channel Mode.
                                 *
                                 *   @sa    IAUDIO_ChannelMode
                                 *   @sa    IAUDENC1_DynamicParams.channelMode
                                 */
    XDAS_Int32 dataEndianness;  /**< Endianness of output data.
                                 *
                                 *   @sa    XDM_DataFormat
                                 */
    XDAS_Int32 encMode;         /**< Encoding mode.
                                 *
                                 *   @sa    IAUDIO_EncMode
                                 *   @sa    IAUDENC1_Status.encMode
                                 */
    XDAS_Int32 inputFormat;     /**< Input format block/interleaved.
                                 *
                                 *   @sa    IAUDIO_PcmFormat
                                 */
    XDAS_Int32 inputBitsPerSample;/**< Number of input bits per output sample.
                                 *   For example, if there are 16 bits per
                                 *   PCM input sample, this should be set to 16.
                                 */
    XDAS_Int32 maxBitRate;      /**< Maximum bit rate in case of VBR.  Some
                                 *   of the encoders allow you to specify
                                 *   max bit rate in case of VBR mode
                                 */
    XDAS_Int32 dualMonoMode;    /**< Mode to indicate type of Dual Mono.
                                 *   Applicable only if channelMode is
                                 *   dualMono
                                 *
                                 *   @sa IAUDIO_DualMonoMode
                                 */
    XDAS_Int32 crcFlag;         /**< Flag indicating whether the encoder
                                 *   should insert CRC bits into the
                                 *   bitstream or not.
                                 *
                                 *   Valid values for this field are XDAS_TRUE
                                 *   and XDAS_FALSE.
                                 */
    XDAS_Int32 ancFlag;         /**< Ancillary Data Flag.
                                 *
                                 *   @remarks   This flag indicates to the
                                 *              encoder that Ancillary data
                                 *              needs to be encoded along with
                                 *              the normal audio data.  The
                                 *              ancillary data can be provided
                                 *              for each input frame.
                                 *
                                 *   Valid values for this field are XDAS_TRUE
                                 *   and XDAS_FALSE.
                                 */
    XDAS_Int32 lfeFlag;         /**< Flag indicating whether LFE channel data
                                 *   is present or not in the input.
                                 *
                                 *   Valid values for this field are XDAS_TRUE
                                 *   and XDAS_FALSE.
                                 */
} IAUDENC1_Params;


/**
 *  @brief      This structure defines the algorithm parameters that can be
 *              modified after creation via control() calls.
 *
 *  @remarks    It is not necessary that given implementation support all
 *              dynamic parameters to be configurable at run time.  If a
 *              particular algorithm does not support run-time updates to
 *              a parameter that the application is attempting to change
 *              at runtime, it may indicate this as an error.
 *
 *  @extensibleStruct
 *
 *  @sa         IAUDENC1_Fxns::control()
 */
typedef struct IAUDENC1_DynamicParams {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 bitRate;         /**< Average bit rate, in bits per second.
                                 *
                                 *   @sa    IAUDENC1_Params.bitRate
                                 *   @sa    IAUDENC1_Status.bitRate
                                 */
    XDAS_Int32 sampleRate;      /**< Sampling frequency in Hz.
                                 *   For example, if the sampling
                                 *   frequency is 44.1 kHz, this field will
                                 *   be 44100.
                                 *
                                 *   @sa    IAUDENC1_Params.sampleRate
                                 *   @sa    IAUDENC1_Status.sampleRate
                                 */
    XDAS_Int32 channelMode;     /**< Input Channel Mode.
                                 *
                                 *   @sa    IAUDIO_ChannelMode
                                 *   @sa    IAUDENC1_Params.channelMode
                                 */
    XDAS_Int32 lfeFlag;         /**< Flag indicating whether LFE channel data
                                 *   is present or not in the input.
                                 *
                                 *   Valid values for this field are XDAS_TRUE
                                 *   and XDAS_FALSE.
                                 */
    XDAS_Int32 dualMonoMode;    /**< Mode to indicate type of Dual Mono.
                                 *   Applicable only if channelMode is
                                 *   dualMono
                                 *
                                 *   @sa IAUDIO_DualMonoMode
                                 */
} IAUDENC1_DynamicParams;


/**
 *  @brief      Defines the input arguments for all IAUDENC1 instance
 *              process function.
 *
 *  @extensibleStruct
 *
 *  @sa         IAUDENC1_Fxns::process()
 */
typedef struct IAUDENC1_InArgs {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 numInSamples;    /**< Number of Input Samples per Channel */
    XDM_SingleBufDesc ancData;  /**< Ancillary Data.
                                 *
                                 *   @remarks   Ancillary data contains any
                                 *              extra information to be
                                 *              encoded along with the PCM
                                 *              samples.
                                 */
} IAUDENC1_InArgs;


/**
 *  @brief      Defines instance status parameters (read-only).
 *
 *  @extensibleStruct
 *
 *  @sa         IAUDENC1_Fxns::control()
 */
typedef struct IAUDENC1_Status {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 extendedError;   /**< @extendedErrorField */
    XDM_SingleBufDesc data;     /**< Buffer descriptor for data passing.
                                 *
                                 *   @remarks   This buffer can be used as
                                 *              either input or output,
                                 *              depending on the command.
                                 *
                                 *   @remarks   The buffer will be provided
                                 *              by the application, and
                                 *              returned to the application
                                 *              upon return of the control()
                                 *              call.  The algorithm must
                                 *              not retain a pointer to this
                                 *              data.
                                 *
                                 *   @sa    #XDM_GETVERSION
                                 */
    XDM_AlgBufInfo bufInfo;     /**< Input and output buffer information.
                                 *
                                 *   @sa    XDM_AlgBufInfo
                                 */
    XDAS_Int32 validFlag;       /**< Flag indicating the validity of the
                                 *   status struct.
                                 *
                                 *   Valid values for this field are XDAS_TRUE
                                 *   and XDAS_FALSE.
                                 */
    XDAS_Int32 lfeFlag;         /**< Flag indicating whether LFE channel data
                                 *   is present or not in the input.
                                 *
                                 *   Valid values for this field are XDAS_TRUE
                                 *   and XDAS_FALSE.
                                 */
    XDAS_Int32 bitRate;         /**< Average bit rate, in bits per second.
                                 *
                                 *   @sa    IAUDENC1_Params.bitRate
                                 *   @sa    IAUDENC1_DynamicParams.bitRate
                                 */
    XDAS_Int32 sampleRate;      /**< Sampling frequency in Hz.
                                 *   For example, if the sampling
                                 *   frequency is 44.1 kHz, this field will
                                 *   be 44100.
                                 *
                                 *   @sa    IAUDENC1_Params.sampleRate
                                 *   @sa    IAUDENC1_DynamicParams.sampleRate
                                 */
    XDAS_Int32 channelMode;     /**< Input Channel Mode.
                                 *
                                 *   @sa    IAUDIO_ChannelMode for valid values.
                                 *   @sa    IAUDENC1_Params.channelMode
                                 *   @sa    IAUDENC1_DynamicParams.channelMode
                                 */
    XDAS_Int32 encMode;         /**< Encoding mode.
                                 *
                                 *   @sa    IAUDIO_EncMode
                                 *   @sa    IAUDENC1_Params.encMode
                                 */
} IAUDENC1_Status;


/**
 *  @brief      Defines the run time output arguments for all IAUDENC1
 *              instance objects.
 *
 *  @extensibleStruct
 *
 *  @sa         IAUDENC1_Fxns::process()
 */
typedef struct IAUDENC1_OutArgs {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 extendedError;   /**< @extendedErrorField */
    XDAS_Int32 bytesGenerated;  /**< Number of bytes generated during the
                                 *   process() call.
                                 */
    XDAS_Int32 numZeroesPadded; /**< Number of zeroes padded to input.
                                 *
                                 *   @remarks   Some of the encoders pad
                                 *              zeroes at the end of the
                                 *              stream to complete the last
                                 *              frame.
                                 */
} IAUDENC1_OutArgs;


/**
 *  @brief      Defines the control commands for the IAUDENC1 module.
 *
 *  @remarks    This ID can be extended in IMOD interface for
 *              additional controls.
 *
 *  @sa         XDM_CmdId
 *
 *  @sa         IAUDENC1_Fxns::control()
 */
typedef  IALG_Cmd IAUDENC1_Cmd;


/**
 *  @brief      Defines all of the operations on IAUDENC1 objects.
 */
typedef struct IAUDENC1_Fxns{
    IALG_Fxns   ialg;             /**< Traditional xDAIS algorithm interface. */

/**
 *  @brief      Basic audio encoding call.
 *
 *  @param[in]  handle          Handle to an algorithm instance.
 *  @param[in]  inBufs          Input buffer descriptors.
 *  @param[in,out] outBufs      Output buffer descriptors.  The algorithm
 *                              may modify the output buffer pointers.
 *  @param[in]  inArgs          Input arguments.  This is a required
 *                              parameter.
 *  @param[out] outArgs         Ouput results.  This is a required parameter.
 *
 *  @pre        @c inArgs must not be NULL, and must point to a valid
 *              IAUDENC1_InArgs structure.
 *
 *  @pre        @c outArgs must not be NULL, and must point to a valid
 *              IAUDENC1_OutArgs structure.
 *
 *  @pre        @c inBufs must not be NULL, and must point to a valid
 *              XDM_BufDesc structure.
 *
 *  @pre        @c inBufs.buf[0] must not be NULL, and must point to a valid
 *              buffer of data that is at least @c inBufs.bufSizes[0] bytes in
 *              length.
 *
 *  @pre        @c outBufs must not be NULL, and must point to a valid
 *              XDM_BufDesc structure.
 *
 *  @pre        @c outBufs.buf[0] must not be NULL, and must point to a valid
 *              buffer of data that is at least @c outBufs.bufSizes[0] bytes in
 *              length.
 *
 *  @post       The algorithm <b>must not</b> modify the contents of
 *              @c inBufs or @c inArgs.
 *              That is, the data and buffers pointed to by these parameters
 *              must be treated as read-only.
 *
 *  @retval     IALG_EOK        Success.
 *  @retval     IALG_EFAIL      Failure.  See IAUDENC1_OutArgs#extendedError
 *                              for more detailed further error conditions.
 */
    XDAS_Int32  (*process)(IAUDENC1_Handle handle, XDM_BufDesc *inBufs,
        XDM_BufDesc *outBufs, IAUDENC1_InArgs *inArgs,
        IAUDENC1_OutArgs *outArgs);

/**
 *  @brief      Control behaviour of an algorithm.
 *
 *  @param[in]  handle          Handle to an algorithm instance.
 *  @param[in]  id              Command id.  See #XDM_CmdId.
 *  @param[in]  params          Dynamic parameters.  This is a required
 *                              parameter.
 *  @param[out] status          Output results.  This is a required parameter.
 *
 *  @pre        @c params must not be NULL, and must point to a valid
 *              IAUDENC1_DynamicParams structure.
 *
 *  @pre        @c status must not be NULL, and must point to a valid
 *              IAUDENC1_Status structure.
 *
 *  @post       The algorithm <b>must not</b> modify the contents of @c params.
 *              That is, the data pointed to by this parameter must be
 *              treated as read-only.
 *
 *  @retval     IALG_EOK        Success.
 *  @retval     IALG_EFAIL      Failure.  See IAUDENC1_Status#extendedError
 *                              for more detailed further error conditions.
 */
    XDAS_Int32  (*control)(IAUDENC1_Handle handle, IAUDENC1_Cmd id,
        IAUDENC1_DynamicParams *params, IAUDENC1_Status *status);
} IAUDENC1_Fxns;


/*@}*/  /* ingroup */

#ifdef __cplusplus
}
#endif

#endif  /* ti_xdais_dm_IAUDENC1_ */
/*
 *  @(#) ti.xdais.dm; 1, 0, 1, 0,46; 10-18-2006 19:11:35; /db/wtree/library/trees/dais-g07x/src/
 */

