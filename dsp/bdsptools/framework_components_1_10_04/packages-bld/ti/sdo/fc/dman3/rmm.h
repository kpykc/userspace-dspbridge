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
 *  ======== rmm.h ========
 *
 *  This "remote" memory manager provides general heap management of an
 *  arbitrary memory segment.
 *
 *  Public functions:
 *      RMM_alloc
 *      RMM_create
 *      RMM_delete
 *      RMM_exit
 *      RMM_free
 *      RMM_init
 *
 */

#ifndef RMM_
#define RMM_

/*
 *  ======== RMM_Attrs ========
 *  Attributes passed to RMM_create().
 */
typedef struct RMM_Attrs {
    Int         segid;                  /* Heap ID to use for internal allocs */
    Uns         maxFreeListBlocks;  /* Maximum number of free blocks */
    LgUns       base;               /* Base address of memory to be managed */
    LgUns       length;             /* Size (MAUs) of memory to be managed */
} RMM_Attrs;

/*
 *  ======== RMM_Segment ========
 *  Memory segment on the DSP available for remote allocations.
 */
typedef struct RMM_Segment {
    LgUns   base;       /* Base of the segment */
    LgUns   length;     /* Size of the segment (target MAUs) */
} RMM_Segment;

/*
 *  ======== RMM_Stat ========
 *  Status structure for RMM_Segment, used for debugging.
 */
typedef struct RMM_Stat {
    LgUns   size;       /* original size of segment */
    LgUns   used;       /* number of bytes used in segment */
    LgUns   length;     /* length of largest contiguous block (target MAUs) */
    Uns     nBlocks;    /* Total number of free blocks */
} RMM_Stat;

/*
 *  ======== RMM_Handle ========
 */
typedef struct RMM_Obj *RMM_Handle;


/*
 *  ======== RMM_alloc ========
 *
 *  RMM_alloc is used to remotely allocate or reserve memory on the DSP.
 *
 *  Parameters:
 *      rmmHandle       - Handle returned from RMM_create().
 *      size            - Size (target MAUS) to allocate.
 *      pAddr           - Location to store allocated address.
 *  Returns:
 *      TRUE:                   Success.
 *      FALSE:                  Memory allocation failed.
 *
 *  Requires:
 *      Valid rmmHandle.
 *      dspAddr != NULL.
 *      size > 0
 *      reserve || rmmHandle->numSegs > 0.
 *  Ensures:
 */
extern Bool RMM_alloc(RMM_Handle rmmHandle, LgUns size, LgUns *pAddr);

/*
 *  ======== RMM_check ========
 *
 *  RMM_check checks (via asserts) the "sanity" of the free list for
 *  all valid memory segments.
 */
extern Void RMM_check(RMM_Handle rmmHandle, RMM_Stat *stat);

/*
 *  ======== RMM_create ========
 *  Create an RMM object for reserving memory.
 *
 *  Parameters:
 *      seg:         - memory segment base, length.
 *      attrs:       - pointer to RMM_Attrs.
 *  Returns:
 *      attrs != NULL
 *      attrs->maxFreeListBlocks > 0
 *  Requires:
 *      attrs != NULL
 *      attrs->maxFreeListBlocks > 0;
 *      attrs->length > 0;
 *  Ensures:
 */
extern RMM_Handle RMM_create(RMM_Attrs *attrs);

/*
 *  ======== RMM_delete ========
 *  Delete rmmHandle allocated in RMM_create().
 *
 *  Parameters:
 *      rmmHandle          - Handle returned from RMM_create().
 *  Returns:
 *  Requires:
 *      RMM initialized.
 *      rmmHandle != NULL.
 *  Ensures:
 */
extern Void RMM_delete(RMM_Handle rmmHandle);

/*
 *  ======== RMM_exit ========
 *  Exit the RMM module
 *
 *  Parameters:
 *  Returns:
 *  Requires:
 *  Ensures:
 */
extern Void RMM_exit(Void);

/*
 *  ======== RMM_free ========
 *  Free or unreserve memory allocated through RMM_alloc().
 *
 *  Parameters:
 *      rmmHandle:      - Handle returned from RMM_create().
 *      addr:           - Address to free.
 *      size:           - Size of memory to free.
 *  Returns:
 *  Requires:
 *      rmmHandle != NULL.
 *      size > 0
 *  Ensures:
 */
extern Bool RMM_free(RMM_Handle rmmHandle, LgUns dspAddr, LgUns size);

/*
 *  ======== RMM_init ========
 *  Initialize the RMM module
 *
 *  Parameters:
 *  Returns:
 *  Requires:
 *  Ensures:
 */
extern Void RMM_init(Void);


#endif /* RMM_ */

/*
 *  @(#) ti.sdo.fc.dman3; 1, 0, 1,66; 5-10-2007 10:49:43; /db/atree/library/trees/fc-b12x/src/
 */

