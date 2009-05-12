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
 *  ======== acpy3_fastsetfinal.c ========
 */


#include <std.h>

#include <dbc.h>

#include <acpy3.h>

/*
 * ACPY3_fastSetFinal does the work of ACPY3_setFinal but 
 * assumes that there is only 1 TCC allocated to the handle.
 * It is very much optimized compared to ACPY3_setFinal.
 * For any handle you can choose to use ACPY3_setFinal, or you
 * can use ACPY3_fastSetFinal, they are mutually compatible.
 */
#define ACPY3_FASTSETFINAL_IMPLEMENTATION
#pragma CODE_SECTION(ACPY3_fastSetFinal, ".text:ACPY3_fastSetFinal")
#include <acpy3_fastsetfinal.h>


/*
 *  @(#) ti.sdo.fc.acpy3; 1, 0, 0,66; 5-10-2007 10:49:19; /db/atree/library/trees/fc-b12x/src/
 */

