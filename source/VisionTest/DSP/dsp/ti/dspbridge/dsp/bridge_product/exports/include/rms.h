/*
 *! ============================================================
 *! Copyright 2005 by Texas Instruments Incorporated.
 *! All rights reserved. Property of Texas Instruments Incorporated.
 *! Restricted rights to use, duplicate or disclose this code are
 *! granted through contract.
 *! ============================================================
 */
/**
 *  @file       rms.h
 *
 *  @brief      DSP Bridge DSP-side Resource Manager Server module
 *              definitions.
 */

/**
 *  @defgroup   DSPRMS RMS
 *
 *              RMS stands for Resource Manager Server, a DSP-side task created
 *              by DSP Bridge at runtime to handle node related commands and
 *              messages sent by the MPU-side Resource Manager.  It is primarily
 *              responsible for the creation, execution, and deletion of
 *              nodes on the DSP.
 */

#ifndef RMS_
#define RMS_

/** @ingroup DSPRMS */
/*@{*/


#ifdef __cplusplus
extern "C" {
#endif


#include <msg.h>
#include <tsk.h>
#include <rms_sh.h>
#include <rmsdefs.h>

#define RMS_MINPRIORITY     -1  /**< Minimum allowable priority level for
                                   DSP Bridge Task and XDAIS nodes. */
#define RMS_MAXPRIORITY     15  /**< Maximum allowable priority level for
                                   DSP Bridge Task and XDAIS nodes. */

/**
 *  @brief   	Variable defining the number of microseconds per DSP timer
 * 				tick.
 */
extern Uns RMS_msecPerTick;


/**
 *  @brief      Server function used to change the priority of a Task node or
 *              XDAIS socket node.
 *
 *  @param[in]  pEnv        Pointer to the node's environment structure.
 *  @param[in]  iPriority   New priority for the node:
 *                          <tt>RMS_MINPRIORITY <= iPriority <=
 *                          RMS_MAXPRIORITY).</tt>
 *  @param[in]  dummy       Unused argument; pads function signature.
 *
 *  @pre        <tt>pEnv != NULL</tt>
 *  @pre        <tt>RMS_MINPRIORITY <= iPriority <= RMS_MAXPRIORITY</tt>
 *
 *  @remarks    The RM can use this function to suspend, resume, or simply
 *              re-prioritize a node.  The function is invoked by the RM when
 *              an application calls:
 *              - @c DSPNode_Pause(), to transition the node from the @b Running
 *                state to the @b Paused state,
 *              - @c DSPNode_Run(), to transition the node from the @b Paused
 *                state to the @b Running state, and
 *              - @c DSPNode_ChangePriority(), when a node is in the @b Running
 *                state and its runtime task priority is to be changed.
 *
 *  @remarks    The function returns nothing to the caller, but does reply to
 *              the RM via an internal RMS call to @c _RMS_replyToHost().
 *
 *  @note       The node's priority level is tracked only by the RM on the GPP;
 *              the node on the DSP does @b NOT keep track of changes to its
 *              priority level.  There is a @c 'priority' field in the node's
 *              secondary environment (@c _RMS_TaskEnviron), but this field is
 *              only used to initially get the task running, and is not updated
 *              when the RMS invokes RMS_changeNodePriority().
 */
extern Void RMS_changeNodePriority(RMS_WORD pEnv, RMS_WORD iPriority,
    RMS_WORD dummy);


/**
 *  @brief      Server function used by the RM to change configuration
 *              parameters of the RM Server (RMS) at runtime.
 *
 *  @param[in]  iServerParam    The server configuration parameter to be
 *                              changed.
 *  @param[in]  uValue          New value for the configuration parameter.
 *  @param[in]  pData           Additional configuration data.
 *
 *  @remarks    This function is for future use.  It returns nothing to the
 *              caller, but does reply to the RM via an internal RMS call to
 *              @c _RMS_replyToHost().
 */
extern Void RMS_configureServer(RMS_WORD iServerParam, RMS_WORD uValue,
    RMS_WORD pData);


/**
 *  @brief      Server function to copy memory from a DSP source address
 *              to a DSP destination address.
 *
 *  @param[in]  upDestAddr  The DSP destination address to write to.
 *  @param[in]  upDestSpace The memory space to be written.
 *  @param[in]  upSize      Pointer to the size of the data block and ensuing
 *                          data, which contains the DSP source address.
 *
 *  @remarks    This function returns nothing to the caller, but does reply to
 *              the RM via an internal call to @c _RMS_replyToHost().
 */
extern Void RMS_copy(RMS_WORD upDestAddr, RMS_WORD upDestSpace,
    RMS_WORD upSize);


/**
 *  @brief      Server function used to instantiate a node on the DSP.
 *
 *
 *  @param[in]  pCreateFxn  Pointer to the node's create-phase function.
 *  @param[in]  dummy       Unused argument; pads function signature.
 *  @param[in]  pCreateArgs Pointer to a block of create-phase arguments for the
 *                          node.
 *
 *  @pre    <tt>pCreateFxn != NULL</tt>
 *  @pre    <tt>pCreateArgs != NULL</tt>
 *
 *  @remarks    The function instantiates a node as follows:
 *              - Allocate a node environment structure.
 *              - Allocate messaging resources.
 *              - For a Task or XDAIS socket node, create the node's thread.
 *              - Prepare stream definition arrays.
 *              - Invoke the node's create-phase function.
 *
 *  @remarks    This function returns nothing to the caller, but does reply
 *              to the RM via an internal call to @c _RMS_replyToHost().
 *
 *  @note       It is assumed that @b if RMS_createNode() is called for a
 *              node, RMS_deleteNode() will also be called for the node, even
 *              if a failure occurs during node creation.  This allows RMS
 *              cleanup code to reside in one place (RMS_deleteNode()), and
 *              node-specific cleanup to reside in one place (the node's own
 *              delete-phase function).
 *
 *  @note       For example, if a node's create-phase function detects an
 *              allocation error, it can exit the function immediately without
 *              performing cleanup of already allocated resources, as these will
 *              be freed by the node's delete-phase function.
 *
 *  @note       The basic implications are:
 *              - The Node Dispatcher on the GPP will -- and should -- invoke
 *                RMS_deleteNode() even if a failure occurs in RMS_createNode.
 *              - RMS_createNode() and RMS_deleteNode() are implemented to deal
 *                with partial success in RMS_createNode.
 *              - A node's create and delete functions should be structured to
 *                deal with partial success (e.g., the delete function should
 *                check to ensure a resource is non-NULL before freeing the
 *                resource).
 *
 *  @sa         RMS_executeNode()
 *  @sa         RMS_deleteNode()
 */
extern Void RMS_createNode(RMS_WORD pCreateFxn, RMS_WORD dummy,
    RMS_WORD pCreateArgs);


/**
 *  @brief      Server function used to delete a node from the DSP.
 *
 *  @param[in]  pEnv        Pointer to the node's environment structure.
 *  @param[in]  pDeleteFxn  Pointer to the node's delete-phase function.
 *  @param[in]  pNodeType   The type of node that is being deleted.
 *
 *  @pre        <tt>pDeleteFxn != NULL</tt>
 *
 *  @remarks    The function deletes a node as follows:
 *              - Invoke the node's delete phase function.
 *              - For a Task or XDAIS socket node, delete the node's thread.
 *              - Free the node's messaging resources.
 *              - Free the node's environment.
 *              - Reply to the RM upon completion of node deletion.
 *
 *  @sa         RMS_createNode()
 *  @sa         RMS_executeNode()
 */
extern Void RMS_deleteNode(RMS_WORD pEnv, RMS_WORD pDeleteFxn,
    RMS_WORD pNodeType);


/**
 *  @brief      Server function used to launch a node into its execute phase.
 *
 *  @param[in]  pEnv        Pointer to the node's environment structure.
 *  @param[in]  pExecuteFxn Pointer to the node's execute-phase function.
 *  @param[in]  pNodeType   The type of node being launched.
 *
 *  @pre        <tt>pEnv != NULL</tt>
 *  @pre        <tt>pExecuteFxn != NULL</tt>
 *
 *  @remarks    For a Task or XDAIS socket node, the function will launch a node
 *              as follows:
 *              - Reply to the RM to acknowledge the execute command.
 *              - Change the node's priority level from inactive to its runtime
 *                priority, so as to wake up the node's task stub.
 *
 *  @remarks    For a Message node, the function will launch a node as follows:
 *              - Invoke the node's execute-phase function.
 *              - Reply to the RM, passing it the node execute-phase function's
 *                return code.
 *
 *  @sa         RMS_createNode()
 *  @sa         RMS_deleteNode()
 */
extern Void RMS_executeNode(RMS_WORD pEnv, RMS_WORD pExecuteFxn,
    RMS_WORD pNodeType);


/**
 *  @brief      Server function used by the RM to query the RM Server for
 *              settings and capabilities.
 *
 *  @param[in]  iServerParam    The server configuration parameter to be
 *                              queried.
 *  @param[in]  dummy           Unused argument; pads function signature.
 *  @param[in]  dummy2          Unused argument; pads function signature.
 *
 *  @remarks    This function is for future use.  The function returns nothing
 *              to the caller, but does reply to the RM -- returning the
 *              queried settings and capabilities -- via an internal call to
 *              @c _RMS_replyToHost().
 */
extern Void RMS_queryServer(RMS_WORD iServerParam, RMS_WORD dummy,
    RMS_WORD dummy2);


/**
 *  @brief      Server function used by the RM to read a single word of data
 *              from a specified DSP memory address and space.
 *
 *  @param[in]  upSourceAddr    The DSP address to read from.
 *  @param[in]  upSourceSpace   The DSP memory space to be read.
 *  @param[in]  dummy           Unused argument; pads function signature.
 *
 *  @remarks    This function returns nothing to the caller, but does reply to
 *              the RM -- returning a success/fail code and the requested
 *              value -- via an internal call to @c _RMS_replyToHost().
 *
 *  @sa         RMS_writeMemory()
 */
extern Void RMS_readMemory(RMS_WORD upSourceAddr, RMS_WORD upSourceSpace,
    RMS_WORD dummy);


/**
 *  @brief      RM Server Dispatcher's RPC loop.  The DSP base image has a
 *              statically allocated server task; RMS_rpc() is this task's
 *              execute function.
 *
 *  @pre        The address of the server function to be run is non-NULL.
 */
extern Void RMS_rpc(Void);


/**
 *  @brief      Server function used by the RM to write a block of data to
 *              a specified DSP memory address and space.
 *
 *  @remarks    This function returns nothing to the caller, but does reply to
 *              the RM via an internal call to @c _RMS_replyToHost().
 *
 *  @param[in]  upDestAddr  The DSP address to write to.
 *  @param[in]  upDestSpace The DSP memory space to be written.
 *  @param[in]  upSize      Pointer to the size of the data block and ensuing
 *                          data.
 *
 *  @sa         RMS_readMemory()
 */
extern Void RMS_writeMemory(RMS_WORD upDestAddr, RMS_WORD upDestSpace,
    RMS_WORD upSize);

/**
 *  @brief   	semaphore handle for RMS_terminateTask
 */	 
extern SEM_Handle termtsk_sem ;

/**
 *  @brief   	hanging task's node environment pointer
 */ 
extern RMS_WORD KillTaskNodeEnvPtr;

#ifdef __cplusplus
}
#endif /* extern "C" */


/*@}*/ /* ingroup DSPRMS */


#endif /* RMS_ */
