/* 
 *  Copyright 2007
 *  Texas Instruments Incorporated
 *
 *  All rights reserved.  Property of Texas Instruments Incorporated
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 * 
 */
/*
 *  ======== dma.h ========
 *  DMA module interface declarations.
 *
 */

#ifndef DMA_
#define DMA_

/*
 * opaque handle to a DMA job queue
 */
typedef struct _DMA_QueueObj *_DMA_Queue;

/*
 *  ======== DMA_exit ========
 *  Module finalization method.
 *  All the queues created in the DMA_init is freed up
 *  Interrupts are disabled and unplugged.
 *  Parameters:
 *  Returns:
 *  : Nothing. 
 */
extern Void DMA_exit(Void);

/*
 *  ======== DMA_init ========
 *  Module initialization method.
 *  Job queues for each channel is created. 
 *  Also, the interrupts are plugged and enabled.
 *
 *  Parameters:
 *  Returns:
 *     TRUE: Success.
 *     FALSE: Failure
 */
extern Bool DMA_init(Void);

/*
 *  ======== DMA_load ========
 *  Directly load the registers and start the DMA 
 *  Parameters:
 *    handle: IDMA2 Handle
 *    src:i   source address
 *    dst: i  dst address
 *    cnt:    number of elements
 *  Returns:  : Nothing
 *  Requires  : valid handle 
 *  Pre-conditions: Interrupts disabled.
 *  Note:
 *      This will also update the jobQueue's current handle so that 
 *  in the ISR, we process that.
 */
extern Void DMA_load(IDMA2_Handle handle, IDMA2_AdrPtr src, 
                     IDMA2_AdrPtr dst, Uns cnt);

/*
 *  ======== DMA_put ========
 *  The DMA job is put in the queue.
 *
 *  Parameters:
 *    handle    : IDMA2 Handle
 *    src       : source address
 *    dst       : dst address
 *    cnt       : number of elements
 *  Returns:    : Nothing
 *  Requires    : valid handle 
 *  Pre-conditions: Interrupts disabled and jobQueue is not full.
 */
extern Void DMA_put(IDMA2_Handle handle, IDMA2_AdrPtr src,
                    IDMA2_AdrPtr dst, Uns cnt);

/*
 *  ======== DMA_status ========
 *  Return the count of outstanding requests for this queue.
 *
 *  Parameters: queue
 *  Returns:     : The count of outstanding requests for this queue.
 *  Requires     : valid handle 
 *  Pre-conditions: Interrupts disabled.
 */
extern Int DMA_status(_DMA_Queue queue);

/*
 *  ======== DMA_queueEmpty ========
 *  Checks atomically if the queue is empty from ACPY_start. 
 *  Parameters: queue
 *  Returns:     : TRUE if queue is Empty
 *               : FALSE if queue is not Empty.
 *  Requires: queue != NULL
 *  Pre-conditions: Interrupts disabled.
 *
 */
extern Bool DMA_queueEmpty(_DMA_Queue queue);

/*
 *  ======== DMA_queueFull ========
 *  Checks atomically if the queue is full. Called from ACPY_start. 
 *  Parameters: queue
 *  Returns:      : TRUE if queue is full
 *                : FALSE if queue is not full.
 *  Requires: queue != NULL
 *
 */
extern Bool DMA_queueFull(_DMA_Queue queue);

#endif /* DMA_ */
/*
 *  @(#) ti.sdo.fc.acpy2; 1,0,0,20; 5-10-2007 10:49:05; /db/atree/library/trees/fc-b12x/src/
 */

