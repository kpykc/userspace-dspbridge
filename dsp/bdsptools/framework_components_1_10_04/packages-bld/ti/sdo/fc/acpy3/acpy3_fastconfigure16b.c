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
 * ======== acpy3_fastconfigure16b.c ======== 
 * Configure a logical channel efficiently
 */
 
#include <std.h>

#include <dbc.h>

#include <acpy3.h>

#define ACPY3_FASTCONFIGURE16B_IMPLEMENTATION
#pragma CODE_SECTION(ACPY3_fastConfigure16b, ".text:ACPY3_fastConfigure16b")
#include <acpy3_fastconfigure16b.h>

/*
 *  @(#) ti.sdo.fc.acpy3; 1, 0, 0,66; 5-10-2007 10:49:19; /db/atree/library/trees/fc-b12x/src/
 */

