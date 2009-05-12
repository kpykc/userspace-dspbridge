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
 *  ======== dma4/_dman.h ========
 *
 *  DMAN configuration parameters shared by dman and acpy2 libraries.
 */

#ifndef _DMAN_
#define _DMAN_


/* Shared by DMAN and ACPY2 */
extern Int DMAN_numChannels;
extern Int DMAN_numEnabled;
extern Int DMAN_irq;
extern LgUns DMAN_enabledMask;
extern LgUns DMAN_priMask;
extern Int DMAN_heapId;


/* Internal DMAN objects */

/*
 *  ======== DmanChannel ========
 */
typedef struct DmanChannel {
    Uns     id;          /* Logical channel number */
    Int     used;        /* # of IDMA2 channels mapped to this channel */
} DmanChannel;

/*
 *  ======== QueueIdMap ========
 */
typedef struct QueueIdMap {
    Uns     id;     /* Index that queueId (mod DMAN_numEnabled) is mapped to */
    Int     used;   /* # of default assignments made to channel */
} QueueIdMap;

/*
 *  Statically allocated array of size DMAN_numEnabled to keep track of
 *  usage of logical DMA channels enabled for DMAN.
 */
extern DmanChannel  _DMAN_channel[];

/*
 *  Default queueId to logical channel number mapping - If a queueId is not
 *  equal to a DMAN enabled channel number, we use this table to determine
 *  which channel number to use for the queueId.
 */
extern QueueIdMap   _DMAN_defaultQueueMap[];

#endif /* _DMAN_*/
/*
 *  @(#) ti.sdo.fc.dman; 1,0,0,20; 5-10-2007 10:49:27; /db/atree/library/trees/fc-b12x/src/
 */

