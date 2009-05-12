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
 *  ======== acpy2_omap_instrumentation.h ========
 *  DMA bandwidth instrumentation data structures and APIs.
 *
 */

#include <std.h>

#ifndef _INSTR_BANDWIDTH
# define _INSTR_BANDWIDTH 0
#endif

#if _INSTR_BANDWIDTH

typedef struct _ChanStat {
        LgUns bytesTransfered;
        LgUns bytesPending;
        LgUns numTransfersStarted;
        LgUns numTransfersFinished;
} ACPY2_OMAP_ChanStat;


Void ACPY2_OMAP_resetChannelStats(Uns chanId);

Void ACPY2_OMAP_getChannelStats(Uns chanId, ACPY2_OMAP_ChanStat * chanStat);

#endif // if _INSTR_BANDWIDTH

/*
 *  @(#) ti.sdo.fc.acpy2; 1,0,0,20; 5-10-2007 10:49:05; /db/atree/library/trees/fc-b12x/src/
 */

