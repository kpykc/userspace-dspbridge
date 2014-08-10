/*
 *! ============================================================
 *! Copyright 2004 by Texas Instruments Incorporated.
 *! All rights reserved. Property of Texas Instruments Incorporated.
 *! Restricted rights to use, duplicate or disclose this code are
 *! granted through contract.
 *! ============================================================
 */
/**
 *  @file		rmsdefs.h
 *
 *  @brief      DSP Bridge Resource Manager Server External Definitions
 */

/**
 *  @defgroup 	DSPRMSDEFS RMS error codes
 *
 *              The Resource Manager Server (RMS) external header defines RMS
 *              return codes and the RMS message environmet data structure for
 *              nodes.
 */
#ifndef RMSDEFS_
#define RMSDEFS_

/** @ingroup DSPRMSDEFS */
/*@{*/


#ifdef __cplusplus
extern "C" {
#endif


#include <rmstypes.h>
#include <msg.h>

/**
 *  @name	Defines: RMS Return Codes
 *
 *  @note   The RMS error codes are mapped directly to the DSP Bridge API
 *          error codes to eliminate a translation layer.  Any changes must be
 *          synchronized to the API definition, and to the GPP-side
 *          implementation.
 */
/*@{*/
#define RMS_SBASE    0x8000         /**< Base for success codes */
#define RMS_EBASE    0x80008000     /**< Base for error codes */

#define RMS_EOK      (RMS_SBASE + 0)    /**< Success */

#define RMS_EFAIL    (RMS_EBASE + 0x8)  /**< General failure */
#define RMS_EFREE        (RMS_EBASE + 0x2a) /**< Memory free failure */
#define RMS_EIOFREE      (RMS_EBASE + 0x2b) /**< I/O free failure */
#define RMS_EMULINST     (RMS_EBASE + 0x2c) /**< Mult. instances not allowed*/
#define RMS_ENOTFOUND    (RMS_EBASE + 0x2d) /**< Entity not found */
#define RMS_ENOTIMPL     (RMS_EBASE + 0x10) /**< Not implemented yet */
#define RMS_EOUTOFIO     (RMS_EBASE + 0x2e) /**< I/O resource not available */
#define RMS_EOUTOFMEMORY (RMS_EBASE + 0xc)  /**< Memory allocation failed */
#define RMS_ERESOURCE    (RMS_EBASE + 0x28) /**< Resource not available */
#define RMS_ESTREAM      (RMS_EBASE + 0x15) /**< Stream create failed */
#define RMS_ETASK    (RMS_EBASE + 0x16) /**< Task create failed */
#define RMS_ENODEHEAP (RMS_EBASE + 0x17) /**< No more seg Ids for node heap */
#define RMS_EUSER1   (RMS_EBASE + 0x40) /**< Node-specific error 1 */
#define RMS_EUSER2   (RMS_EBASE + 0x41) /**< Node-specific error 2 */
#define RMS_EUSER3   (RMS_EBASE + 0x42) /**< Node-specific error 3 */
#define RMS_EUSER4   (RMS_EBASE + 0x43) /**< Node-specific error 4 */
#define RMS_EUSER5   (RMS_EBASE + 0x44) /**< Node-specific error 5 */
#define RMS_EUSER6   (RMS_EBASE + 0x45) /**< Node-specific error 6 */
#define RMS_EUSER7   (RMS_EBASE + 0x46) /**< Node-specific error 7 */
#define RMS_EUSER8   (RMS_EBASE + 0x47) /**< Node-specific error 8 */
#define RMS_EUSER9   (RMS_EBASE + 0x48) /**< Node-specific error 9 */
#define RMS_EUSER10  (RMS_EBASE + 0x49) /**< Node-specific error 10 */
#define RMS_EUSER11  (RMS_EBASE + 0x4a) /**< Node-specific error 11 */
#define RMS_EUSER12  (RMS_EBASE + 0x4b) /**< Node-specific error 12 */
#define RMS_EUSER13  (RMS_EBASE + 0x4c) /**< Node-specific error 13 */
#define RMS_EUSER14  (RMS_EBASE + 0x4d) /**< Node-specific error 14 */
#define RMS_EUSER15  (RMS_EBASE + 0x4e) /**< Node-specific error 15 */
#define RMS_EUSER16  (RMS_EBASE + 0x4f) /**< Node-specific error 16 */
/*@}*/


/**
 *  @brief      RMS message environment.
 *
 *  @remarks    The RMS_MsgEnv structure represents the environment for a
 *              message node started by the Resource Manager Server. It holds
 *              run-time resources used by the node, including the message
 *              object.
 *
 *  @remarks    The moreEnv field is a pointer to the node's private context.
 *              The node's create function may allocate it's own context
 *              object, and place a pointer to that object in this moreEnv
 *              field.  The node is responsible for deallocating the context
 *              object in its delete function.
 *
 *  @remarks    The RMS allocates the RMS_MsgEnv structure and initializes it.
 *              Nodes should not modify any of the fields in this structure
 *              other than the moreEnv pointer. The RMS will clean up the
 *              enviroment structure after the node delete phase.
 */
typedef struct {
    MSG_Obj     message;        /**< Node message object.               */
    Ptr         moreEnv;        /**< Pointer to node context object.    */
} RMS_MsgEnv;


#ifdef __cplusplus
}
#endif /* extern "C" */


/*@}*/ /* ingroup DSPRMSDEFS */


#endif /* RMSDEFS_ */
