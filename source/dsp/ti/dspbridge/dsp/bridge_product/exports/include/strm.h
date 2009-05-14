/*
 *! ============================================================
 *! Copyright 2004 by Texas Instruments Incorporated.
 *! All rights reserved. Property of Texas Instruments Incorporated.
 *! Restricted rights to use, duplicate or disclose this code are
 *! granted through contract.
 *! ============================================================
 */
/**
 *  @file       strm.h
 *
 *  @brief      DSP Bridge STRM API Definitions - Provides data streaming
 *              services.
 */

/**
 *  @defgroup   DSPSTRM STRM
 *
 *              The DSP Bridge STRM API provides data streaming services.
 */


#ifndef STRM_
#define STRM_


/** @ingroup DSPSTRM */
/*@{*/


#ifdef __cplusplus
extern "C" {
#endif


#include <dev.h>

/* pre-defined STRM return codes: */
#define STRM_OK         0       /**< Success */
#define STRM_EALLOC     1       /**< Memory allocation error */
#define STRM_EFREE      2       /**< Unable to free memory */
#define STRM_ENODEV     3       /**< Device not found */
#define STRM_EBUSY      4       /**< Already opened by max number users */
#define STRM_EINVAL     5       /**< Invalid parameter passed to device */
#define STRM_EBADIO     6       /**< Device unable to support operation */
#define STRM_EMODE      7       /**< Attempt to open in wrong mode */
#define STRM_EDOMAIN    8       /**< Operation is not supported */
#define STRM_ETIMEOUT   9       /**< Timeout occurred before completion */
#define STRM_EEOF       10      /**< Device indicated end-of-file */
#define STRM_EDEAD      11      /**< Attempt to use deleted object */
#define STRM_EBADOBJ    12      /**< Attempt to use non-existent object */
#define STRM_EUSER      256     /**< Base of user-defined errors */

/**
 *  @brief Opaque handle to a stream.
 */
typedef Void * STRM_Handle;


/**
 *  @brief Stream direction - streams are uni-directional.
 */
typedef enum {
    STRM_INPUT = DEV_INPUT,     /**< Input stream */
    STRM_OUTPUT = DEV_OUTPUT    /**< Output stream */
} STRM_DIRECTION;

/**
 *  @brief      Stream Attributes
 */
typedef struct STRM_Attrs {
    Int     nbufs;      /**< The maximum number of buffers that can be
                           outstanding (i.e., issued but not reclaimed)
                           at any point in time.  The default value of
                           nbufs is 2. */
    Int     segid;      /**< The memory segment used for stream
                           buffer allocation.  The default value is 0,
                           meaning that buffers will be allocated from
                           that segment. */
    Int     align;      /**< The memory alignment for stream buffers.
                           The default value is 0 or 1, meaning that no
                           alignment is needed. Otherwise, value must be set
                           to a power of 2. */
    Uns     timeout;    /**< The timeout value (in milliseconds) for
                           blocking calls on this stream.  A value of 0
                           means no blocking; a value of @c SYS_FOREVER
                           means an infinite block. */
} STRM_Attrs;

/**
 *  @brief      Variable containing default stream attributes.
 */
extern STRM_Attrs STRM_ATTRS;


/**
 *  @brief      Allocate and prepare a data buffer for use with a stream.
 *
 *  @param[in]  stream      Handle of the stream, as returned from a successful
 *                          call to STRM_create().
 *  @param[in]  size        Size (in DSP MAUs) of the buffer to be allocated.
 *
 *  @retval     non-NULL    Address of the allocated buffer.
 *  @retval     NULL        Failure.
 *
 *  @remarks    STRM_allocateBuffer() allocates a contiguous block of memory,
 *              size minimum addressable units long.  The alignment, and memory
 *              segment from which the buffer should be allocated were
 *              specified when the stream was opened, i.e., these attributes
 *              were defined via an STRM_Attrs structure when STRM_create()
 *              was called.  STRM_allocateBuffer() also does any necessary
 *              preparation of the buffer for use with the stream, e.g.,
 *              page-locking, if appropriate.
 *
 *  @remarks    STRM_allocateBuffer() does not initialize the contents of the
 *              allocated buffer; data intialization is left to the client
 *              application.
 *
 *  @remarks    If an allocation failure occurs, STRM_allocateBuffer() returns
 *              an address of @c NULL.
 *
 *  @remarks    Buffers allocated with STRM_allocateBuffer() must be freed by
 *              the application via calls to STRM_freeBuffer().
 *
 *  @note       A node should only call this function during its create phase.
 *
 *  @sa         STRM_create(), STRM_freeBuffer()
 */
extern Ptr STRM_allocateBuffer(STRM_Handle stream, LgUns size);

/**
 *  @name       Defines: STRM Macro Function
 */
/*@{*/
/**
 *  @brief      Perform a device-dependent control operation on the underlying
 *              device.
 *
 *  @param[in]  stream  Handle of the stream, as returned from a successful
 *                      call to STRM_create().
 *  @param[in]  cmd     Command to the device (device specific).
 *  @param[in]  arg     Aribitrary argument.
 *
 *  @retval     STRM_OK Success.
 *  @retval     other   Device-dependent error code.
 *
 *  @sa         STRM_create()
 */
#define STRM_control(stream, cmd, arg)  DEV_ctrl((DEV_Handle)stream, cmd, arg)
/*@}*/


/**
 *  @brief      Create a new stream object and open the underlying device.
 *
 *  @param[in]  name        An ANSI string containing the name of the device to
 *                          open.
 *  @param[in]  mode        Stream direction (#STRM_INPUT or #STRM_OUTPUT).
 *  @param[in]  bufsize     Size of buffers that will be used with the stream.
 *  @param[in]  attrs       Pointer to a stream attributes structure.
 *
 *  @retval     Non-NULL    The handle of the newly created stream.
 *  @retval     NULL        Failure.
 *
 *  @remarks    STRM_create() creates a new stream object and opens the device
 *              specified by name.  If successful, STRM_create() returns the
 *              handle of the new stream object.  If unsuccessful, STRM_create()
 *              returns @c NULL.
 *
 *  @remarks    @c mode specifies whether the stream will be used for input
 *              (#STRM_INPUT) or output (#STRM_OUTPUT).
 *
 *  @remarks    @c bufsize is used by devices that internally allocate
 *              supplementary buffers needed to perform their processing,
 *              (i.e., buffers in addition to those submitted to the stream by
 *              the stream client).  Also, some devices use the bufsize
 *              parameter for error checking purposes. The buffer size for the
 *              stream is defined in the RMS_StrmDef structure passed to the
 *              node from the GPP; your program should extract @c bufsize from
 *              the RMS_StrmDef structure, and use this same value in the
 *              STRM_create() call.
 *
 *  @remarks    STRM_create() does not allocate any buffers for the stream. All
 *              buffers must be allocated by the client (by calling
 *              STRM_allocateBuffer()), and passed to the stream with
 *              STRM_issue() calls.  STRM_create() does, however,
 *              prepare the stream for a maximum number of buffers
 *              specified by STRM_Attrs::nbufs.
 *
 *  @remarks    If the @c attrs parameter is @c NULL, the new stream is
 *              assigned the default set of attributes specified by
 *              STRM_ATTRS.  See STRM_Attrs for these default values.
 *
 *  @note       A node should only call this function during its create phase.
 *
 *  @note       @c stream can only be used by one task simultaneously.
 *              Unpredictable failure may result if more than one task calls
 *              STRM_issue() / STRM_reclaim() using the same input or output
 *              stream handle.
 *
 *  @sa         STRM_delete(), STRM_issue(), STRM_reclaim(), STRM_Attrs,
 *              STRM_DIRECTION
 */
extern STRM_Handle STRM_create(String name, STRM_DIRECTION mode,
    LgUns bufsize, STRM_Attrs *attrs);


/**
 *  @brief      Close a stream and free its resources.
 *
 *  @param[in]  stream  Handle of the stream, as returned from a successful
 *                      call to STRM_create().
 *
 *  @retval     STRM_OK Success.
 *  @retval     other   Failure.  Error code depends upon device driver
 *                      implementation, and may include driver-specific errors.
 *
 *  @remarks    STRM_delete() idles the device before freeing the stream object
 *              and buffers.
 *
 *  @remarks    When STRM_delete() is called, all pending stream data will be
 *              discarded.
 *
 *  @note       A node should only call this function during its delete phase.
 *
 *  @sa         STRM_create(), STRM_idle().
 */
extern Int STRM_delete(STRM_Handle stream);


/**
 *  @brief      Release a stream data buffer previously allocated with
 *              STRM_allocateBuffer().
 *
 *  @param[in]  stream  Handle of the stream, as returned from a successful
 *                      call to STRM_create().
 *  @param[in]  addr    Address of the buffer, as returned from a successful
 *                      call to STRM_allocateBuffer().
 *  @param[in]  size    Size (in DSP MAUs) of the buffer to be freed.
 *
 *  @retval     TRUE    Success.
 *  @retval     FALSE   Failure.
 *
 *  @remarks    STRM_freeBuffer() releases a stream buffer allocated by
 *              STRM_allocateBuffer() back to the free memory pool.
 *
 *  @note       This function should only be called to free a buffer that was
 *              successfully allocated with STRM_allocateBuffer().
 *
 *  @sa         STRM_allocateBuffer()
 */
extern Bool STRM_freeBuffer(STRM_Handle stream, Void * addr, LgUns size);


/**
 *  @brief      Idle a stream.
 *
 *  @param[in]  stream  Handle of the stream, as returned from a successful
 *                      call to STRM_create().
 *  @param[in]  flush   Flag indicating if an output stream's data should be
 *                      discarded.
 *
 *  @retval     STRM_OK Success.
 *  @retval     other   Failure.  Error code depends upon device driver
 *                      implementation, and may include driver-specific errors.
 *
 *  @remarks    If @c stream is being used for input, STRM_idle() causes any
 *              currently buffered data to be discarded.
 *
 *  @remarks    If @c stream is being used for output, the operation depends on
 *              the value of the @c flush flag:
 *              <ul>
 *                <li>If @c flush is @c FALSE, STRM_idle() causes any currently
 *                      buffered data to be transferred to the output device
 *                      associated with @c stream.  STRM_idle() will suspend
 *                      program execution for as long as is required for the
 *                      data to be consumed by the underlying device.
 *                </li>
 *                <li>If @c flush is @c TRUE, STRM_idle() causes any currently
 *                      buffered output data to be discared without blocking.
 *                </li>
 *              </ul>
 *
 *  @remarks    The underlying device connected to @c stream will be idled as a
 *              result of calling STRM_idle().  In general, the interrupt will
 *              be disabled for this device.
 *
 *  @remarks    One of the purposes of this function is to provide
 *              synchronization with the external environment.
 *
 *  @sa         STRM_create()
 */
extern Int STRM_idle(STRM_Handle stream, Bool flush);


/**
 *  @brief      Submit a buffer to a stream.
 *
 *  @param[in]  stream      Handle of the stream, as returned from a
 *                          successful call to STRM_create().
 *  @param[in]  pbuf        Pointer to the buffer.
 *  @param[in]  datasize    Amount of valid data in the buffer (in DSP
 *                          MAUs).
 *  @param[in]  bufsize     Actual buffer size (in DSP MAUs) of buffer
 *                          pointed to by @c pbuf.
 *  @param[in]  arg         Arbitrary user argument.
 *
 *  @retval     STRM_OK     Success.
 *  @retval     STRM_EBADIO No frames available for the buffer.
 *  @retval     other       Failure.  Error code depends upon device driver
 *                          implementation, and may include driver-specific
 *                          errors.
 *
 *  @remarks    STRM_issue() is used to send a buffer and its related
 *              information to a stream. The buffer-related information
 *              consists of the logical length of the buffer (datasize), the
 *              actual (allocated) size of the buffer (bufsize), and the user
 *              argument to be associated with that buffer.  STRM_issue() will
 *              send a buffer to the stream and return to the caller without
 *              blocking.  It will also return a code indicating success
 *              (#STRM_OK), or failure of the call.
 *
 *  @remarks    Failure of STRM_issue() indicates that the stream was not able
 *              to accept the buffer being issued or that there was an
 *              underlying device error.  In the first case, the application is
 *              probably issuing more buffers than the maximum STRM_Attrs::nbufs
 *              allowed for the stream.  In the second case, the failure
 *              reveals an underlying device driver or hardware problem.  If
 *              STRM_issue() fails, STRM_idle() should be called (with flush
 *              set to @c TRUE), before attempting more I/O through the stream.
 *
 *  @remarks    The interpretation of @c datasize, the logical size of a
 *              buffer, is direction-dependent. For a stream opened in
 *              #STRM_OUTPUT mode, the logical size of the buffer indicates the
 *              amount of valid data it contains. For a stream opened in
 *              #STRM_INPUT mode, the logical length of a buffer indicates the
 *              amount of data being requested by the client.  In either case,
 *              the logical size of the buffer must be less than or equal to
 *              the physical size of the buffer.
 *
 *  @remarks    The argument @c arg is not interpreted by STRM, but is offered
 *              as a service to the stream client. All drivers should preserve
 *              the value of @c arg and maintain its association with the data
 *              that it was issued with.  @c arg provides a user argument as a
 *              method for a client to associate additional information with a
 *              particular buffer of data.
 *
 *  @remarks    STRM_issue() is used in conjunction with STRM_reclaim(). The
 *              STRM_issue() call sends a buffer to a stream, and STRM_reclaim()
 *              retrieves a buffer from a stream.  In normal operation each
 *              STRM_issue() call is followed by a STRM_reclaim() call. Short
 *              bursts of multiple STRM_issue() calls can be made without an
 *              intervening STRM_reclaim() call, but over the life of the
 *              stream, STRM_issue() and STRM_reclaim() should be called the
 *              same number of times.
 *
 *  @remarks    At any given point in the life of a stream, the number of
 *              STRM_issue() calls can exceed the number of STRM_reclaim()
 *              calls by a maximum of STRM_Attrs::nbufs. The value of
 *              STRM_Attrs::nbufs is determined by the STRM_create() call.
 *
 *  @remarks    NOTE: A STRM_reclaim() call should not be made without at least
 *              one outstanding STRM_issue() call.  Calling STRM_reclaim() with
 *              no outstanding STRM_issue() calls has undefined results.
 *
 *  @sa         STRM_create(), STRM_reclaim()
 */
extern Int STRM_issue(STRM_Handle stream, Ptr pbuf, LgUns datasize,
    LgUns bufsize, Arg arg);


/**
 *  @brief      Request a buffer back from a stream.
 *
 *  @param[in]      stream      Handle of the stream, as returned from a
 *                              successful call to STRM_create().
 *  @param[in]      ppbuf       Pointer to a pointer to a buffer.
 *  @param[in,out]  pbufsize    Pointer to actual size (MAUs) of buffer
 *                              reclaimed.  If pbufsize is specified
 *                              as @c NULL, the buffer size will not
 *                              be returned.
 *  @param[in]      parg        Pointer to a user argment.
 *
 *  @retval     0               Success, there is no data in the buffer.
 *  @retval     positive        Success, the value indicates number of valid
 *                              data MAUs in the reclaimed buffer.
 *  @retval     negative        The reclaim operation failed.  The error code
 *                              depends upon the device driver implementation,
 *                              and may include driver-specific errors.  Note
 *                              that the normal STRM error codes are multiplied
 *                              by -1 before being returned by STRM_reclaim.
 *                              For example, if a timeout occurred, the return
 *                              code will be (-1 * #STRM_ETIMEOUT).
 *
 *  @remarks    STRM_reclaim() is used to request a buffer back from a stream.
 *              It returns a pointer to the buffer, the amount of valid data in
 *              the buffer, the actual allocated size of the buffer, and a user
 *              argument (@c parg). After the STRM_reclaim() call, @c parg
 *              points to the same value that was passed in with this buffer
 *              using the STRM_issue() call.
 *
 *  @remarks    If the stream was created in #STRM_OUTPUT mode, then
 *              STRM_reclaim() will return an empty buffer, and size will be
 *              zero, since the buffer is empty. If the stream was opened in
 *              #STRM_INPUT mode, STRM_reclaim() will return a non-empty buffer,
 *              and size will be the amount of data in the buffer.  In either
 *              mode STRM_reclaim() will block until a buffer can be returned
 *              to the caller, or until the timeout expires, and it will return
 *              a positive number or zero (indicating success), or a negative
 *              number (indicating an error condition).  If successful and
 *              pbufsize is non-NULL, then the actual buffer size (in MAUs) will
 *              be returned to the user.  This is useful for zero copy streaming
 *              where buffer pointers are passed between the GPP and DSP.
 *
 *  @remarks    The timeout value for the stream is specified in the stream
 *              attribute structure (see STRM_Attrs::timeout and STRM_create()).
 *              If timeout is @c SYS_FOREVER, the task will remain suspended
 *              until a buffer is available to be returned by the stream.
 *              If timeout is 0, STRM_reclaim() returns immediately.
 *
 *  @remarks    If the timeout expires before a buffer is available to be
 *              returned, STRM_reclaim() returns (-1 * #STRM_ETIMEOUT).
 *
 *  @remarks    Failure of STRM_reclaim() indicates that no buffer was
 *              returned to the client.  Therefore, if STRM_reclaim() fails,
 *              the client should not attempt to de-reference @c ppbuf, since
 *              it is not guaranteed to contain a valid buffer pointer.
 *
 *  @remarks    STRM_reclaim() is used in conjunction with STRM_issue(). The
 *              STRM_issue() call sends a buffer to a stream, and
 *              STRM_reclaim() retrieves a buffer from a stream. In normal
 *              operation each STRM_issue() call is followed by a STRM_reclaim()
 *              call.  Short bursts of multiple STRM_issue() calls can be made
 *              without an intervening STRM_reclaim() call, but over the life
 *              of the stream STRM_issue() and STRM_reclaim() should be called
 *              the same number of times. The number of STRM_issue() calls can
 *              exceed the number of STRM_reclaim() calls by a maximum of
 *              STRM_Attrs::nbufs at any given time. The value of @c nbufs is
 *              established by the STRM_create() call.
 *
 *  @remarks    NOTE: An STRM_reclaim() call should not be made without at
 *              least one outstanding STRM_issue() call.  Calling STRM_reclaim()
 *              with no outstanding STRM_issue() calls has undefined results.
 *
 *  @remarks    A task switch will occur when calling STRM_reclaim() if
 *              STRM_Attrs::timeout was not set to 0, and there are no data
 *              buffers available to be returned.
 *
 *  @note       There must be at least one outstanding issue when STRM_reclaim()
 *              is called.
 *
 *  @note       Since STRM_reclaim() is a blocking call, it should not be
 *              called from a hardware or software interrupt context.
 *
 *  @note       All buffers should be reclaimed before closing a stream.  That
 *              is, STRM_reclaim() should be called the same number of times
 *              STRM_issue() was called.
 *
 *  @sa         STRM_create(), STRM_issue().
 */
extern LgInt STRM_reclaim(STRM_Handle stream, Ptr *ppbuf, LgUns *pbufsize,
    Arg *parg);


#ifdef __cplusplus
}
#endif /* extern "C" */


/*@}*/ /* ingroup DSPSTRM */

#endif /* STRM_ */
