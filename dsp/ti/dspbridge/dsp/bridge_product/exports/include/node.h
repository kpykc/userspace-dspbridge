/*
 *! ============================================================
 *! Copyright 2005 by Texas Instruments Incorporated.
 *! All rights reserved. Property of Texas Instruments Incorporated.
 *! Restricted rights to use, duplicate or disclose this code are
 *! granted through contract.
 *! ============================================================
 */
/**
 *  @file       node.h
 *
 *  @brief      DSP Bridge NODE API Definitions - Provides node messaging and
 *              synchronization services.
 */

/**
 *  @defgroup   DSPNODE NODE
 *
 *              The DSP Bridge NODE API provides node messaging and
 *              sychronization services.
 */

#ifndef NODE_
#define NODE_


/** @ingroup 	DSPNODE */
/*@{*/


#ifdef __cplusplus
extern "C" {
#endif


#include <rmsdefs.h>    /* RM Server external definitions */
#include <strm.h>       /* STRM module definitions */


#define NODE_FOREVER    (Uns)-1 /**< infinite timeout (SYS_FOREVER) */
#define NODE_TOGPP      0       /**< GPP is the destination for message */


/** @cond 		BRIDGE_INTERNAL */
#define NODE_MSGISRESPONSE   0x80000000 /**< bit to set when MSG is response */
#define NODE_MSGISDESCRIPTOR 0x40000000 /**< bit to set when MSG is descriptor
                                         */
/** @endcond */


/**
 *  @brief Opaque Node environment pointer.
 */
typedef RMS_MsgEnv * NODE_EnvPtr;


/** @cond BRIDGE_INTERNAL */
/**
 *  @brief Internal structure used to configure node properties.
 */
typedef struct NODE_Config {
    Int     MSGFRAMESEG;    /**< Segment to allocate message frames */
    Uns     MAXNODEHEAPS;   /**< Maximum number of nodes loaded on the target
                                 that have external heaps
                             */
} NODE_Config;


/**
 *  @brief Node configuration variable.
 */
#ifdef _64P_
extern far NODE_Config *NODE;
#else
extern NODE_Config *NODE;
#endif
/** @endcond */

/**
 *  @brief      Allocate a data buffer whose descriptor will be passed to the
 *              GPP within a message.
 *
 *  @param[in]  node            Node's environment pointer.
 *  @param[in]  size            Size (in DSP MAU minimum addressible units -
 *                              MAUs) of the buffer to be allocated.
 *  @param[in]  align           Buffer alignment.
 *
 *  @retval     non-NULL        Address of the allocated buffer.
 *  @retval     NULL            Failure.
 *
 *  @remarks    NODE_allocMsgBuf() allocates a contiguous block of memory,
 *              size minimum addressable units long, with an alignment multiple
 *              of align (an alignment of 0 or 1 indicates no alignment
 *              constraint; otherwise, align must be set to a power of 2).
 *              Note that the memory segment that will be used for
 *              the allocation is passed from the GPP to the node as a creation
 *              argument, and it is maintained internally within the node's
 *              environment.
 *
 * @remarks     If an allocation failure occurs, NODE_allocMsgBuf() returns an
 *              addr of NULL.
 *
 * @remarks     Buffers allocated with NODE_allocMsgBuf() must be freed by
 *              the application via calls to NODE_freeMsgBuf().
 *
 * @note        A node is allowed to call NODE_allocMsgBuf() only during its
 *              create-phase function.
 *
 * @sa          NODE_freeMsgBuf
 */
extern Ptr NODE_allocMsgBuf(NODE_EnvPtr node, Uns size, Uns align);


/** @cond       BRIDGE_INTERNAL */
/**
 *  @brief      Enable message flow between the DSP and GPP.  This is an
 *              OEM-only function, and not part of the DSP Bridge application
 *              developer's API.
 *
 *  @remarks    NODE_enableMsg is useful only if the @c MSG_ENABLE bit is
 *              configured for the Messenger's SWIs.  This function will call
 *              @c SWI_andn(swi, MSG_ENABLE) to clear the @c MSG_ENABLE bit
 *              in each of the Messenger SWI's mailboxes, conditionally posting
 *              the SWI if the resultant mailbox value is zero.
 *
 *  @remarks    This function will only enable message flow temporarily,
 *              i.e., this function must be called again after a Messenger
 *              SWI runs, to re-enable message flow.
 *
 *  @sa         NODE_getMsg(), NODE_putMsg()
 */
extern Void NODE_enableMsg(Void);
/** @endcond */


/**
 *  @brief      Release a data buffer previously allocated with
 *              NODE_allocMsgBuf().
 *
 *  @param[in]  node    Node's environment pointer.
 *  @param[in]  addr    Address of the buffer, as returned from a successful
 *                      call to NODE_allocMsgBuf().
 *  @param[in]  size    Size (in DSP MAUs) of the buffer to be freed.
 *
 *  @retval     TRUE    Success.
 *  @retval     FALSE   A failure occurs when the specified address does
 *                      not reside in the memory segment used for allocations by
 *              NODE_allocMsgBuf().
 *
 *  @remarks    NODE_freeMsgBuf() releases a data buffer back to the free
 *              memory pool.
 *
 *  @remarks    NODE_freeMsgBuf() returns @c TRUE on success.  @c FALSE is
 *              returned if @c addr does not reside in the memory segment that
 *              NODE_allocMsgBuf() uses for allocation.
 *
 *  @note       NODE_freeMsgBuf() must only be called for a buffer that was
 *              successfully allocated using NODE_allocMsgBuf().
 *
 *  @sa         NODE_allocMsgBuf()
 *
 */
extern Bool NODE_freeMsgBuf(NODE_EnvPtr node, Ptr addr, Uns size);


/**
 *  @brief      Retrieve the base address of the node's external heap,
 *              if defined.
 *
 *  @param[in]  node            Node's environment pointer.
 *
 *  @retval     non-NULL        Address of node's external heap.
 *  @retval     NULL            Node has no external heap defined.
 *
 *  @pre        <tt>node != NULL</tt>
 */
extern Ptr NODE_getHeapAddr(NODE_EnvPtr node);

/**
 *  @brief      Retrieve the segment ID of the node's external heap,
 *              if defined.
 *
 *  @param[in]  node            Node's environment pointer.
 *
 *  @retval     > 0             Segment id of node's external heap.
 *  @retval     -1              Node has no external heap defined.
 *
 *  @pre        <tt>node != NULL</tt>
 */
extern Int NODE_getHeapSeg(NODE_EnvPtr node);

/**
 *  @brief      Retrieve a message from another node or the GPP.
 *
 *  @param[in]          node    Node's environment pointer.
 *  @param[in,out]      msg     Pointer to a message buffer to be filled.
 *                              Note: an @c RMS_DSPMSG structure is the DSP-side
 *                              representation of @c DSP_MSG, i.e., a structure
 *                              holding three 32-bit message elements: a
 *                              command code, and two arguments.
 *                              message contents.
 *  @param[in]          timeout Timeout value in milliseconds.
 *
 *  @retval     TRUE    Success.
 *  @retval     FALSE   Failure.
 *
 *  @pre        <tt>node != NULL</tt>
 *  @pre        <tt>msg != NULL</tt>
 *  @pre        if <tt>timeout != 0</tt> then must be running in a task context.
 *
 *  @remarks    NODE_getMsg() retrieves a message from the node's message queue.
 *              If a message is not available, the function will block until
 *              one becomes available, or the timeout value is reached. If
 *              timeout is @c NODE_FOREVER, the function will block indefinitely
 *              until a message becomes available. If timeout is 0 (zero), the
 *              function will not block.  The return value of NODE_getMsg()
 *              indicates whether a message was received or not. If
 *              NODE_getMsg() returns @c TRUE, a message was received, otherwise
 *              the call timed out.
 *
 *  @note       If NODE_getMsg() is called from a context where blocking is not
 *              allowed, (e.g., a software interrupt context), @c timeout
 *              @b must be specified as zero.
 *
 *  @note       NODE_getMsg() has the same calling restrictions as the
 *              DSP/BIOS @c SEM_pend() service.
 *
 *  @sa         NODE_allocMsgBuf(), NODE_freeMsgBuf(), NODE_putMsg()
 */
extern Bool NODE_getMsg(NODE_EnvPtr node, RMS_DSPMSG * msg, Uns timeout);


/**
 *  @brief      Retrieve the task priority of a node.
 *
 *  @param[in]  node            Node's environment pointer.
 *
 *  @retval     taskPriority    The task priority set for the node.
 *
 *  @pre        <tt>node != NULL</tt>
 */
extern Uns NODE_getPri(NODE_EnvPtr node);


/**
 *  @brief      Post a message to another node or the GPP.
 *
 *  @param[in]  self    Environment pointer of the node that is posting the
 *                      message.
 *  @param[in]  dest    Environment pointer of the destination node for the
 *                      message.  If the destination is another node, @c dest
 *                      is the other node's environment pointer.  If the message
 *                      is being sent to the GPP, dest should be specified as
 *                      the manifest constant @c NODE_TOGPP.
 *  @param[in]  msg     Pointer to the message to send.  Note:  an
 *                      @c RMS_DSPMSG structure is the DSP-side representation
 *                      of @c DSP_MSG, i.e., a structure holding three 32-bit
 *                      message elements:  a command code, and two arguments.
 *  @param[in]  timeout Timeout value in milliseconds.
 *
 *  @retval     TRUE    Success.
 *  @retval     FALSE   Failure.
 *
 *  @pre        <tt>self != NULL</tt>
 *  @pre        <tt>msg != NULL</tt>
 *  @pre        if <tt>timeout != 0</tt> then \b must be running in a task
 *              context.
 *
 *  @remarks    NODE_putMsg() attempts to post a message to another node or to
 *              the GPP. The function blocks until the destination's message
 *              queue can accommodate the message, or until the timeout value
 *              is reached. If timeout is @c NODE_FOREVER, the function will
 *              block indefinitely. If the timeout value is 0 (zero), the
 *              function will not block.
 *
 *  @remarks    The return value of NODE_putMsg() is @c TRUE if the message was
 *              queued, otherwise it is @c FALSE.  Note that the message
 *              contents are copied out of the message structure referenced by
 *              msg, so the structure can be re-used immediately after returning
 *              from NODE_putMsg().

 *  @note       If NODE_putMsg() is called from a context where blocking is not
 *              allowed, (e.g., a software interrupt context), @c timeout
 *              \b must be specified as zero.
 *
 *  @note       NODE_putMsg() has the same calling restrictions as the
 *              DSP/BIOS @c SEM_pend() service.
 *
 *  @sa         NODE_allocMsgBuf(), NODE_freeMsgBuf(), NODE_getMsg()
 */
extern Bool NODE_putMsg(NODE_EnvPtr self, NODE_EnvPtr dest, RMS_DSPMSG * msg,
    Uns timeout);


/**
 *  @brief      Wait until a message has arrived for this node, or at least one
 *              of a set of specified streams is ready for I/O.
 *
 *  @param[in]  node            Node's environment pointer.  If node is NULL,
 *                              NODE_wait() will only wait for a stream to be
 *                              ready, i.e., it will not wait for messages.
 *  @param[in]  streamtab       Array of stream handles.
 *  @param[in]  nstreams        Number of streams in @c streamtab[].
 *  @param[in]  timeout         Timeout value in milliseconds.
 *  @param[out] messagesReady   The location where the function should write
 *                              the number of messages ready for the node.
 *
 *  @retval     mask            A bit mask indicating which streams in
 *                              @c streamtab[] are ready for I/O.  A value of
 *                              zero indicates no streams are ready.
 *
 *  @pre        <tt>messagesReady != NULL</tt>
 *  @pre        If <tt>timeout != 0</tt> then must be running in a task context.
 *
 *  @remarks    NODE_wait() will block until one or more of the streams in the
 *              @c streamtab[] array is ready for I/O (i.e., a reclaim
 *              operation on the ready stream will not block), or a message is
 *              ready to be read by NODE_getMsg().
 *
 *  @remarks    @c nstreams indicates the number of stream handles in
 *              @c streamtab[]. The return value @c mask indicates which
 *              streams are ready for I/O.  A 1 in bit position j indicates the
 *              stream @c streamtab[j] is ready. The maximum value allowed for
 *              @c nstreams corresponds to the DSP processor word size, i.e.,
 *              the number of streams that can be represented by the number of
 *              bits in mask.
 *
 *  @remarks    The @c timeout parameter indicates the number of milliseconds
 *              to wait before a stream or message becomes ready. If @c timeout
 *              is 0, NODE_wait() will return immediately. If @c timeout is
 *              @c NODE_FOREVER, NODE_wait() will wait indefinitely.
 *
 *  @remarks    If NODE_wait() returns with @c mask equal to zero, and
 *              @c *messagesReady set to zero, then the function timed out,
 *              with no streams ready yet, and no messages queued.
 *
 *  @remarks    If @c node is specified as @c NULL, the function will not check
 *              for messages, nor pend upon their arrival; that is,
 *              <tt>node = NULL</tt> indicates to the function that it should
 *              only pay attention to the streams specified in @c streamtab[],
 *              and ignore messages.
 *
 *  @note       @c nstreams cannot be larger than the number of bits in @c mask.
 *
 *  @note       @c streamtab must contain handles of type STRM_Handle
 *              returned from prior calls to STRM_create().
 *
 *  @note       If NODE_wait() is called from a context where blocking is not
 *              allowed, (e.g., a software interrupt context), @c timeout
 *              @b must be specified as zero.
 *
 *  @note       NODE_wait() has the same calling restrictions as the
 *              DSP/BIOS @c SEM_pend() service.
 *
 *  @sa         NODE_getMsg(), STRM_reclaim()
 */
extern Uns NODE_wait(NODE_EnvPtr node, STRM_Handle streamtab[], Uns nstreams,
    Uns timeout, Uns * messagesReady);


#ifdef __cplusplus
}
#endif /* extern "C" */


/*@}*/ /* ingroup DSPNODE */

#endif /* NODE_ */
