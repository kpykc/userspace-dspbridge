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
 *  @file       ti/xdais/dm/ividenc1.h
 *
 *  @brief      This header defines all types, constants, and functions
 *              shared by all implementations of the video encoder algorithms.
 */
/**
 *  @defgroup   ti_xdais_dm_IVIDENC1   xDM 1.0 Video Encoder Interface
 *
 *  This is the xDM 1.0 Video Encoder Interface.
 */

#ifndef ti_xdais_dm_IVIDENC1_
#define ti_xdais_dm_IVIDENC1_

#include <ti/xdais/ialg.h>
#include <ti/xdais/xdas.h>
#include "xdm.h"
#include "ivideo.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup    ti_xdais_dm_IVIDENC1 */
/*@{*/

#define IVIDENC1_ERUNTIME  XDM_ERUNTIME        /**< @copydoc XDM_ERUNTIME */
#define IVIDENC1_EOK       XDM_EOK             /**< @copydoc XDM_EOK */
#define IVIDENC1_EFAIL     XDM_EFAIL           /**< @copydoc XDM_EFAIL */
#define IVIDENC1_EUNSUPPORTED XDM_EUNSUPPORTED /**< @copydoc XDM_EUNSUPPORTED */

/**
 *  @brief      This must be the first field of all IVIDENC1
 *              instance objects.
 */
typedef struct IVIDENC1_Obj {
    struct IVIDENC1_Fxns *fxns;
} IVIDENC1_Obj;


/**
 *  @brief      Opaque handle to an IVIDENC1 objects.
 */
typedef struct IVIDENC1_Obj  *IVIDENC1_Handle;


/**
 *  @brief      Defines the creation time parameters for
 *              all IVIDENC1 instance objects.
 *
 *  @extensibleStruct
 */
typedef struct IVIDENC1_Params {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 encodingPreset;  /**< Encoding preset. */
    XDAS_Int32 rateControlPreset;/**< Rate control presets. */
    XDAS_Int32 maxHeight;       /**< Maximum height. */
    XDAS_Int32 maxWidth;        /**< Maximum width. */
    XDAS_Int32 maxFrameRate;    /**< Maximum frame rate. */
    XDAS_Int32 maxBitRate;      /**< Maximum bit rate, bits per second. */
    XDAS_Int32 dataEndianness;  /**< Endianness of output data.
                                 *
                                 *   @sa    XDM_DataFormat
                                 */
    XDAS_Int32 maxInterFrameInterval;/**< I to P frame distance. e.g. = 1 if
                                 *   no B frames, 2 to insert one B frame.
                                 */
    XDAS_Int32 inputChromaFormat;/**< Chroma format for the input buffer.
                                 *
                                 *   @sa XDM_ChromaFormat
                                 */
    XDAS_Int32 inputContentType;/**< Video content type of the buffer being
                                 *   encoded.
                                 *
                                 *   @sa IVIDEO_ContentType
                                 */
    XDAS_Int32 reconChromaFormat;/**< Chroma formats for the reconstruction
                                 *   buffers.
                                 *
                                 *   @remarks   The application should set
                                 *              this to #XDM_CHROMA_NA
                                 *              if reconstruction
                                 *              buffers are not
                                 *              required.
                                 *
                                 *   @sa XDM_ChromaFormat
                                 */
} IVIDENC1_Params;


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
 *  @sa         IVIDENC1_Fxns::control()
 */
typedef struct IVIDENC1_DynamicParams {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 inputHeight;     /**< Input frame height. */
    XDAS_Int32 inputWidth;      /**< Input frame width. */
    XDAS_Int32 refFrameRate;    /**< Reference or input frame rate. */
    XDAS_Int32 targetFrameRate; /**< Target frame rate. */
    XDAS_Int32 targetBitRate;   /**< Target bit rate in bits per second. */
    XDAS_Int32 intraFrameInterval;/**< I frame interval.  For example, 30. */
    XDAS_Int32 generateHeader;  /**< @copydoc XDM_EncMode
                                 *
                                 *   @sa XDM_EncMode
                                 */
    XDAS_Int32 captureWidth;    /**< DEFAULT(0): use imagewidth as
                                 *   pitch else use given capture
                                 *   width for pitch provided it
                                 *   is greater than image width.
                                 */
    XDAS_Int32 forceIFrame;     /**< Force the current frame to be encoded
                                 *   as an I frame.
                                 */
} IVIDENC1_DynamicParams;


/**
 *  @brief      Defines the input arguments for all IVIDENC1 instance
 *              process function.
 *
 *  @extensibleStruct
 *
 *  @sa         IVIDENC1_Fxns::process()
 */
typedef struct IVIDENC1_InArgs {
    XDAS_Int32 size;            /**< @sizeField */
} IVIDENC1_InArgs;


/**
 *  @brief      Defines instance status parameters.
 *
 *  @extensibleStruct
 *
 *  @sa         IVIDENC1_Fxns::control()
 */
typedef struct IVIDENC1_Status {
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
                                 *   @remarks   This field provides the
                                 *              application with the algorithm's
                                 *              buffer requirements.  The
                                 *              requirements may vary depending
                                 *              on the current configuration
                                 *              of the algorithm instance.
                                 *
                                 *   @remarks   When configured to generate
                                 *              reconstruction buffers (see
                                 *      IVIDENC1_Params::reconChromaFormat),
                                 *              the recontruction buffer
                                 *              information will also be
                                 *              reported here.
                                 *
                                 *   @sa        IVIDENC1_Params
                                 *   @sa        XDM_AlgBufInfo
                                 *   @sa        IVIDENC1_Fxns::process()
                                 */
} IVIDENC1_Status;


/**
 *  @brief      Defines the run time output arguments for all IVIDENC1
 *              instance objects.
 *
 *  @extensibleStruct
 *
 *  @sa         IVIDENC1_Fxns::process()
 */
typedef struct IVIDENC1_OutArgs {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 extendedError;   /**< @extendedErrorField */
    XDAS_Int32 bytesGenerated;  /**< Number of bytes generated during the
                                 *   process() call.
                                 */
    XDAS_Int32 encodedFrameType;/**< @copydoc IVIDEO_FrameType
                                 *
                                 *   @sa IVIDEO_FrameType
                                 */
    XDAS_Int32 inputFrameSkip;  /**< @copydoc IVIDEO_SkipMode
                                 *
                                 *   @sa IVIDEO_SkipMode
                                 */
    IVIDEO_BufDesc reconBufs;   /**< Reconstruction frames.
                                 *
                                 *   @remarks   These output buffers
                                 *              correspond to
                                 *              @c outBufs->bufs[1],
                                 *              @c outBufs->bufs[2],
                                 *              and @c outBufs->bufs[3].
                                 *
                                 *   @remarks   @c reconBufs->bufs[0] is
                                 *              equivalent to
                                 *              @c outBufs->bufs[1],
                                 *              @c reconBufs->bufs[1] is
                                 *              equivalent to
                                 *              @c outBufs->bufs[2], and
                                 *              @c reconBufs->bufs[2] is
                                 *              equivalent to
                                 *              @c outBufs->bufs[3].
                                 */
} IVIDENC1_OutArgs;


/**
 *  @brief      Defines the control commands for the IVIDENC1 module.
 *
 *  @remarks    This ID can be extended in IMOD interface for
 *              additional controls.
 *
 *  @sa         XDM_CmdId
 *
 *  @sa         IVIDENC1_Fxns::control()
 */
typedef  IALG_Cmd IVIDENC1_Cmd;


/**
 *  @brief      Defines all of the operations on IVIDENC1 objects.
 */
typedef struct IVIDENC1_Fxns {
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
 *  @pre        @c inArgs must not be NULL, and must point to a valid
 *              IVIDENC1_InArgs structure.
 *
 *  @pre        @c outArgs must not be NULL, and must point to a valid
 *              IVIDENC1_OutArgs structure.
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
 *  @remarks    @c outBufs->bufs[0] will contain the encoded data buffer.
 *
 *  @remarks    @c outBufs->bufs[1], @c outBufs->bufs[2], and
 *              @c outBufs->bufs[3] are used when providing reconstruction
 *              buffers.
 *
 *  @retval     IALG_EOK        Success.
 *  @retval     IALG_EFAIL      Failure.  See IVIDENC1_OutArgs#extendedError
 *                              for more detailed further error conditions.
 */
    XDAS_Int32 (*process)(IVIDENC1_Handle handle, XDM_BufDesc *inBufs,
        XDM_BufDesc *outBufs, IVIDENC1_InArgs *inArgs,
        IVIDENC1_OutArgs *outArgs);

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
 *              IVIDENC1_DynamicParams structure.
 *
 *  @pre        @c status must not be NULL, and must point to a valid
 *              IVIDENC1_Status structure.
 *
 *  @post       The algorithm <b>must not</b> modify the contents of @c params.
 *              That is, the data pointed to by this parameter must be
 *              treated as read-only.
 *
 *  @retval     IALG_EOK        Success.
 *  @retval     IALG_EFAIL      Failure.  See IVIDENC1_Status#extendedError
 *                              for more detailed further error conditions.
 */
    XDAS_Int32 (*control)(IVIDENC1_Handle handle, IVIDENC1_Cmd id,
        IVIDENC1_DynamicParams *params, IVIDENC1_Status *status);

} IVIDENC1_Fxns;


/*@}*/

#ifdef __cplusplus
}
#endif

#endif  /* ti_xdais_dm_IVIDENC1_ */
/*
 *  @(#) ti.xdais.dm; 1, 0, 1, 0,46; 10-18-2006 19:11:35; /db/wtree/library/trees/dais-g07x/src/
 */

