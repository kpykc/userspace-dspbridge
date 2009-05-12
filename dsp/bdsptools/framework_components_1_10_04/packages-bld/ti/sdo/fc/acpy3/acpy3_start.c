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
 *  ======== acpy3_start.c ========
 *  C model of the acpy3_start() implementation using Joule QDMA.
 */

#include <std.h>

#include <dbc.h>

#include <ti/xdais/idma3.h>
#include <acpy3.h>

#ifndef _ACPY3_CPUCOPY_


#define ACPY3_START_IMPLEMENTATION
#pragma CODE_SECTION(ACPY3_start, ".text:ACPY3_start") 
#include <acpy3_start.h>

#endif   /* ifndef _ACPY3_CPUCOPY */

/*
 *  @(#) ti.sdo.fc.acpy3; 1, 0, 0,66; 5-10-2007 10:49:19; /db/atree/library/trees/fc-b12x/src/
 */

