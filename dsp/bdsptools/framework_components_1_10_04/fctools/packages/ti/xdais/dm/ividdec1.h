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
 *  @file       ti/xdais/dm/ividdec1.h
 *
 *  @brief      This header defines all types, constants, and functions
 *              shared by all implementations of the video decoder
 *              algorithms.
 */
/**
 *  @defgroup   ti_xdais_dm_IVIDDEC1   xDM 1.0 Video Decoder Interface
 *
 *  This is the xDM 1.0 Video Decoder Interface.
 */

#ifndef ti_xdais_dm_IVIDDEC1_
#define ti_xdais_dm_IVIDDEC1_

#include <ti/xdais/ialg.h>
#include <ti/xdais/xdas.h>
#include "xdm.h"
#include "ivideo.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup    ti_xdais_dm_IVIDDEC1 */
/*@{*/

#define IVIDDEC1_ERUNTIME  XDM_ERUNTIME        /**< @copydoc XDM_ERUNTIME */
#define IVIDDEC1_EOK       XDM_EOK             /**< @copydoc XDM_EOK */
#define IVIDDEC1_EFAIL     XDM_EFAIL           /**< @copydoc XDM_EFAIL */
#define IVIDDEC1_EUNSUPPORTED XDM_EUNSUPPORTED /**< @copydoc XDM_EUNSUPPORTED */

/**
 *  @brief      This must be the first field of all IVIDDEC1
 *              instance objects.
 */
typedef struct IVIDDEC1_Obj {
    struct IVIDDEC1_Fxns *fxns;
} IVIDDEC1_Obj;


/**
 *  @brief      Opaque handle to an IVIDDEC1 objects.
 */
typedef struct IVIDDEC1_Obj  *IVIDDEC1_Handle;


/**
 *  @brief      Video decoder output frame order.
 */
typedef enum {
    IVIDDEC_DISPLAY_ORDER = 0,  /**< The decoder provides decoded output in
                                 *   in the actual order of displaying the
                                 *   output buffer.  The codec assumes the
                                 *   responsibility of reordering the frames.
                                 *
                                 *   @remarks   The output buffer will be
                                 *              delayed by one frame,
                                 *              regardless of whether the frame
                                 *              contains I/P or I/P/B frames.
                                 *
                                 *   @remarks   This is the default mode.
                                 *
                                 *   @remarks   This mode is required to be
                                 *              supported by all video decoder
                                 *              codecs.
                                 */
    IVIDDEC_DECODE_ORDER = 1    /**< The decoder provides decoded output in the
                                 *   the order of decoding.  There will be no
                                 *   delay in the output buffers.
                                 *
                                 *   @remarks   It is the application's
                                 *              responsibility to handle the
                                 *              frame re-ordering.
                                 *
                                 *   @remarks   This mode is optional.  If it
                                 *              is not supported by the
                                 *              decoder, IVIDDEC_EUNSUPPORTED
                                 *              will be returned.
                                 */
} IVIDDEC1_FrameOrder;


/**
 *  @brief      Defines the creation time parameters for
 *              all IVIDDEC1 instance objects.
 *
 *  @extensibleStruct
 */
typedef struct IVIDDEC1_Params {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 maxHeight;       /**< Maximum video height in pixels. */
    XDAS_Int32 maxWidth;        /**< Maximum video width in pixels. */
    XDAS_Int32 maxFrameRate;    /**< Maximum frame rate in fps. */
    XDAS_Int32 maxBitRate;      /**< Maximum bit rate, bits per second.
                                 *   For example, if bit rate is 10 Mbps, set
                                 *   this field to 10000000
                                 */
    XDAS_Int32 dataEndianness;  /**< Endianness of output data.
                                 *
                                 *   @sa    XDM_DataFormat
                                 */
    XDAS_Int32 forceChromaFormat;/**< @copydoc XDM_ChromaFormat
                                 *
                                 *   @sa XDM_ChromaFormat.
                                 */
} IVIDDEC1_Params;


/**
 *  @brief      This structure defines the codec parameters that can be
 *              modified after creation via control() calls.
 *
 *  @remarks    It is not necessary that a given implementation support all
 *              dynamic parameters to be configurable at run time.  If a
 *              particular algorithm does not support run-time updates to
 *              a parameter that the application is attempting to change
 *              at runtime, it may indicate this as an error.
 *
 *  @extensibleStruct
 *
 *  @sa         IVIDDEC1_Fxns::control()
 */
typedef struct IVIDDEC1_DynamicParams {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 decodeHeader;    /**< @copydoc XDM_DecMode
                                 *
                                 *   @sa XDM_DecMode
                                 */
    XDAS_Int32 displayWidth;    /**< Pitch.  If set to zero, use the decoded
                                 *   image width.  Else, use given display
                                 *   width in pixels.
                                 */
    XDAS_Int32 frameSkipMode;   /**< @copydoc IVIDEO_FrameSkip
                                 *
                                 *   @sa IVIDEO_FrameSkip
                                 */
    XDAS_Int32 frameOrder;      /**< @copydoc IVIDDEC1_FrameOrder
                                 *
                                 *   @sa IVIDDEC1_FrameOrder
                                 */
} IVIDDEC1_DynamicParams;


/**
 *  @brief      Defines the input arguments for all IVIDDEC1 instance
 *              process function.
 *
 *  @extensibleStruct
 *
 *  @sa         IVIDDEC1_Fxns::process()
 */
typedef struct IVIDDEC1_InArgs {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 numBytes;        /**< Size of input data in bytes, provided
                                 *   to the algorithm for decoding.
                                 */
    XDAS_Int32 inputID;         /**< The decoder will attach
                                 *   this ID with the corresponding output
                                 *   frames.
                                 *
                                 *   @remarks   This is useful when frames
                                 *   require re-ordering (e.g. B frames).
                                 *
                                 *   @remarks   When there is no re-ordering,
                                 *   IVIDDEC1_OutArgs#outputID will be same
                                 *   as this inputID field.
                                 *
                                 *   @remarks   Zero (0) is not a supported
                                 *              inputID.  This value is
                                 *              reserved for cases when there
                                 *              is no output buffer provided in
                                 *              IVIDDEC1_OutArgs::displayBufs.
                                 *
                                 *   @sa IVIDDEC1_OutArgs::outputID.
                                 */
} IVIDDEC1_InArgs;


/**
 *  @brief      Defines instance status parameters.
 *
 *  @extensibleStruct
 *
 *  @sa         IVIDDEC1_Fxns::control()
 */
typedef struct IVIDDEC1_Status {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 extendedError;   /**< @extendedErrorField */
    XDM_SingleBufDesc data;     /**< Buffer descriptor for
                                 *   data passing.
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
    XDAS_Int32 outputHeight;    /**< Output height in pixels. */
    XDAS_Int32 outputWidth;     /**< Output width in pixels. */
    XDAS_Int32 frameRate;       /**< Average frame rate in fps. */
    XDAS_Int32 bitRate;         /**< Average bit rate, in bits per second. */
    XDAS_Int32 contentType;     /**< @copydoc IVIDEO_ContentType
                                 *
                                 *   @sa IVIDEO_ContentType
                                 */
    XDAS_Int32 outputChromaFormat; /**< @copydoc XDM_ChromaFormat
                                 *
                                 *   @sa XDM_ChromaFormat
                                 */
    XDM_AlgBufInfo  bufInfo;    /**< Input and output buffer information.
                                 *
                                 *   @sa    XDM_AlgBufInfo
                                 */
} IVIDDEC1_Status;


/**
 *  @brief      Defines the run time output arguments for
 *              all IVIDDEC1 instance objects.
 *
 *  @extensibleStruct
 *
 *  @sa         IVIDDEC1_Fxns::process()
 */
typedef struct IVIDDEC1_OutArgs {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 extendedError;   /**< @extendedErrorField */
    XDAS_Int32 bytesConsumed;   /**< Number of bytes consumed during the
                                 *   process() call.
                                 */
    XDAS_Int32 decodedFrameType;/**< @copydoc IVIDEO_FrameType
                                 *
                                 *   @sa IVIDEO_FrameType
                                 */
    XDAS_Int32 outputID;        /**< Output ID corresponding with the decoded
                                 *   buffer.
                                 *
                                 *   @remarks   This will be set to zero when
                                 *              there is no output buffer
                                 *              present in displayBufs.
                                 *
                                 *   @sa IVIDDEC1_InArgs#inputID
                                 */
    IVIDEO_BufDesc displayBufs; /**< The decoder fills these buffers
                                 *   with the current displayable
                                 *   frames.  In case of sequences
                                 *   having I & P frames only, these
                                 *   values are identical to outBufs
                                 *   passed in process().
                                 */
    XDAS_Int32 topFieldFirstFlag;/**< Flag to indicate when the application
                                 *   should display the top field first.
                                 *
                                 *   @remarks   Valid values are XDAS_TRUE
                                 *              and XDAS_FALSE.
                                 *
                                 *   @remarks   This field is only applicable
                                 *              to the display buffer,
                                 *              not the decoded frame.
                                 *
                                 *   @remarks   This field is only applicable
                                 *              for interlaced content, not
                                 *              progressive.
                                 */
    XDAS_Int32 repeatFirstFieldFlag;/**< Flag to indicate when the first field
                                 *   should be repeated.
                                 *
                                 *   @remarks   Valid values are XDAS_TRUE
                                 *              and XDAS_FALSE.
                                 *
                                 *   @remarks   This field is only applicable
                                 *              to the display buffer,
                                 *              not the decoded frame.
                                 *
                                 *   @remarks   This field is only applicable
                                 *              for interlaced content, not
                                 *              progressive.
                                 */
    XDAS_Int32 outBufRetainedFlag;/**< Flag to indicate when the algorithm
                                 *   retains the output buffer.  When XDAS_TRUE,
                                 *   ownership of the buffer has transferred
                                 *   from the application to the algorithm,
                                 *   and the application no longer owns the
                                 *   buffer.
                                 *
                                 *   @remarks   Valid values are XDAS_TRUE
                                 *              and XDAS_FALSE.
                                 */
    XDAS_Int32 displayBufsStatus;/**< @copydoc  IVIDEO_OutputFrameStatus
                                 *
                                 *   @sa        IVIDEO_OutputFrameStatus
                                 */
    XDAS_Int32 repeatFrame;     /**< Number of times the display process
                                 *   needs to repeat the displayed progressive
                                 *   frame.
                                 *
                                 *   @remarks   This information is useful
                                 *              for progressive
                                 *              content when the
                                 *              decoder expects the
                                 *              display process to
                                 *              repeat the displayed
                                 *              frame for a certain
                                 *              number of times. This
                                 *              is useful for pulldown
                                 *              (frame/field
                                 *              repetition by display
                                 *              system) support
                                 *              where the display
                                 *              frame rate is
                                 *              increased without
                                 *              increasing the decode
                                 *              frame rate.
                                 *
                                 *   @remarks   The default value is 0.
                                 */
    XDAS_Int32 displayContentType;/**< Content type of the display buffer.
                                 *
                                 *   @remarks   This field is only applicable
                                 *              to the display buffer,
                                 *              not the decoded frame.
                                 *
                                 *   @remarks   This is useful when the
                                 *              content is both
                                 *              interlaced and
                                 *              progressive.  The
                                 *              display process can
                                 *              use this field to
                                 *              determine how to
                                 *              render the display
                                 *              buffer.
                                 *
                                 *   @sa        IVIDEO_ContentType
                                 */
} IVIDDEC1_OutArgs;


/**
 *  @brief      Defines the control commands for the IVIDDEC1 module.
 *
 *  @remarks    This ID can be extended in IMOD interface for
 *              additional controls.
 *
 *  @sa         XDM_CmdId
 *
 *  @sa         IVIDDEC1_Fxns::control()
 */
typedef  IALG_Cmd IVIDDEC1_Cmd;


/**
 *  @brief      Defines all of the operations on IVIDDEC1 objects.
 */
typedef struct IVIDDEC1_Fxns {
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
 *  @pre        @c inArgs must not be NULL, and must point to a valid
 *              IVIDDEC1_InArgs structure.
 *
 *  @pre        @c outArgs must not be NULL, and must point to a valid
 *              IVIDDEC1_OutArgs structure.
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
 *  @retval     IALG_EFAIL      Failure.  See IVIDDEC1_OutArgs#extendedError
 *                              for more detailed further error conditions.
 */
    XDAS_Int32 (*process)(IVIDDEC1_Handle handle, XDM_BufDesc *inBufs,
        XDM_BufDesc *outBufs, IVIDDEC1_InArgs *inArgs,
        IVIDDEC1_OutArgs *outArgs);

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
 *              IVIDDEC1_DynamicParams structure.
 *
 *  @pre        @c status must not be NULL, and must point to a valid
 *              IVIDDEC1_Status structure.
 *
 *  @post       The algorithm <b>must not</b> modify the contents of @c params.
 *              That is, the data pointed to by this parameter must be
 *              treated as read-only.
 *
 *  @retval     IALG_EOK        Success.
 *  @retval     IALG_EFAIL      Failure.  See IVIDDEC1_Status#extendedError
 *                              for more detailed further error conditions.
 */
    XDAS_Int32 (*control)(IVIDDEC1_Handle handle, IVIDDEC1_Cmd id,
        IVIDDEC1_DynamicParams *params, IVIDDEC1_Status *status);

} IVIDDEC1_Fxns;


/*@}*/

#ifdef __cplusplus
}
#endif

#endif  /* ti_xdais_dm_IVIDDEC1_ */
/*
 *  @(#) ti.xdais.dm; 1, 0, 1, 0,46; 10-18-2006 19:11:35; /db/wtree/library/trees/dais-g07x/src/
 */

