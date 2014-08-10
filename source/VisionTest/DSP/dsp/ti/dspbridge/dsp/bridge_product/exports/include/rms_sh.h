/*
 *! ============================================================
 *! Copyright 2005 by Texas Instruments Incorporated.
 *! All rights reserved. Property of Texas Instruments Incorporated.
 *! Restricted rights to use, duplicate or disclose this code are
 *! granted through contract.
 *! ============================================================
 */
/**
 *  @file 		rms_sh.h
 *
 *  @brief     	DSP Bridge Resource Manager Server Shared Definitions
 */

/**
 *  @defgroup 	DSPRMSSH RMS GPP/DSP shared definitions
 *
 *				The Resource Manager Server (RMS) shared header defines types,
 *              codes, constants, and structures that are shared between the
 *              DSP Bridge GPP (MPU) and DSP frameworks.
 */
#ifndef RMS_SH_
#define RMS_SH_

/** @ingroup DSPRMSSH */
/*@{*/


#ifdef __cplusplus
extern "C" {
#endif


#include <rmstypes.h>

/**
 *  @name Defines: Node Types
 */
/*@{*/
#define RMS_TASK                1    /**< Node Type: Task node */
#define RMS_DAIS                2    /**< Node Type: XDAIS socket node */
#define RMS_MSG                 3    /**< Node Type: Message node */
/*@}*/

/**
 *  @name Defines: Memory Types
 */
/*@{*/
#define RMS_CODE                0    /**< Memory Type: Program space */
#define RMS_DATA                1    /**< Memory Type: Data space */
#define RMS_IO                  2    /**< Memory Type: I/O space */
/*@}*/

/**
 *  @name Defines: RMS Command/Response Codes
 */
/*@{*/
#define RMS_COMMANDBUFSIZE      256   /**< Size of RMS command buffer */
#define RMS_RESPONSEBUFSIZE     16    /**< Size of RMS response buffer */


#define RMS_EXIT                0x80000000  /**< GPP->Node: Shutdown */
#define RMS_EXITACK             0x40000000  /**< Node->GPP: Acknowledge
                                               shutdown */
#define RMS_BUFDESC             0x20000000  /**< Arg1 is shared memory buffer,
                                               Arg2 is shared memory size */
#define RMS_KILLTASK        	0x10000000

#define RMS_USER                0x0     /**< Start of user-defined msg codes */
#define RMS_MAXUSERCODES        0xfff   /**< Maximum user defined C/R Codes */
/*@}*/


/** @cond BRIDGE_INTERNAL */
/**
 *	@brief    RM Server RPC Command Structure.
 */
typedef struct {
    RMS_WORD fxn;               /**< Server function address */
    RMS_WORD arg1;              /**< First argument */
    RMS_WORD arg2;              /**< Second argument */
    RMS_WORD data;              /**< Function-specific data array */
} RMS_Command;
/** @endcond */


/**
 *  @brief      The RMS_StrmDef structure defines the parameters for both
 *              input and output streams, and is passed to a node's
 *              create function.
 */
typedef struct {
    RMS_WORD     bufsize;       /**< Buffer size (in DSP words) */
    RMS_WORD     nbufs;         /**< Max number of bufs in stream */
    RMS_WORD     segid;         /**< Segment to allocate buffers */
    RMS_WORD     align;         /**< Alignment for allocated buffers */
    RMS_WORD     timeout;       /**< Timeout (msec) for blocking calls */
    RMS_CHAR     name[1];       /**< Device Name (terminated by '\\0') */
} RMS_StrmDef;


/** @cond BRIDGE_INTERNAL */
/**
 *  @brief      Message node create args structure.
 */
typedef struct {
    RMS_WORD    maxMessages;    /**< Maximum number of simultaneous messages
                                   to node. */
    RMS_WORD    segid;          /**< Memory segment used by NODE_allocMsgBuf. */
    RMS_WORD    notifyType;     /**< Type of message notification. */
    RMS_WORD    argLength;      /**< Length (in DSP chars) of arg data. */
    RMS_WORD    argData;        /**< Arg data for node. */
} RMS_MsgArgs;
/** @endcond */


/** @cond BRIDGE_INTERNAL */
/**
 *  @brief      Partial task create args structure.
 */
typedef struct {
    RMS_WORD    priority;       /**< Task's runtime priority level. */
    RMS_WORD    stackSize;      /**< Task's stack size. */
    RMS_WORD    sysstackSize;   /**< Task's system stack size (55x). */
    RMS_WORD    stackSeg;       /**< Memory segment for task's stack. */
#ifdef _TI_NODEHEAP
    RMS_WORD    heapAddr;       /**< Base address of task's heap in
                                   external memory. */
    RMS_WORD    heapSize;       /**< Size (MAUs) of task's heap in external
                                   memory. */
#endif
    RMS_WORD    misc;           /**< Misc field.  Not used for @b normal
                                   task nodes; for xDAIS socket nodes
                                   this specifies the IALG_Fxn pointer. */
    RMS_WORD    numInputStreams;/**< Number of input STRM definition
                                   structures */
} RMS_MoreTaskArgs;
/** @endcond */


#ifdef __cplusplus
}
#endif /* extern "C" */


/*@}*/ /* ingroup DSPRMSSH */


#endif /* RMS_SH_ */
