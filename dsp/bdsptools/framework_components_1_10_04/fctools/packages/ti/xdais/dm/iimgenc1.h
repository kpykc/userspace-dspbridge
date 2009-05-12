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
 *  @file       ti/xdais/dm/iimgenc1.h
 *
 *  @brief      This header defines all types, constants, and functions
 *              shared by all implementations of the image encoder algorithms.
 */
/**
 *  @defgroup   ti_xdais_dm_IIMGENC1   xDM 1.0 Image Encoder Interface
 *
 *  This is the xDM 1.0 Image Encoder Interface.
 */

#ifndef ti_xdais_dm_IIMGENC1_
#define ti_xdais_dm_IIMGENC1_

#include <ti/xdais/ialg.h>
#include <ti/xdais/xdas.h>
#include "xdm.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup    ti_xdais_dm_IIMGENC1 */
/*@{*/

#define IIMGENC1_ERUNTIME   XDM_ERUNTIME       /**< @copydoc XDM_ERUNTIME */
#define IIMGENC1_EOK        XDM_EOK            /**< @copydoc XDM_EOK */
#define IIMGENC1_EFAIL      XDM_EFAIL          /**< @copydoc XDM_EFAIL */
#define IIMGENC1_EUNSUPPORTED XDM_EUNSUPPORTED /**< @copydoc XDM_EUNSUPPORTED */

/**
 *  @brief      This must be the first field of all IIMGENC1
 *              instance objects.
 */
typedef struct IIMGENC1_Obj {
    struct IIMGENC1_Fxns *fxns;
} IIMGENC1_Obj;


/**
 *  @brief      Opaque handle to an IIMGENC1 objects.
 */
typedef struct IIMGENC1_Obj  *IIMGENC1_Handle;


/**
 *  @brief      Defines the creation time parameters for
 *              all IIMGENC1 instance objects.
 *
 *  @extensibleStruct
 */
typedef struct IIMGENC1_Params {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 maxHeight;       /**< Maximum height. */
    XDAS_Int32 maxWidth;        /**< Maximum width. */
    XDAS_Int32 maxScans;        /**< Maximum number of scans. */
    XDAS_Int32 dataEndianness;  /**< Endianness of output data.
                                 *
                                 *   @sa    XDM_DataFormat
                                 */
    XDAS_Int32 forceChromaFormat;/**< Force encoding in given Chroma format.
                                 *
                                 *   @sa XDM_ChromaFormat
                                 */
}IIMGENC1_Params;


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
 *  @sa         IIMGENC1_Fxns::control()
 */
typedef struct IIMGENC1_DynamicParams {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 numAU;           /**< Number of Access unit to encode.
                                 *   Set to #XDM_DEFAULT to decode the
                                 *   entire frame.
                                 */
    XDAS_Int32 inputChromaFormat;/**< Input chroma format.
                                 *
                                 *    @sa   XDM_ChromaFormat
                                 */
    XDAS_Int32 inputHeight;     /**< Input frame height. */
    XDAS_Int32 inputWidth;      /**< Input frame width. */
    XDAS_Int32 captureWidth;    /**< Pitch.  If set to zero, imageWidth is
                                 *  used; else, this field (assuming it's
                                 *  larger than imageWidth) is used.
                                 */
    XDAS_Int32 generateHeader;  /**< Header generation mode.
                                 *
                                 *   @sa    XDM_DecMode
                                 */
    XDAS_Int32 qValue;          /**< Q value compression factor for encoder. */
} IIMGENC1_DynamicParams;


/**
 *  @brief      Defines the input arguments for all IIMGENC1 instance
 *              process function.
 *
 *  @extensibleStruct
 *
 *  @sa         IIMGENC1_Fxns::process()
 */
typedef struct IIMGENC1_InArgs {
    XDAS_Int32 size;            /**< @sizeField */
} IIMGENC1_InArgs;


/**
 *  @brief      Defines instance status parameters.
 *
 *  @extensibleStruct
 *
 *  @sa         IIMGENC1_Fxns::control()
 */
typedef struct IIMGENC1_Status {
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
    XDAS_Int32 totalAU;         /**< Total number of Access Units. */
    XDM_AlgBufInfo bufInfo;     /**< Input and output buffer information.
                                 *
                                 *   @sa        XDM_AlgBufInfo
                                 */
} IIMGENC1_Status;


/**
 *  @brief      Defines the run time output arguments for all IIMGENC1
 *              instance objects.
 *
 *  @extensibleStruct
 *
 *  @sa         IIMGENC1_Fxns::process()
 */
typedef struct IIMGENC1_OutArgs {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 extendedError;   /**< @extendedErrorField */
    XDAS_Int32 bytesGenerated;  /**< Number of bytes generated during the
                                 *   process() call.
                                 */
    XDAS_Int32 currentAU;       /**< Current access unit number. */
} IIMGENC1_OutArgs;


/**
 *  @brief      Defines the control commands for the IIMGENC1 module.
 *
 *  @remarks    This ID can be extended in IMOD interface for
 *              additional controls.
 *
 *  @sa         XDM_CmdId
 *
 *  @sa         IIMGENC1_Fxns::control()
 */
typedef  IALG_Cmd IIMGENC1_Cmd;


/**
 *  @brief      Defines all of the operations on IIMGENC1 objects.
 */
typedef struct IIMGENC1_Fxns{
    IALG_Fxns   ialg;             /**< Traditional xDAIS algorithm interface. */

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
 *  @pre        @c inArgs must not be NULL, and must point to a valid
 *              IIMGENC1_InArgs structure.
 *
 *  @pre        @c outArgs must not be NULL, and must point to a valid
 *              IIMGENC1_OutArgs structure.
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
 *  @retval     IALG_EFAIL      Failure.  See IIMGENC1_OutArgs#extendedError
 *                              for more detailed further error conditions.
 */
    XDAS_Int32  (*process)(IIMGENC1_Handle handle, XDM_BufDesc *inBufs,
        XDM_BufDesc *outBufs, IIMGENC1_InArgs *inArgs,
        IIMGENC1_OutArgs *outArgs);

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
 *              IIMGENC1_DynamicParams structure.
 *
 *  @pre        @c status must not be NULL, and must point to a valid
 *              IIMGENC1_Status structure.
 *
 *  @post       The algorithm <b>must not</b> modify the contents of @c params.
 *              That is, the data pointed to by this parameter must be
 *              treated as read-only.
 *
 *  @retval     IALG_EOK        Success.
 *  @retval     IALG_EFAIL      Failure.  See IIMGENC1_Status#extendedError
 *                              for more detailed further error conditions.
 */
    XDAS_Int32  (*control)(IIMGENC1_Handle handle, IIMGENC1_Cmd id,
        IIMGENC1_DynamicParams *params, IIMGENC1_Status *status);

} IIMGENC1_Fxns;


/*@}*/  /* ingroup */

#ifdef __cplusplus
}
#endif

#endif  /* ti_xdais_dm_IIMGENC1_ */
/*
 *  @(#) ti.xdais.dm; 1, 0, 1, 0,46; 10-18-2006 19:11:35; /db/wtree/library/trees/dais-g07x/src/
 */

