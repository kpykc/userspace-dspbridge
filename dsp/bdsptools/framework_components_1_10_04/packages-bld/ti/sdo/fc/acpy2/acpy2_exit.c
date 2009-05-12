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
 *  ======== acpy2_exit.c ========
 *
 */

#include <std.h>
#include <ti/xdais/acpy2.h>
#include "dma.h"

#pragma CODE_SECTION(ACPY2_exit, ".text:ACPY2_exit")

extern Int refCount;

Void ACPY2_exit(Void)
{
    refCount--;
    if (refCount == 0) {
        /* call DMA_exit that will disable the interrupts and free the q's */
        DMA_exit();
    }
}
/*
 *  @(#) ti.sdo.fc.acpy2; 1,0,0,20; 5-10-2007 10:49:05; /db/atree/library/trees/fc-b12x/src/
 */

