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
 *  ======== mgs3/acpy2_omap_cb ========
 *  Implementation of ACPY2_OMAP_setCallbackNotification function.
 */

#include <std.h>
#ifndef _RTS_MODE
#include <dbc.h>
#else
#include <dbc_rts.h>
#endif
#include <ti/xdais/acpy2.h>
#include <acpy2_omap.h>
#include "_dma.h"
#include "_dmaOmaphw.h"

#pragma CODE_SECTION(ACPY2_OMAP_setCallbackNotification,  \
              ".text:ACPY2_OMAP_setCallbackNotification")

/*
 *  ======== ACPY2_OMAP_setCallbackNotification  ========
 *  Sets the callback function and argument for the function
 *  that (when not NULL) gets called by the DMA ISR when at the moment 
 *  when physical DMA transfer completes on this logical channel
 */
Void ACPY2_OMAP_setCallbackNotification(IDMA2_Handle handle,
                     ACPY2_OMAP_CallbackFxn cbFxn, Arg cbArg)
{
    /* Assert if handle is null */
     DBC_require(handle !=NULL);

     handle->cbFxn = cbFxn;
     handle->cbArg = cbArg;
}
/*
 *  @(#) ti.sdo.fc.acpy2; 1,0,0,20; 5-10-2007 10:49:06; /db/atree/library/trees/fc-b12x/src/
 */

