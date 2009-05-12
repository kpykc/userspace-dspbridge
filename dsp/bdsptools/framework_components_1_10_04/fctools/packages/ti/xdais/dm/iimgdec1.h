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
 *  @file       ti/xdais/dm/iimgdec1.h
 *
 *  @brief      This header defines all types, constants, and functions
 *              shared by all implementations of the image decoder
 *              algorithms.
 */
/**
 *  @defgroup   ti_xdais_dm_IIMGDEC1   xDM 1.0 Image Decoder Interface
 *
 *  This is the xDM 1.0 Image Decoder Interface.
 */

#ifndef ti_xdais_dm_IIMGDEC1_
#define ti_xdais_dm_IIMGDEC1_

#include <ti/xdais/ialg.h>
#include <ti/xdais/xdas.h>
#include "xdm.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup    ti_xdais_dm_IIMGDEC1 */
/*@{*/

#define IIMGDEC1_ERUNTIME XDM_ERUNTIME         /**< @copydoc XDM_ERUNTIME */
#define IIMGDEC1_EOK      XDM_EOK              /**< @copydoc XDM_EOK */
#define IIMGDEC1_EFAIL    XDM_EFAIL            /**< @copydoc XDM_EFAIL */
#define IIMGDEC1_EUNSUPPORTED XDM_EUNSUPPORTED /**< @copydoc XDM_EUNSUPPORTED */

/**
 *  @brief      This must be the first field of all IIMGDEC1
 *              instance objects.
 */
typedef struct IIMGDEC1_Obj {
    struct IIMGDEC1_Fxns *fxns;
} IIMGDEC1_Obj;


/**
 *  @brief      Opaque handle to an IIMGDEC1 objects.
 */
typedef struct IIMGDEC1_Obj  *IIMGDEC1_Handle;


/**
 *  @brief      Defines the creation time parameters for
 *              all IIMGDEC1 instance objects.
 *
 *  @extensibleStruct
 */
typedef struct IIMGDEC1_Params {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 maxHeight;       /**< Maximum image height. */
    XDAS_Int32 maxWidth;        /**< Maximum image width. */
    XDAS_Int32 maxScans;        /**< Maximum number of scans. */
    XDAS_Int32 dataEndianness;  /**< Endianness of output data.
                                 *
                                 *   @sa    XDM_DataFormat
                                 */
    XDAS_Int32 forceChromaFormat;/**< @copydoc XDM_ChromaFormat
                                 *
                                 *   @sa XDM_ChromaFormat.
                                 */
}IIMGDEC1_Params;


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
 *  @extensibleStruct
 *
 *  @sa         IIMGDEC1_Fxns::control()
 */
typedef struct IIMGDEC1_DynamicParams {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 numAU;           /**< Number of Access unit to decode.
                                 *   Set to #XDM_DEFAULT to decode the
                                 *   entire frame.
                                 */
    XDAS_Int32 decodeHeader;    /**< @copydoc XDM_DecMode
                                 *
                                 *   @sa XDM_DecMode
                                 */
    XDAS_Int32 displayWidth;    /**< Pitch.  If set to zero, imageWidth is
                                 *   used; else, this field (assuming it's
                                 *   larger than imageWidth) is used.
                                 */
} IIMGDEC1_DynamicParams;


/**
 *  @brief      Defines the input arguments for all IIMGDEC1 instance
 *              process function.
 *
 *  @extensibleStruct
 *
 *  @sa         IIMGDEC1_Fxns::process()
 */
typedef struct IIMGDEC1_InArgs {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 numBytes;        /**< Size of input data in bytes, provided
                                 *   to the algorithm for decoding.
                                 */
} IIMGDEC1_InArgs;


/**
 *  @brief      Defines instance status parameters.
 *
 *  @extensibleStruct
 *
 *  @sa         IIMGDEC1_Fxns::control()
 */
typedef struct IIMGDEC1_Status {
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
    XDAS_Int32 outputHeight;    /**< Output height. */
    XDAS_Int32 outputWidth;     /**< Output width.  Padding should be taken
                                 *   care of by the decoder.
                                 */
    XDAS_Int32 imageWidth;      /**< Image width. */
    XDAS_Int32 outputChromaFormat; /**< @copydoc XDM_ChromaFormat
                                 *
                                 *   @sa XDM_ChromaFormat
                                 */
    XDAS_Int32 totalAU;         /**< Total number of Access Units. */
    XDAS_Int32 totalScan;       /**< Total number of scans. */
    XDM_AlgBufInfo  bufInfo;    /**< Input and output buffer information.
                                 *
                                 *   @sa    XDM_AlgBufInfo
                                 */
} IIMGDEC1_Status;


/**
 *  @brief      Defines the run time output arguments for all IIMGDEC1
 *              instance objects.
 *
 *  @extensibleStruct
 *
 *  @sa         IIMGDEC1_Fxns::process()
 */
typedef struct IIMGDEC1_OutArgs {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 extendedError;   /**< @extendedErrorField */
    XDAS_Int32 bytesConsumed;   /**< Number of bytes consumed during the
                                 *   process() call.
                                 */
    XDAS_Int32 currentAU;       /**< Current access unit number. */
    XDAS_Int32 currentScan;     /**< Current scan number. */
} IIMGDEC1_OutArgs;


/**
 *  @brief      Defines the control commands for the IIMGDEC1 module.
 *
 *  @remarks    This ID can be extended in IMOD interface for
 *              additional controls.
 *
 *  @sa         XDM_CmdId
 *
 *  @sa         IIMGDEC1_Fxns::control()
 */
typedef  IALG_Cmd IIMGDEC1_Cmd;


/**
 *  @brief      Defines all of the operations on IIMGDEC1 objects.
 */
typedef struct IIMGDEC1_Fxns{
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
 *  @pre        @c inArgs must not be NULL, and must point to a valid
 *              IIMGDEC1_InArgs structure.
 *
 *  @pre        @c outArgs must not be NULL, and must point to a valid
 *              IIMGDEC1_OutArgs structure.
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
 *  @retval     IALG_EFAIL      Failure.  See IIMGDEC_OutArgs#extendedError
 *                              for more detailed further error conditions.
 */
    XDAS_Int32 (*process)(IIMGDEC1_Handle handle, XDM_BufDesc *inBufs,
            XDM_BufDesc *outBufs, IIMGDEC1_InArgs *inArgs,
            IIMGDEC1_OutArgs *outArgs);

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
 *              IIMGDEC1_DynamicParams structure.
 *
 *  @pre        @c status must not be NULL, and must point to a valid
 *              IIMGDEC1_Status structure.
 *
 *  @post       The algorithm <b>must not</b> modify the contents of @c params.
 *              That is, the data pointed to by this parameter must be
 *              treated as read-only.
 *
 *  @retval     IALG_EOK        Success.
 *  @retval     IALG_EFAIL      Failure.  See IIMGDEC_Status#extendedError
 *                              for more detailed further error conditions.
 */
    XDAS_Int32 (*control)(IIMGDEC1_Handle handle, IIMGDEC1_Cmd id,
            IIMGDEC1_DynamicParams *params, IIMGDEC1_Status *status);

}IIMGDEC1_Fxns;


/*@}*/  /* ingroup */

#ifdef __cplusplus
}
#endif

#endif  /* ti_xdais_dm_IIMGDEC1_ */
/*
 *  @(#) ti.xdais.dm; 1, 0, 1, 0,46; 10-18-2006 19:11:35; /db/wtree/library/trees/dais-g07x/src/
 */

