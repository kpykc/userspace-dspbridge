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
 *  @file       ti/xdais/dm/ispeech1.h
 *
 *  @brief      This header defines all types, constants, enums, and functions
 *              that are common across the various speech codecs.
 */
/**
 *  @defgroup   ti_xdais_dm_ISPEECH1  xDM 1.0 Speech/Voice Interface
 *
 *  This is the xDM1 speech/voice interface shared between the various codecs.
 */

#ifndef ti_xdais_dm_ISPEECH1_
#define ti_xdais_dm_ISPEECH1_

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup    ti_xdais_dm_ISPEECH1 */
/*@{*/


/**
 *  @brief      Voice Activity Detection (VAD) setting.
 */
typedef enum {
    ISPEECH1_VADFLAG_OFF = 0,          /**< Voice Activity Detection off. */
    ISPEECH1_VADFLAG_ON = 1,           /**< Voice Activity Detection on. */

    /** @copydoc ISPEECH1_VADFLAG_ON */
    ISPEECH1_VADFLAG_DEFAULT = ISPEECH1_VADFLAG_ON
} ISPEECH1_VADFlag;


/**
 *  @brief      Post filter setting.
 *
 *  These values are used for enabling/disabling the post filter.
 */
typedef enum {
    ISPEECH1_POSTFILTER_OFF = 0,       /**< Post Filter disabled. */
    ISPEECH1_POSTFILTER_ON = 1,        /**< Post Filter enabled. */

    /** @copydoc ISPEECH1_POSTFILTER_ON */
    ISPEECH1_POSTFILTER_DEFAULT = ISPEECH1_POSTFILTER_ON
} ISPEECH1_PostFilter;


/**
 *  @brief      Null traffic channel setting.
 *
 *  @sa         ISPHENC1_InArgs.nullTrafficChannel.
 */
typedef enum {
    ISPEECH1_NULLTRAFFICMODE_OFF = 0,  /**< Null traffic channel frames
                                        *   disabled, normal speech encoding
                                        *   is enabled.
                                        */
    ISPEECH1_NULLTRAFFICMODE_ON = 1,   /**< Null traffic channel frames
                                        *   enabled.
                                        */

    /** @copydoc ISPEECH1_NULLTRAFFICMODE_OFF */
    ISPEECH1_NULLTRAFFICMODE_DEFAULT = ISPEECH1_NULLTRAFFICMODE_OFF
} ISPEECH1_NullTrafficMode;

/**
 *  @brief      Codec selection if codec supports multiple options.
 *
 *  @remarks    This parameter is used to select the codec in cases where
 *              multiple codecs are supported from the same code base
 *              (e.g. GSMEFR can be supported from GSMAMR
 *              codebase).  This parameter is also used for codecs
 *              which support multiple rates as initialization time
 *              option (like g726 supports 16, 24,32 and 40KBPS).  In
 *              this case each rate is configured as a different codec.
 *
 *  @remarks    See the codec-specific documentation for further details
 *              describing valid values for these fields.
 */
typedef enum {
    ISPEECH1_CODECSELECT_PRIMARY = 0,      /**< Primary mode. */
    ISPEECH1_CODECSELECT_SECONDARY = 1,    /**< Secondary mode. */
    ISPEECH1_CODECSELECT_THIRD = 2,        /**< Third mode. */
    ISPEECH1_CODECSELECT_FOURTH = 3,       /**< Fourth mode. */

    /** @copydoc ISPEECH1_CODECSELECT_PRIMARY */
    ISPEECH1_CODECSELECT_DEFAULT = ISPEECH1_CODECSELECT_PRIMARY
} ISPEECH1_CodecSelect;


/*@}*/  /* ingroup */

#ifdef __cplusplus
}
#endif

#endif  /* ti_xdais_dm_ISPEECH1_ */
/*
 *  @(#) ti.xdais.dm; 1, 0, 1, 0,46; 10-18-2006 19:11:35; /db/wtree/library/trees/dais-g07x/src/
 */

