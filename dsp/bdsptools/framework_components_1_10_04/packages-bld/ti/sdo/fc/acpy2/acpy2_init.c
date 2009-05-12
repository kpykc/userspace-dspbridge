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
 *  ======== acpy2_init.c ========
 *
 */

#include <std.h>
#ifndef _RTS_MODE
#include <sys.h>
#else
#include <dbc_rts.h>
#endif
#include <ti/xdais/acpy2.h>
#include "dma.h"

#pragma CODE_SECTION(ACPY2_init, ".text:ACPY2_init")
Int refCount  = 0;

/*
 *  ======== ACPY2_init ========
 *  Initialize the ACPY module
 */
Void ACPY2_init(Void)
{
    /* Use CSL to open DMAN_numberOfDMANChannels */
#ifdef DMAN_USES_CSL

#else
     if (refCount == 0) {
        if (!DMA_init()) {
            SYS_abort("ACPY_Init failed. Aborting \n");
        }
     }

     /* count this reference even if no allocation is done */
     /* so that memory free is done only when last node requires ACPY2 */
    refCount++;
#endif
}
/*
 *  @(#) ti.sdo.fc.acpy2; 1,0,0,20; 5-10-2007 10:49:05; /db/atree/library/trees/fc-b12x/src/
 */

