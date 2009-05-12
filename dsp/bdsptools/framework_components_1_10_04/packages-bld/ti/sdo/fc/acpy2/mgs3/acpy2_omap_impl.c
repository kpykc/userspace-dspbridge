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
 *  ======== mgs3/acpy2_omap_impl ========
 *
 *  Implementaion of Platform specific functions.
 *
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

#pragma CODE_SECTION(ACPY2_OMAP_setDstPortEndianism, \
              ".text:ACPY2_OMAP_setDstPortEndianism")
#pragma CODE_SECTION(ACPY2_OMAP_setSrcPortEndianism, \
             ".text:ACPY2_OMAP_setSrcPortEndianism")
#pragma CODE_SECTION(ACPY2_OMAP_getDstPortEndianism, \
             ".text:ACPY2_OMAP_getDstPortEndianism")
#pragma CODE_SECTION(ACPY2_OMAP_getSrcPortEndianism, \
              ".text:ACPY2_OMAP_getSrcPortEndianism")

#pragma CODE_SECTION(ACPY2_OMAP_enablePackedBurst, \
              ".text:ACPY2_OMAP_enablePackedBurst")
#pragma CODE_SECTION(ACPY2_OMAP_disablePackedBurst, \
              ".text:ACPY2_OMAP_disablePackedBurst")

/*
 *  ======== ACPY2_OMAP_setSrcPortEndianism ========
 *  Set the source port endianism.
 */
Void ACPY2_OMAP_setSrcPortEndianism(IDMA2_Handle handle, 
                                    ACPY2_OMAP_EndianismType endianness)
{
    /* Assert if handle is null */
     DBC_require(handle !=NULL);

     if (endianness == ACPY2_OMAP_LITTLEENDIAN) {
         handle->ccr2 |=  CCR2_LE_SRC;
     } else {
         handle->ccr2 &= ~CCR2_LE_SRC;
     }
}

/*
 *  ======== ACPY2_OMAP_setDstPortEndianism ========
 *  Set the destination port endianism.
 */
Void ACPY2_OMAP_setDstPortEndianism(IDMA2_Handle handle, 
                                    ACPY2_OMAP_EndianismType endianness)
{
    /* Assert if handle is null */
     DBC_require(handle !=NULL);

     if (endianness == ACPY2_OMAP_LITTLEENDIAN) {
         handle->ccr2 |=  CCR2_LE_DST;
     } else {
         handle->ccr2 &= ~CCR2_LE_DST;
     }
}

/*
 *  ======== ACPY2_OMAP_getDstPortEndianism ========
 *  Get the destination port endianism setting.
 */
ACPY2_OMAP_EndianismType ACPY2_OMAP_getDstPortEndianism(IDMA2_Handle handle)
{
    /* Assert if handle is null */
     DBC_require(handle !=NULL);

     return ((handle->ccr2 & CCR2_LE_DST)? ACPY2_OMAP_LITTLEENDIAN :
                                           ACPY2_OMAP_BIGENDIAN);
 }

/*
 *  ======== ACPY2_OMAP_getSrcPortEndianism ========
 *  Get the source port endianism setting.
 */
ACPY2_OMAP_EndianismType ACPY2_OMAP_getSrcPortEndianism(IDMA2_Handle handle)
{
    /* Assert if handle is null */
     DBC_require(handle !=NULL);

     return ((handle->ccr2 & CCR2_LE_SRC)? ACPY2_OMAP_LITTLEENDIAN :
                                           ACPY2_OMAP_BIGENDIAN);
}
/*
 *  ======== ACPY2_OMAP_enablePackedBurst  ========
 *  Enable Packed & Burst mode for the given Logical Channel. 
 */
Void ACPY2_OMAP_enablePackedBurst(IDMA2_Handle handle) 
{
    /* Assert if handle is null */
     DBC_require(handle !=NULL);

     handle->csdp |= CSDP_SRCDSTPKBEN_ENA;
 
}

/*
 *  ======== ACPY2_OMAP_enablePackedBurst  ========
 *  Disable Packed & Burst mode for the given Logical Channel. 
 */
Void ACPY2_OMAP_disablePackedBurst(IDMA2_Handle handle)
{
    /* Assert if handle is null */
     DBC_require(handle !=NULL);

     handle->csdp &= ~CSDP_SRCDSTPKBEN_ENA;
 
}

/*
 *  ======== ACPY2_OMAP_getPhysicalChannel   ========
 *  Return the physical channel assigned to the Logical DMA Channel represented by 
 *  the handle.
 */
Uns ACPY2_OMAP_getPhysicalChannel(IDMA2_Handle handle)
{
    /* Assert if handle is null */
     DBC_require(handle !=NULL);

     return handle->channel;
}

/*
 *  @(#) ti.sdo.fc.acpy2; 1,0,0,20; 5-10-2007 10:49:06; /db/atree/library/trees/fc-b12x/src/
 */

