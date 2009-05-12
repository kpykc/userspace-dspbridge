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
 *  ======== acpy3_init.c ========
 */

#include <std.h>
#include <dbc.h>
#include <acpy3.h>

#pragma CODE_SECTION(ACPY3_init, ".text:ACPY3_init")
#pragma CODE_SECTION(ACPY3_exit, ".text:ACPY3_exit")


static far Int _ACPY3_refCount  = 0;


/*
 *  ======== ACPY3_exit ========
 *  Finalize the ACPY3 module
 */
Void ACPY3_exit(Void)
{
    _ACPY3_refCount--;

    if (_ACPY3_refCount == 0) {
        
    }
}

/*
 *  ======== ACPY3_init ========
 *  Initialize the ACPY3 module
 */
void ACPY3_init(Void)
{
    if (_ACPY3_refCount == 0) {

     }

    _ACPY3_refCount++;
}

/*
 *  @(#) ti.sdo.fc.acpy3; 1, 0, 0,66; 5-10-2007 10:49:19; /db/atree/library/trees/fc-b12x/src/
 */

