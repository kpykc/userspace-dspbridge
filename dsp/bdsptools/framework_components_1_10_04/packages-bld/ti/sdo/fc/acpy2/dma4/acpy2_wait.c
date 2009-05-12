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
 *  ======== dma4/acpy2_wait.c ========
 */

#include <std.h>

#ifndef _RTS_MODE
#include <tsk.h>
#include <dbc.h>
#else
#include <dbc_rts.h>
#endif
#include <ti/xdais/acpy2.h>
#include "_dma.h"

#if !defined(DMA_OPT) 
#define DMA_OPT 0         // if not defined
#endif

#if !DMA_OPT & 0

#pragma CODE_SECTION(ACPY2_wait, ".text:ACPY2_wait")

/*
 *  ======== ACPY2_wait ========
 *  Wait for all data transfer on logical channel to complete. This
 *  implementation does a TSK_yield while waiting for all outstanding
 *  jobs to complete. Other implementations might do other processing while
 *  waiting.
 */

Void ACPY2_wait(IDMA2_Handle handle)
{
    _DMA_Handle dmaHandle = (_DMA_Handle)handle;

    while (dmaHandle->numJobs > 0) {
# ifndef _RTS_MODE
        //TSK_yield();
# endif
    }
}
#endif // if !DMA_OPT
/*
 *  @(#) ti.sdo.fc.acpy2; 1,0,0,20; 5-10-2007 10:49:06; /db/atree/library/trees/fc-b12x/src/
 */

