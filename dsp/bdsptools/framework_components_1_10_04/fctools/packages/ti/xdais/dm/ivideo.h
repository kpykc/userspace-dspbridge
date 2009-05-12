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
 *  @file       /ti/xdais/dm/ivideo.h
 *
 *  @brief      This header defines all types, constants, enums, and functions
 *              that are common across the various video codecs.
 */
/**
 *  @addtogroup   ti_xdais_dm_IVIDEO    xDM 0.9 Video Interface
 *
 *  This is the xDM video interface shared between the various codecs.
 *
 *  @todo       In speech, we added a "default" enum value to each enum,
 *              should we do the same for video (and the other classes)?
 */

#ifndef ti_xdais_dm_IVIDEO_
#define ti_xdais_dm_IVIDEO_

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup    ti_xdais_dm_IVIDEO */
/*@{*/

/**
 *  @brief      Video frame skip features for video decoder.
 *
 *  @todo       Proposed IVIDDEC_DynamicParams::frameSkipMode mentions
 *              IVIDEO_SKIP_IDR (for H.264).  Is that needed here?
 */
typedef enum {
    IVIDEO_NO_SKIP = 0,     /**< Do not skip any frame types. */
    IVIDEO_SKIP_P = 1,      /**< Decode the P frame/skip frames internally,
                             *   but do not copy the decoded output to the
                             *   output buffers.  This should be indicated
                             *   by setting the output buffers to NULL.
                             *
                             *   @remarks       For example, if a B frame is
                             *                  dependant on the
                             *                  previously decoded P
                             *                  frame, the B frame
                             *                  shall be decoded and
                             *                  displayed
                             *                  successfully.  For
                             *                  this, the P frame
                             *                  needs to be decoded,
                             *                  but not copied to the
                             *                  output buffer.
                             */
    IVIDEO_SKIP_B = 2,      /**< Skip B, BI frames.  For B frames, the
                             *   decoder will decode the frame
                             *   bitstream, and return as soon as the
                             *   frame type is decisively decoded.
                             *   Internally, the algorithm will modify
                             *   its state, so that subsequent
                             *   decoding of other frames is possible.
                             */
    IVIDEO_SKIP_I = 3,      /**< Skip intra coded frame. */
    IVIDEO_SKIP_IP = 4,     /**< Skip I and P frame/field(s). */
    IVIDEO_SKIP_IB = 5,     /**< Skip I and B frame/field(s). */
    IVIDEO_SKIP_PB = 6,     /**< Skip P and B frame/field(s). */
    IVIDEO_SKIP_IPB = 7     /**< Skip I/P/B/BI frames. */
} IVIDEO_FrameSkip;

/**
 *  @brief      Video frame types.
 *
 *  @remarks    For the various @c IVIDEO_xy_FRAME values, this frame type is
 *              interlaced where both top and bottom fields are
 *              provided in a single frame.  The first field is an "x"
 *              frame, the second field is "y" field.
 */
typedef enum {
    IVIDEO_NA_FRAME = -1,   /**< Frame type not available. */
    IVIDEO_I_FRAME = 0,     /**< Intra coded frame. */
    IVIDEO_P_FRAME = 1,     /**< Forward inter coded frame. */
    IVIDEO_B_FRAME = 2,     /**< Bi-directional inter coded frame. */
    IVIDEO_IDR_FRAME = 3,   /**< Intra coded frame that can be used for
                             *   refreshing video content.
                             */
    IVIDEO_II_FRAME = 4,    /**< Interlaced Frame, both fields are I frames */
    IVIDEO_IP_FRAME = 5,    /**< Interlaced Frame, first field is an I frame,
                             *   second field is a P frame.
                             */
    IVIDEO_IB_FRAME = 6,    /**< Interlaced Frame, first field is an I frame,
                             *   second field is a B frame.
                             */
    IVIDEO_PI_FRAME = 7,    /**< Interlaced Frame, first field is a P frame,
                             *   second field is a I frame.
                             */
    IVIDEO_PP_FRAME = 8,    /**< Interlaced Frame, both fields are P frames. */
    IVIDEO_PB_FRAME = 9,    /**< Interlaced Frame, first field is a P frame,
                             *   second field is a B frame.
                             */
    IVIDEO_BI_FRAME = 10,   /**< Interlaced Frame, first field is a B frame,
                             *   second field is an I frame.
                             */
    IVIDEO_BP_FRAME = 11,   /**< Interlaced Frame, first field is a B frame,
                             *   second field is a P frame.
                             */
    IVIDEO_BB_FRAME = 12    /**< Interlaced Frame, both fields are B frames. */
} IVIDEO_FrameType;

/**
 *  @brief      Video content types.
 */
typedef enum {
    IVIDEO_CONTENTTYPE_NA = -1,/**< Frame type is not available. */
    IVIDEO_PROGRESSIVE = 0,    /**< Progressive video content. */
    IVIDEO_INTERLACED = 1      /**< Interlaced video content. */
} IVIDEO_ContentType;


/**
 *  @brief      Video rate control presets.
 */
typedef enum {
    IVIDEO_LOW_DELAY = 1,   /**< CBR rate control for video conferencing. */
    IVIDEO_STORAGE = 2,     /**< VBR rate control for local storage (DVD)
                             *   recording.
                             */
    IVIDEO_TWOPASS = 3,     /**< Two pass rate control for non real time
                             *   applications.
                             */
    IVIDEO_NONE = 4,        /**< @todo  Document this. */
    IVIDEO_USER_DEFINED = 5 /**< User defined configuration using advanced
                             *   parameters.
                             */
} IVIDEO_RateControlPreset;


/**
 *  @brief      Video frame skipping modes.
 */
typedef enum {
    IVIDEO_FRAME_ENCODED = 0,   /**< Input video frame successfully encoded. */
    IVIDEO_FRAME_SKIPPED = 1    /**< Input video frame dropped.  There is no
                                 *   encoded bitstream corresponding to the
                                 *   input frame.
                                 */
} IVIDEO_SkipMode;


/**
 *  @brief      Video output buffer's status.
 *
 *  @remarks    Ownership of the buffers, either by application or algorithm,
 *              is conveyed via these values.
 *
 *  @todo       What if there are multiple output buffers?  Do these represent
 *              the status of ALL output buffers?
 */
typedef enum {
    IVIDEO_FRAME_NOERROR = 0,   /**< The output buffer is available.
                                 */
    IVIDEO_FRAME_NOTAVAILABLE = 1, /**< The codec doesn't have any output
                                 *   buffers.
                                 */
    IVIDEO_FRAME_ERROR = 2      /**< The output buffer is available and
                                 *   corrupted.
                                 *
                                 *   @remarks   For example, if a bitstream
                                 *              is erroneous and
                                 *              partially decoded, a
                                 *              portion of the decoded
                                 *              image may be available
                                 *              for display.  Another
                                 *              example is if the
                                 *              bitstream for a given
                                 *              frame decode may be
                                 *              decoded without error,
                                 *              but the previously
                                 *              decoded dependant
                                 *              frames weren't
                                 *              successfully decoded.
                                 *              This would result in
                                 *              an incorrectly decoded
                                 *              frame.
                                 */
} IVIDEO_OutputFrameStatus;


/**
 *  @brief      Buffer descriptor for input and output buffers.
 */
typedef struct IVIDEO_BufDesc {
    XDAS_Int32  numBufs;    /**< Number of buffers. */
    XDAS_Int32  width;      /**< Added width of a video frame. */
    XDAS_Int8  *bufs[XDM_MAX_IO_BUFFERS];  /**< Pointer to vector
                                            *   containing buffer addresses.
                                            */
    XDAS_Int32  bufSizes[XDM_MAX_IO_BUFFERS]; /**< Size of each buffer
                                            *   in 8-bit bytes.
                                            */

} IVIDEO_BufDesc;


/*@}*/  /* ingroup */

#ifdef __cplusplus
}
#endif

#endif  /* ti_xdais_dm_IVIDEO_ */
/*
 *  @(#) ti.xdais.dm; 1, 0, 1, 0,46; 10-18-2006 19:11:35; /db/wtree/library/trees/dais-g07x/src/
 */

