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
 *  ======== dman3_init.c ========
 *  Initialize the DMAN module
 */

#pragma CODE_SECTION(DMAN3_init, ".text:DMAN3_init");

#include <std.h>
#include <dman3.h>
#include <dman3_impl.h>

/*
 *  ======== DMAN3_init ========
 *  Initialize the DMAN module
 */
Void DMAN3_init(Void)
{
    _DMAN3_initImpl();
}

/*
 *  @(#) ti.sdo.fc.dman3; 1, 0, 1,66; 5-10-2007 10:49:43; /db/atree/library/trees/fc-b12x/src/
 */

