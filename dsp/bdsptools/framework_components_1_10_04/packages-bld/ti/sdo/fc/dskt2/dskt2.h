/* 
 *  Copyright 2007
 *  Texas Instruments Incorporated
 *
 *  All rights reserved.  Property of Texas Instruments Incorporated
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 * 
 */
/**
 *  @file               ti/sdo/fc/dskt2/dskt2.h
 *
 *  @brief              Provides services to support XDAIS algorithm instance objects.
 */

/**
 *  @defgroup   DSPDSKT2 DSKT2
 *
 *              The XDAIS Socket Library provides services to support the
 *              creation, initialization, control, and deletion of XDAIS
 *              algorithm instance objects.
 */

#ifndef DSKT2_
#define DSKT2_

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup DSPDSKT2 */
/*@{*/


#include <ti/xdais/ialg.h>
#include <stddef.h>

#define DSKT2_NUM_SCRATCH_GROUPS 20     /**< Maximum number of scratch groups
                                         *   that can be supported.
                                         */


#define DSKT2_MAX_MEM_ALLOCATION_SIZE  0xFFFF   /**< Maximum memory
                                                 *   allocation.
                                                 */

/* NOTE: DO NOT Change NUM_SHAREDHEAPS*/
#define DSKT2_NUM_SHAREDHEAPS 2

/**
 *  @brief      Function prototype of cache writeback function: Writes back
 *              the memory within the range of blockPtr and blockPtr + byteCnt.
 *
 *  @remarks    If @c wait is @c TRUE, wait for the writeback to complete before
 *              returning from this fuction call.
 */
typedef Void (*DSKT2_CacheWBInvFxn)(Ptr blockPtr, size_t byteCnt, Bool wait);

/**
 *  @brief      Prepares a xDAIS algorithm instance object to start using its
 *              scratch memory.
 *
 *              DSKT2_activateAlg() performs all scratch memory initialization
 *              for an algorithm's instance object. After a successful return
 *              from DSKT2_activateAlg(), the algorithm's instance object is
 *              ready to be used to process data.
 *
 *              Unless the instance is already active, DSKT2_activateAlg()
 *              uses the algorithm's IALG interface function algActivate to
 *              initialize the algorithm instance's scratch buffers from
 *              persistent data memory.
 *
 *              DSKT2_activateAlg() must be called before any processing or
 *              control methods of the algorithm instance can be called.
 *
 *              DSKT2 maintains state information about current "active"
 *              algorithm instances at run-time, so that it does not perform
 *              unnecessary IALG "activation" calls when alg is already active.
 *              As part of this optimization it may call the algDeactivate
 *              method of the currently active algorithm instance in order to
 *              activate this  algorithm instance (alg).
 *
 *  @param[in]  scratchMutexId  scratch-group Id used to associate the instance
 *                              with a scratch group. The value must be the
 *                              same id used when creating this algorithm
 *                              instance.
 *
 *  @param[in]  alg             IALG_Handle of the algorithm instance to be
 *                              activated.
 *
 *  @retval     None
 *
 *  @pre        alg must be a valid handle for the algorithm's instance object
 *              returned by an earlier call to DSKT2_createAlg()
 *
 *  @pre        If alg uses DMA (IDMA2) dmaInit must be called before calling
 *              this methed, ensuring that all of the instance's persistent
 *              memory is initialized and the object is ready to be used
 *
 *  @pre        No other algorithm method is currently being run on this
 *              instance. This method never preempts any other method on the
 *              same instance.
 *
 *  @post       All of the instance's persistent and scratch memory is
 *              initialized and the object is ready to be used.
 *
 *  @remarks    Caller ensures algorithms with the same "scratchMutexId" will
 *              not be activated simultaneously. Usually the scratch group id
 *              is associated to the priority of the task in which the
 *              algorithm is running.
 *
 *  @remarks    The implementation of the IALG algActivate() is optional by
 *              xDAIS standard. So, the instance activation makes sense
 *              only when the method is implemented by the algorithm.
 *              DSKT2_activateAlg() makes proper checks to ensure correct
 *              operation even if the algorithm does not implement the
 *              algActivate() method.
 *
 *  @sa         DSKT2_deactivateAlg()
 *  @sa         DMAN_addAlg()
 */
extern Void DSKT2_activateAlg(Int scratchMutexId, IALG_Handle alg);


/**
 *  @brief      This is a convenience API to call a xDAIS algorithm instance's
 *              algControl function
 *
 *  @param[in]  alg             IALG_Handle of the algorithm instance.
 *  @param[in]  cmd             IALG_Cmd structure for the control operation.
 *  @param[in]  status          Pointer to IALG_Status structure for algorithm
 *                              to return status.
 *
 *  @retval     IALG_EOK        The control operation was successful
 *  @retval     otherwise       A failure occurred during the control operation
 *
 *  @pre        alg must be a valid handle for the algorithm's instance object
 *              returned by an earlier call to DSKT2_createAlg()
 *
 *  @pre        Algorithm specific cmd values are always less than IALG_SYSCMD
 *
 *  @post       If the control operation is successful, the return value from
 *              this operation is equal to IALG_EOK; otherwise it is equal to
 *              either IALG_EFAIL or an algorithm-specific return value
 *
 *  @post       If the cmd value is not recognized, the return value is not
 *              equal to IALG_EOK
 *
 *  @note       DSKT2_controlAlg() must be called only when the instance is in
 *              active state
 *
 *  @note       The implementation of the IALG algControl is optional by XDAIS
 *              standard. DSKT2_controlAlg() makes proper checks to ensure
 *              correct operation even if the algorithm does not implement
 *              algControl.
 */
extern Int DSKT2_controlAlg(IALG_Handle alg, IALG_Cmd cmd,
    IALG_Status *status);


/**
 *  @brief      Creates and initializes a xDAIS algorithm instance object.
 *
 *              It uses the algorithm's IALG interface functions to query the
 *              algorithm for its memory needs then allocates the memory for
 *              the algorithm and calls the algorithm's algInit function to
 *              let the new algorithm instance object initialize itself using
 *              the allocated memory.
 *
 *              DSKT2_createAlg() performs initialization necessary to complete
 *              the run-time creation of an algorithm's instance object. After
 *              a successful return from DSKT2_createAlg(), the algorithm's
 *              instance object can be activated via a DSKT2_activateAlg()
 *              before it can be used to process data.
 *
 *              DSKT2_createAlg() tries to dynamically allocate instance memory
 *              based on the IALG_MemSpace attribute of the requested memory.
 *              Global DSKT2 configuration settings allow OEM to designate a
 *              memory heap for each IALG_MemSpace. DSKT2_createAlg() attempts
 *              to allocate memory in the requested space, but may search for
 *              alternative heaps when preferred heap is not large enough.
 *
 *  @param[in]  scratchMutexId  Scratch-group Id for determining the shared
 *                              scratch memory buffer. Value -1 is used to
 *                              disable scratch sharing for this algorithm
 *                              instance. A value between 0 and
 *                              (DSKT2_NUM_SCRATCH_GROUPS-1) indicates a valid
 *                              scratch group
 *  @param[in]  fxns            Pointer to the algorithm's IALG_Fxns table.
 *  @param[in]  parent          Handle of parent algorithm (optional).
 *  @param[in]  params          Pointer to an IALG_Params structure.
 *
 *  @retval     non-NULL        IALG_Handle for the new instance object.
 *  @retval     NULL            Failure.
 *
 *  @pre        fxns is a valid pointer to an IALG_Fxns structure
 *              (containing the vtable for a XDAIS compliant algorithm)
 *
 *  @post       If scratchMutexId is -1, all instance scratch memory is
 *              allocated as persistent (i.e. not shared) and instance
 *              operation does not require mutual exclusion
 *
 *  @post       With the exception of any initialization performed by
 *              algActivate and (IDMA2) dmaInit all of the instance's
 *              persistent memory is initialized and the object is ready to be
 *              used
 *
 *  @note       The parent argument is a handle to another algorithm instance
 *              object. This parameter is often NULL, indicating that no parent
 *              object exists.  This parameter allows clients to create a
 *              shared algorithm instance object and pass it to other algorithm
 *              instances. For example, a parent instance object might contain
 *              global read-only tables that are used by several instances of a
 *              vocoder.
 *
 *  @remark     Caller need to ensure algorithms created with the same
 *              "scratchMutexId" will not execute simultaneously
 *
 *  @remark     On failure all memory allocated during the call (used for
 *              algorithm query, and portions of algorithm memory that were
 *              successfully allocated during the function) has been freed
 *              before DSKT2_createAlg() returns
 *
 *  @note       The params argument is a pointer to algorithm-specific
 *              parameters that are necessary for the creation and
 *              initialization of the instance object. This pointer points to
 *              the same parameters passed to the algorithm's IALG algAlloc
 *              function. However, this pointer may be NULL. In this case,
 *              algorithm's IALG function algInit, must assume default creation
 *              parameters.
 *
 *  @sa         DSKT2_activateAlg(), DSKT2_deactivateAlg(), DSKT2_freeAlg()
 */
extern IALG_Handle DSKT2_createAlg(Int scratchMutexId, IALG_Fxns * fxns,
    IALG_Handle parent, IALG_Params * params);


/**
 *  @brief      Creates and initializes a xDAIS algorithm instance object.
 *
 *              It uses the algorithm's IALG interface functions to query the
 *              algorithm for its memory needs then allocates the memory for
 *              the algorithm and calls the algorithm's algInit function to
 *              let the new algorithm instance object initialize itself using
 *              the allocated memory.
 *
 *              DSKT2_createAlg2() performs initialization necessary to complete
 *              the run-time creation of an algorithm's instance object. After
 *              a successful return from DSKT2_createAlg2(), the algorithm's
 *              instance object can be activated via a DSKT2_activateAlg()
 *              before it can be used to process data.
 *
 *              DSKT2_createAlg2() tries to dynamically allocate instance memory
 *              based on the IALG_MemSpace attribute of the requested memory.
 *              Global DSKT2 configuration settings allow OEM to designate a
 *              memory heap for each IALG_MemSpace. DSKT2_createAlg2() attempts
 *              to allocate memory in the requested space, but may search for
 *              alternative heaps when preferred heap is not large enough.
 *
 *              DSKT2_createAlg2() will attempt to use the heap specified by
 *              the input parameter, extHeapId, for all allocations in the
 *              IALG_MemSpace, IALG_ESDATA. Furthermore, this holds for
 *              any IALG_MemSpace that has been configured with the same
 *              memory segment as IALG_ESDATA. e.g., if IALG_ESDATA and
 *              IALG_SARAM3 have both been configured to use the heap in the
 *              memory segment EXTMEM, then DSKT2_createAlg2() will attempt
 *              to allocate memory requested in the spaces IALG_SARAM3
 *              and IALG_ESDATA, from the heap, extHeapId.
 *
 *  @param[in]  scratchMutexId  Scratch-group Id for determining the shared
 *                              scratch memory buffer. Value -1 is used to
 *                              disable scratch sharing for this algorithm
 *                              instance. A value between 0 and
 *                              (DSKT2_NUM_SCRATCH_GROUPS-1) indicates a valid
 *                              scratch group
 *  @param[in]  fxns            Pointer to the algorithm's IALG_Fxns table.
 *  @param[in]  parent          Handle of parent algorithm (optional).
 *  @param[in]  params          Pointer to an IALG_Params structure.
 *  @param[in]  extHeapId       Segment Id of memory heap to be used for all
 *                              allocations in memory space IALG_ESDATA. This
 *                              segment will be used instead of the default
 *                              external memory segment.
 *
 *  @retval     non-NULL        IALG_Handle for the new instance object.
 *  @retval     NULL            Failure.
 *
 *  @pre        fxns is a valid pointer to an IALG_Fxns structure
 *              (containing the vtable for a XDAIS compliant algorithm)
 *
 *  @pre        extHeapId >= 0
 *
 *  @post       If scratchMutexId is -1, all instance scratch memory is
 *              allocated as persistent (i.e. not shared) and instance
 *              operation does not require mutual exclusion
 *
 *  @post       With the exception of any initialization performed by
 *              algActivate and (IDMA2) dmaInit all of the instance's
 *              persistent memory is initialized and the object is ready to be
 *              used
 *
 *  @note       The parent argument is a handle to another algorithm instance
 *              object. This parameter is often NULL, indicating that no parent
 *              object exists.  This parameter allows clients to create a
 *              shared algorithm instance object and pass it to other algorithm
 *              instances. For example, a parent instance object might contain
 *              global read-only tables that are used by several instances of a
 *              vocoder.
 *
 *  @remark     Caller need to ensure algorithms created with the same
 *              "scratchMutexId" will not execute simultaneously
 *
 *  @remark     On failure all memory allocated during the call (used for
 *              algorithm query, and portions of algorithm memory that were
 *              successfully allocated during the function) has been freed
 *              before DSKT2_createAlg() returns
 *
 *  @note       The params argument is a pointer to algorithm-specific
 *              parameters that are necessary for the creation and
 *              initialization of the instance object. This pointer points to
 *              the same parameters passed to the algorithm's IALG algAlloc
 *              function. However, this pointer may be NULL. In this case,
 *              algorithm's IALG function algInit, must assume default creation
 *              parameters.
 *
 *  @sa         DSKT2_activateAlg(), DSKT2_deactivateAlg(), DSKT2_freeAlg()
 */
extern IALG_Handle DSKT2_createAlg2(Int scratchMutexId, IALG_Fxns * fxns,
    IALG_Handle parent, IALG_Params * params, Int extHeapId);


/**
 *  @brief      Prepares a xDAIS algorithm instance object to give up using
 *              its scratch memory
 *
 *              DSKT2_deactivateAlg() marks an algorithm's shared scratch
 *              memory as available to other instances activation. After a
 *              successful return from DSKT2_deactivateAlg(), the algorithm's
 *              processing or control functions cannot be called to process
 *              data.
 *
 *              Object's deactivation logic involves calling the algorithm's
 *              IALG interface function algDeactivate to save necessary data
 *              from the algorithm instance's scratch buffers to its persistent
 *              data memory.
 *
 *              DSKT2_deactivateAlg() must be called after the last processing
 *              or control methods of the algorithm instance, alg, during each
 *              execute stage of its socket operation.
 *
 *              DSKT2 maintains state information about current "active"
 *              algorithm instances at run-time, so that it does not perform
 *              unnecessary IALG "deactivation" calls. As part of this
 *              optimization it may defer the deactivation of this algorithm
 *              instance (alg) to a later stage, i.e. when DSKT2_activateAlg()
 *              is called to activate another algorithm instance.
 *
 *  @param[in]  scratchMutexId  scratch-group Id used for determining the
 *                              shared scratch memory.
 *  @param[in]  alg             IALG_Handle of the algorithm instance to be
 *                              deactivated.
 *
 *  @retval     None
 *
 *  @pre        alg must be a valid handle for the algorithm's instance object
 *              returned by an earlier call to DSKT2_createAlg()
 *
 *  @pre        No other algorithm method is currently being run on this
 *              instance. This method never preempts any other method on the
 *              same instance.
 *
 *  @pre        DSKT2_activateAlg() must be called before calling this method
 *
 *  @note       The implementation of the IALG algDeactivate is optional by
 *              XDAIS standard. So, the instance deactivation makes sense only
 *              when the method is implemented by the algorithm.
 *              DSKT2_deactivateAlg() makes proper checks to ensure correct
 *              operation even if the algorithm does not implement the
 *              algDeactivate methods.
 *
 *  @sa         DSKT2_activateAlg()
 *
 */
extern Void DSKT2_deactivateAlg(Int scratchMutexId, IALG_Handle alg);

/**
 *  @brief      All algorithms that have been deactivated lazily are now
 *              really deactivate. Any algorithms that are still currently
 *              active will be left as is. The number of algorithms that are
 *              still active will be returned from this call.
 *
 *  @retval     >=0   The number of currently active algorithms
 *
 *  @pre        Must be called with the TSK and SWI schedulers disabled.
 *
 */
extern Int DSKT2_deactivateAll();

/**
 *  @brief      Deletes a xDAIS algorithm instance object and frees all
 *              persistent memory allocated for the instance object
 *
 *              A reference counting mechanism is implemented to free up
 *              instance scratch memory so that when the last instance within a
 *              scratch group is deleted all shared scratch memory allocated
 *              for the group is reclaimed.
 *
 *              DSKT2_freeAlg() must be called during delete stage of a
 *              socket's operation to prevent memory leaks.
 *
 *  @param[in]  scratchMutexId  scratch-group Id used for determining the
 *                              shared scratch memory. Must be the same Id
 *                              used when algorithm instance is created
 *  @param[in]  alg             IALG_Handle of the algorithm instance to be
 *                              deleted
 *
 *  @retval     TRUE            Success
 *  @retval     FALSE           Failure
 *
 *  @pre        alg must be a valid handle for the algorithm's instance object
 *              returned by an earlier call to DSKT2_createAlg()
 *
 *  @note       DSKT2 maintains allocation information about all algorithm
 *              instances created by DSKT2_createAlg(), so it does not call the
 *              instance algFree method before free'ing instance memory.
 *
 *  @sa         DSKT2_createAlg()
 */
extern Bool DSKT2_freeAlg(Int scratchMutexId, IALG_Handle alg);


/*@}*/

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  /* DSKT2_ */

/*
 *  @(#) ti.sdo.fc.dskt2; 1, 0, 1,66; 5-10-2007 10:50:09; /db/atree/library/trees/fc-b12x/src/
 */

