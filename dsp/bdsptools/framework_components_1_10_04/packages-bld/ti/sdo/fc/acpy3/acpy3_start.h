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
 *  ======== acpy3_start.h ========
 */

#ifdef _ACPY3_

#ifndef _ACPY3_START_
#define _ACPY3_START_

#include <ti/sdo/fc/acpy3/acpy3_qdma.h>
#include <ti/sdo/fc/acpy3/acpy3_util.h>

#if defined(ACPY3_START_INLINE) || defined(ACPY3_INLINE_ALL)
ACPY3_VERSION_MACRO
static __inline 
#endif
#if defined(ACPY3_START_INLINE) || defined(ACPY3_INLINE_ALL) \
 || defined(ACPY3_START_IMPLEMENTATION)

/*
 *  ======== ACPY3_start ========
 */
void ACPY3_start(IDMA3_Handle restrict handle)
{
    volatile Uns           *lastPaRam;
    Int                     numTransfers;
    Int                     numTransfersM1;
    Int                     numFullTransfers;
    Int                     numPartialTransfers;
    Uns                     csr;
    Uns                     idmaSrcPtr;
    Uns                     idmaDstPtr;
    Int                     addrIncr;
    MdUns                   qdmaChan;
    Uns                     qDmaChanMask;
    volatile Uns           *idmaRegPtr;
    Uns          * restrict paRamAddr;
    IDMA3_Handle * restrict lockTblPtr;
    Uns                     partialAddressesWrittenFlag;
#if _ACPY3_DIRTY_BITS
    SmUns        * restrict dirtyBitPtr;
    Uns                     dirtyMaskFull;
    Int                     i;
#endif
    Uns                     dirtyMaskPartial;

#if defined(ACPY3_START_IMPLEMENTATION)
    DBC_require(handle != NULL);
    DBC_require(handle->transferPending == FALSE);
#endif    

    /*
     * Disable interrupts when modifying IDMA registers.
     * This is necessary since these registers are shared resources.
     */
    csr = _disable_interrupts();
    
    /*
     * Do as much work as possible before waiting for any 
     * shared hardware registers.
     */
    numTransfers = handle->numPaRams;
    numTransfersM1 = numTransfers - 1;
    
    idmaDstPtr = (Uns)ACPY3_getPaRamPhysicalAddress(handle, numTransfersM1);
    idmaRegPtr = (volatile Uns *)IDMA0_STATUS;
    
    /*
     *  Set Dest to Physical PARAM Addr of the same tranfer .
     */
    idmaRegPtr[3] = idmaDstPtr;  /* IDMA0_DEST */
    /*asm(" ; barrier");*/  /* add barrier to force above IDMA write ASAP */
    
#if _ACPY3_DIRTY_BITS
    /* 
     *  Mark the last bit of the first transfer in the array of dirty bytes
     *  as "dirty".  This is the "trigger word" for QDMA to start a transfer,
     *  so it always has to be written, even if its value does not change.
     *  Get a pointer to the end of the array of dirty PaRAM cache bytes
     */
    dirtyBitPtr = ((ACPY3_MoreEnv *)handle->env)->dirtyBitTable;
    *dirtyBitPtr |= (SmUns)(1 << 5);
    dirtyBitPtr += numTransfers;
#endif /* if _ACPY3_DIRTY_BITS */
    
    /* 
     *  Start copying the cached PARAM blocks from their "env" Cache location 
     *  to actual physical PARAM registers.
     *
     *  For linked transfers (i.e. numTransfers > 1) do this copying in
     *  reverse order since the trigger word is associated with the 1st
     *  transfer, all linked PARAM entries must be written to first.
     */

    
    /* USE INTERNAL DMA (IDMA) to copy PARAM cache to Physical PARAM */
      
    /* 
     *  "numFullTransfers" is the number of 4 transfer-blocks that 
     *  can be transfered in a single Internal DMA (IDMA) transfer. 
     *  NOTE: IDMA transfers are 4x8 unsigned int blocks. 
     */
    numFullTransfers = numTransfers >> 2;

#if _ACPY3_DIRTY_BITS
    if (numFullTransfers) {
    
        /* Generate a mask of dirty bits for this transfer */
        dirtyMaskFull = 0;
#pragma MUST_ITERATE(1,8)        
        for (i = numFullTransfers; i != 0; i--) {
            dirtyMaskFull |= _mem4(dirtyBitPtr -= 4);
            _mem4(dirtyBitPtr) = 0;
        }
        dirtyMaskFull = _packlh2(dirtyMaskFull, dirtyMaskFull);
        dirtyMaskFull = _swap4(dirtyMaskFull);
        /* Transfer all necessary words */
        idmaRegPtr[1] = ~dirtyMaskFull;   /* IDMA0_MASK */
    }
#endif /* if _ACPY3_DIRTY_BITS */

    idmaSrcPtr = (Uns)ACPY3_getPaRamCache(handle, numTransfersM1);
    lockTblPtr = (IDMA3_Handle *)_ACPY3_qchLockTbl;
    partialAddressesWrittenFlag = TRUE;
                                   
    /* Remaining transfers are handled via a separate IDMA transfer */
    numPartialTransfers = numTransfers & (4-1);
    qdmaChan = handle->qdmaChan;
    
#if _ACPY3_DIRTY_BITS
    if (numPartialTransfers) {
      dirtyMaskPartial = _mem4(dirtyBitPtr - 4);
      dirtyBitPtr -= numPartialTransfers;
      dirtyMaskPartial = _packlh2(dirtyMaskPartial, dirtyMaskPartial);
      dirtyMaskPartial = _swap4(dirtyMaskPartial);
      dirtyMaskPartial &= 0x00ffffffU >> (8 * (3 - numPartialTransfers));
    }
#else    
    dirtyMaskPartial = ((Uns)(0xffffff << (numPartialTransfers << 3))) |
                       0xc0c0c0;
#endif /* if _ACPY3_DIRTY_BITS */
    
    paRamAddr = handle->paRamAddr;
     
    /* 
     *  Program the QDMA Channel Map register for this handle's qdmaChan
     *  so that DMA transfer is triggerred by the last Physical PARAM write
     *  issued by this 'start'.
     *
     *  NOTE: System must ensure that when the last PARAM write occurs this
     *  QCHMAP setting is still intact. Otherwise the rest of this function
     *  must be executed as a critical section.
     *    Bits 13...5: paRam address
     *    Bits 4..2:   triger word (in this case, word 7)
     */
    /* TODO: need to understand the use of all bits in QCHMAP */

    /*
     *  Set Source to the first transfer's PARAM Cache. 
     *  Convert global address to local address by masking off bit 28.
     */
    idmaRegPtr[2] = idmaSrcPtr;  /* IDMA0_SOURCE */
    
    if (lockTblPtr[qdmaChan])
    {
        qDmaChanMask = 1 << qdmaChan;
        
        /*
         *  Perform an IDMA_wait to make sure that all transfers from any  
         *  preceding transfer starts have completed.
         *  The IDMA transfer registers can be rewritten during an existing
         *  IDMA transfer.  But it is only 2 levels deep.  
         *  This isn't strictly necessary because if the IDMA gets in a 
         *  position where a user tries to trigger another IDMA transfer
         *  and there is already one active and another pending, the system
         *  is supposed to temporarily halt until the currently pending
         *  transfer becomes active.  But for safety's sake we'll check anyway.
         *  See "Programming Model" Section of "GEM INTERNAL DMA" spec.
         */
        while ((*idmaRegPtr) & 0x3);

        /* 
         *  Program the QDMA Channel Map register for this handle's qdmaChan
         *  so that DMA transfer is triggerred by the last Physical PARAM write
         *  issued by this 'start'.
         */
        /*
         *  NOTE: System must ensure that when the last PARAM write occurs 
         *  this QCHMAP setting is still intact. Otherwise the rest of 
         *  this function  must be executed as a critical section.
         *    Bits 13...5: paRam address
         *    Bits 4..2:   trigger word (in this case, word 7)
         */

        /*
         * Ensure that we can use the corresponding QDMA channel.
         * a) wait for the PaRAM location for STATIC = 1, and
         * b) wait for QER to be cleared.
         * Step (a) is necessary to detect that the final link in the QDNA
         * linked list has been copied to the QDMA PaRAM entry location.
         * Step (b) confirms that the newly linked PaRAM entry has been 
         * submitted to TC. 
         */
        /* step (a) */
        lastPaRam = (Uns *)(ACPY3_QDMA_PARAM_BASE + 
                           (ACPY3_QCHMAP[qdmaChan] & 0x3fe0));
        while (!((*lastPaRam) & 0x8));

        /* step (b) */
        while ((*ACPY3_QER) & qDmaChanMask);
    }

    /* Mark that the QDMA channel is locked */
    lockTblPtr[qdmaChan] = handle;

    /*
     *  As soon as channel is clear, mark the channel state to indicate a 
     *  new transfer is being issued and 'transfer is pending'.
     *  Only a subsequent 'ACPY3_wait' will clear the pending state.
     */
    handle->transferPending = TRUE;

    /* Mark the trigger word for the transfer */
    /* trigger word is link|bCntrld */
    ACPY3_QCHMAP[qdmaChan] = ((Uns)(paRamAddr)) | (0x5 << 2); 

    if (numFullTransfers) {
#if _ACPY3_DIRTY_BITS
        if (dirtyMaskFull) {
#else
            /* Transfer all words */
            idmaRegPtr[1] = 0xc0c0c0c0;  /* IDMA0_MASK */
#endif  /* if _ACPY3_DIRTY_BITS ; else */
            /*
             *  Set Source to the first transfer's PARAM Cache. 
             *  Convert global address to local address by masking off bit 28.
             *  Set Dest to Physical PARAM Addr of the same tranfer .
             */
             /* Writing to SRC and DEST was already done at the top */
             
            /* IDMA0_COUNT (this triggers IDMA transfer) */
            idmaRegPtr[4] = (numFullTransfers - 1);  
#if _ACPY3_DIRTY_BITS
        }
#endif
        partialAddressesWrittenFlag = FALSE;

#if (ACPY3_PARAMENTRYSIZE == 8)
        addrIncr = numFullTransfers << (2 + 3 + 2);
#else        
        addrIncr = numFullTransfers * (4 * ACPY3_PARAMENTRYSIZE * 4);
#endif        
        idmaSrcPtr += addrIncr; /* Note: computed as byte addressing */
        idmaDstPtr += addrIncr; /* Note: computed as byte addressing */
    }
               
    if (numPartialTransfers) {

        /*
         *  NOTE: If we did the same optimization as above by issuing 1-2 
         *  transfers using CPU writes and 3 transfers via IDMA then this
         *  would potentially create a race condition if the full transfers
         *  submitted via separate IDMA does not complete by the time the 
         *  CPU writes complete.
         *
         *  Therefore once numFullTransfers>1 we only use IDMA writes, 
         *  since IDMA  ensures FIFO ordering of the submitted writes.
         *
         *  Since the IDMA hardware supports to a 2-level FIFO of transfer
         *  commands, we do not need to check to see if the transfer of
         *  numFullTransfers is complete before setting up the transfer
         *  of numPartialTransfers.
         */

        /* Set right transfer mask for the number of remaining Xfers */
        /* Set Source to first transfer's PARAM Cache (local address) */
        /* Set Destination to Physical PARAM Addr of the same tranfer */
        /* Set count to 0 (i.e. one group of 32 words) */
#if _ACPY3_DIRTY_BITS
        idmaRegPtr[1] = ~dirtyMaskPartial;   /* IDMA0_MASK */
#else        
        idmaRegPtr[1] = dirtyMaskPartial;    /* IDMA0_MASK */
#endif /* if _ACPY3_DIRTY_BITS -- else */

        if (!partialAddressesWrittenFlag) {
          /* IDMA0_SOURCE and IDMA0_DEST */
          ((volatile long long *)(idmaRegPtr))[1] = 
                                 _itoll(idmaDstPtr, idmaSrcPtr);
        }
        idmaRegPtr[4] = 0x0;                 /* IDMA0_COUNT */
    } 
    
    _restore_interrupts(csr); /* reenable interrupts */

#if _ACPY3_DIRTY_BITS
    /*
     *  Mark the dirty bit table as clean for the next call to 
     *  ACPY3_start(). Do it now while the IDMA is concurrently working.
     *  For a linked transfer, the first physical param will be overwritten,
     *  So it needs to be marked as dirty. If it isn't linked, then no problem.
     */
    *dirtyBitPtr++ = (numTransfersM1 != 0) ? 0x3f : 0;
    if (numPartialTransfers >= 2)
        *dirtyBitPtr++ = 0;
    if (numPartialTransfers == 3)
        *dirtyBitPtr++ = 0;
#endif /* if _ACPY3_DIRTY_BITS */

#if defined(ACPY3_START_IMPLEMENTATION)
    DBC_ensure(handle->transferPending == TRUE);
#endif    

    return;
}
#endif /* if defined(ACPY3_START_INLINE) || 
       **    defined(ACPY3_INLINE_ALL) || 
       **    defined(ACPY3_START_IMPLEMENTATION) 
       */
#endif /* _ACPY3_START_ */
#else
#error "ti/sdo/fc/acpy3/acpy3.h must be included first"
#endif 

/*
 *  @(#) ti.sdo.fc.acpy3; 1, 0, 0,66; 5-10-2007 10:49:19; /db/atree/library/trees/fc-b12x/src/
 */

