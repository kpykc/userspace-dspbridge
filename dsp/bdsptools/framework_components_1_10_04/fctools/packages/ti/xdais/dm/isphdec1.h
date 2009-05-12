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
 *  @file       ti/xdais/dm/isphdec1.h
 *
 *  @brief      This header defines all types, constants, and functions
 *              shared by all implementations of the speech/voice decoder
 *              algorithms.
 */
/**
 *  @defgroup   ti_xdais_dm_ISPHDEC1  xDM 1.0 Speech/Voice Decoder Interface
 *
 *  This is the xDM 1.0 Speech/Voice Decoder Interface.
 */

#ifndef ti_xdais_dm_ISPHDEC1_
#define ti_xdais_dm_ISPHDEC1_

#include <ti/xdais/ialg.h>
#include <ti/xdais/xdas.h>
#include "xdm.h"
#include "ispeech1.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup    ti_xdais_dm_ISPHDEC1 */
/*@{*/

#define ISPHDEC1_ERUNTIME  XDM_ERUNTIME        /**< @copydoc XDM_ERUNTIME */
#define ISPHDEC1_EOK       XDM_EOK             /**< @copydoc XDM_EOK */
#define ISPHDEC1_EFAIL     XDM_EFAIL           /**< @copydoc XDM_EFAIL */
#define ISPHDEC1_EUNSUPPORTED XDM_EUNSUPPORTED /**< @copydoc XDM_EUNSUPPORTED */

/**
 *  @brief      This must be the first field of all ISPHDEC1
 *              instance objects.
 */
typedef struct ISPHDEC1_Obj {
    struct ISPHDEC1_Fxns *fxns;
} ISPHDEC1_Obj;


/**
 *  @brief      Opaque handle to an ISPHDEC1 objects.
 */
typedef struct ISPHDEC1_Obj  *ISPHDEC1_Handle;


/**
 *  @brief      Defines the creation time parameters for
 *              all ISPHDEC1 instance objects.
 *
 *  @remarks    Some of the fields in this structure are optional and depend
 *              on the class of speech decoder you're creating.
 *
 *  @extensibleStruct
 */
typedef struct ISPHDEC1_Params {
    XDAS_Int16 size;            /**< @sizeField */
    XDAS_Int16 compandingLaw;   /**< Optional, codec-specific companding law.
                                 *   See your codec-specific interface
                                 *   documentation options.
                                 *
                                 *   @sa ISPEECH1_PCM_CompandingLaw
                                 *   @sa ISPEECH1_G726_CompandingLaw
                                 */
    XDAS_Int16 packingType;     /**< Optional, codec-specific packing type.
                                 *   See your codec-specific interface
                                 *   documentation options.
                                 *
                                 *   @sa ISPEECH1_AMR_PackingType
                                 *   @sa ISPEECH1_G726_PackingType
                                 *   @sa ISPEECH1_WBAMR_PackingType
                                 */
    XDAS_Int16 codecSelection;  /**< @copydoc ISPEECH1_CodecSelect
                                 *
                                 *   @sa ISPEECH1_CodecSelect
                                 *   @sa ISPHDEC1_Status.codecSelection
                                 */
    XDAS_Int8  **tablesPtr;     /**< Optional pointer to the codec's
                                 *   initialization tables.
                                 */
} ISPHDEC1_Params;


/**
 *  @brief      This structure defines the codec parameters that can be
 *              modified after creation via ISPHDEC1_Fxns.control().
 *
 *  @remarks    It is not necessary that a given implementation support all
 *              dynamic parameters to be configurable at run time.  If a
 *              particular algorithm does not support run-time updates to
 *              a parameter that the application is attempting to change
 *              at runtime, it may indicate this as an error.
 *
 *  @extensibleStruct
 */
typedef struct ISPHDEC1_DynamicParams {
    XDAS_Int16 size;            /**< @sizeField */
    XDAS_Int16 postFilter;      /**< Assign the postFilter setting.
                                 *
                                 *   @sa ISPEECH1_PostFilter
                                 *   @sa ISPHDEC1_Status.postFilter
                                 */
} ISPHDEC1_DynamicParams;


/**
 *  @brief      Decoder frame types.
 *
 *  @enumWarning
 *
 *  @sa         ISPHDEC1_InArgs.frameType
 */
typedef enum {
    ISPHDEC1_FTYPE_SPEECHGOOD = 0,  /**< Regular speech frame received
                                     *   without errors/loss.
                                     */
    ISPHDEC1_FTYPE_SIDUPDATE = 1,   /**< SID update frames. */
    ISPHDEC1_FTYPE_NODATA = 2,      /**< Untransmitted frame for codecs which
                                     *   support internal DTX.
                                     */
    ISPHDEC1_FTYPE_SPEECHLOST = 3   /**< Complete loss of speech frame. */
} ISPHDEC1_FrameType;


/**
 *  @brief      Defines the input arguments for all ISPHDEC1 instance
 *              process function.
 */
typedef struct ISPHDEC1_InArgs {
    XDAS_Int16 frameType;       /**< Frame type of this buffer.
                                 *
                                 *   @sa    ISPHDEC1_FrameType
                                 */
    XDAS_Int16 reserved;        /**< Reserved - serves to pad this structure. */
    XDM_SingleBufDesc data;     /**< Raw data output for data
                                 *   transmission.
                                 */
} ISPHDEC1_InArgs;


/**
 *  @brief      Defines instance status parameters.
 *
 *  @extensibleStruct
 */
typedef struct ISPHDEC1_Status {
    XDAS_Int16 size;            /**< @sizeField */
    XDAS_Int16 postFilter;      /**< Provides the current postFilter setting.
                                 *
                                 *   @sa    ISPEECH1_PostFilter
                                 */
    XDAS_Int32 extendedError;   /**< @extendedErrorField
                                 */
    XDM_SingleBufDesc data;     /**< Buffer descriptor for data passing.
                                 *
                                 *   @remarks   If this field is not used,
                                 *              the application <b>must</b>
                                 *              set @c data.buf to NULL.
                                 *
                                 *   @remarks   This buffer can be used as
                                 *              either input or output,
                                 *              depending on the command.
                                 *
                                 *   @remarks   The buffer will be provided
                                 *              by the application, and
                                 *              returned to the application
                                 *              upon return of the
                                 *              ISPHDEC1_Fxns.control()
                                 *              call.  The algorithm must
                                 *              not retain a pointer to this
                                 *              data.
                                 *
                                 *   @sa        #XDM_GETVERSION
                                 */
    XDAS_Int16 compandingLaw;   /**< Optional, codec-specific companding law.
                                 *   See your codec-specific interface
                                 *   documentation options.
                                 *
                                 *   @sa ISPEECH1_PCM_CompandingLaw
                                 *   @sa ISPEECH1_G726_CompandingLaw
                                 */
    XDAS_Int16 packingType;     /**< Optional, codec-specific packing type.
                                 *   See your codec-specific interface
                                 *   documentation options.
                                 *
                                 *   @sa ISPEECH1_AMR_PackingType
                                 *   @sa ISPEECH1_G726_PackingType
                                 *   @sa ISPEECH1_WBAMR_PackingType
                                 */
    XDM_AlgBufInfo  bufInfo;    /**< Input and output buffer information.
                                 *
                                 *   @sa XDM_AlgBufInfo
                                 */
    XDAS_Int16 codecSelection;  /**< @copydoc ISPEECH1_CodecSelect
                                 *
                                 *   @sa ISPEECH1_CodecSelect
                                 *   @sa ISPHDEC1_Params.codecSelection
                                 */
    XDAS_Int16 reserved;        /**< Reserved - serves to pad this structure. */
} ISPHDEC1_Status;


/**
 *  @brief      Defines the run time output arguments for
 *              all ISPHDEC1 instance objects.
 */
typedef struct ISPHDEC1_OutArgs {
    XDAS_Int32 extendedError;   /**< @extendedErrorField */
    XDAS_Int16 dataSize;        /**< Size of raw data output in bits. */
    XDAS_Int16 reserved;        /**< Reserved - serves to pad this structure. */
} ISPHDEC1_OutArgs;


/**
 *  @brief      Defines the control commands for the ISPHDEC1 module.
 *
 *  @remarks    This ID can be extended in IMOD interface for
 *              additional control commands.
 *
 *  @sa         XDM_CmdId
 */
typedef  IALG_Cmd ISPHDEC1_Cmd;


/**
 *  @brief      Defines all of the operations on ISPHDEC1 objects.
 */
typedef struct ISPHDEC1_Fxns{
    IALG_Fxns   ialg;             /**< Traditional xDAIS algorithm interface. */

/**
 *  @brief      Basic speech/voice decoding call.
 *
 *  @param[in]  handle          Handle to an algorithm instance.
 *  @param[in]  inCodeWords     Input buffer.
 *  @param[in,out] outSamples   Output buffer.
 *  @param[in]  inArgs          Input arguments.  This is a required
 *                              parameter.
 *  @param[out] outArgs         Ouput results.  This is a required parameter.
 *
 *  @pre        @c handle must be a valid algorithm instance handle.
 *
 *  @pre        @c inArgs must not be NULL, and must point to a valid
 *              ISPHDEC1_InArgs structure.
 *
 *  @pre        @c outArgs must not be NULL, and must point to a valid
 *              ISPHDEC1_OutArgs structure.
 *
 *  @pre        @c inCodeWords must not be NULL, and must point to a valid
 *              XDM_SingleBufDesc structure.
 *
 *  @pre        @c inCodeWords.buf must not be NULL, and must point to a valid
 *              buffer of data that is at least @c inCodeWords.bufSize bytes in
 *              length.
 *
 *  @pre        @c outSamples must not be NULL, and must point to a valid
 *              XDM_SingleBufDesc structure.
 *
 *  @pre        @c outSamples.buf must not be NULL, and must point to a memory
 *              buffer of at least @c outSamples.bufSize bytes in length.
 *
 *  @post       The algorithm <b>must not</b> modify the contents of
 *              @c inCodeWords, @c inCodeWords.buf, or @c inArgs.
 *              That is, the data and buffers pointed to by these parameters
 *              must be treated as read-only.
 *
 *  @retval     ISPHDEC1_EOK            @copydoc ISPHDEC1_EOK
 *  @retval     ISPHDEC1_EFAIL          @copydoc ISPHDEC1_EFAIL
 *                                      See ISPHDEC1_Status#extendedError
 *                                      for more detailed further error
 *                                      conditions.
 *  @retval     ISPHDEC1_ERUNTIME       @copydoc ISPHDEC1_ERUNTIME
 *  @retval     ISPHDEC1_EUNSUPPORTED   @copydoc ISPHDEC1_EUNSUPPORTED
 */
    XDAS_Int32 (*process)(ISPHDEC1_Handle handle,
        XDM_SingleBufDesc *inCodeWords, XDM_SingleBufDesc *outSamples,
        ISPHDEC1_InArgs *inArgs, ISPHDEC1_OutArgs *outArgs);

/**
 *  @brief      Control behaviour of an algorithm.
 *
 *  @param[in]  handle          Handle to an algorithm instance.
 *  @param[in]  id              Command id.  See #XDM_CmdId.
 *  @param[in]  params          Dynamic parameters.  This is a required
 *                              parameter.
 *  @param[out] status          Output results.  This is a required parameter.
 *
 *  @pre        @c handle must be a valid algorithm instance handle.
 *
 *  @pre        @c params must not be NULL, and must point to a valid
 *              ISPHDEC1_DynamicParams structure.
 *
 *  @pre        @c status must not be NULL, and must point to a valid
 *              ISPHDEC1_Status structure.
 *
 *  @post       The algorithm <b>must not</b> modify the contents of @c params.
 *              That is, the data pointed to by this parameter must be
 *              treated as read-only.
 *
 *  @retval     ISPHDEC1_EOK            @copydoc ISPHDEC1_EOK
 *  @retval     ISPHDEC1_EFAIL          @copydoc ISPHDEC1_EFAIL
 *                                      See ISPHDEC1_Status#extendedError
 *                                      for more detailed further error
 *                                      conditions.
 *  @retval     ISPHDEC1_ERUNTIME       @copydoc ISPHDEC1_ERUNTIME
 *  @retval     ISPHDEC1_EUNSUPPORTED   @copydoc ISPHDEC1_EUNSUPPORTED
 */
    XDAS_Int32 (*control)(ISPHDEC1_Handle handle,
        ISPHDEC1_Cmd id, ISPHDEC1_DynamicParams *params,
        ISPHDEC1_Status *status);

} ISPHDEC1_Fxns;


/*@}*/

#ifdef __cplusplus
}
#endif

#endif  /* ti_xdais_dm_ISPHDEC1_ */
/*
 *  @(#) ti.xdais.dm; 1, 0, 1, 0,46; 10-18-2006 19:11:35; /db/wtree/library/trees/dais-g07x/src/
 */

