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
 *  ======== mgs3/_dman.h ========
 */

#ifndef _DMAN_
#define _DMAN_

#include <ti/sdo/fc/dman/dman.h>

#define DMAN_MAXPHYCHANNEL  6       /**< Maximum number of physical DMA
                                       channels. */
#define DMAN_MAXQID         8       /**< Maximum allowable DMAN queue ID. */

#define ILLEGAL_CHANNEL     -1      /**< An illegal channel. */

/*
 *  DMAN_Obj type definitions.
 */
typedef struct DMAN_Obj {
    Void * selfPtr;             /**< Self pointer. */
    Int queueId[DMAN_MAXQID];   /**< Specifies how logical channels serializer
                                   id is mapped to physical channels. */
    Fxn idmaFxn;                /**< IDMA2 interface of a XDAIS algorithm. */
    Int mask;                   /**< Mask field used in driver
                                   synchronization. */
    Int reserved;               /**< Reserved field. */
} DMAN_Obj;

#endif /*_DMAN_*/

/*
 *  @(#) ti.sdo.fc.dman; 1,0,0,20; 5-10-2007 10:49:28; /db/atree/library/trees/fc-b12x/src/
 */

