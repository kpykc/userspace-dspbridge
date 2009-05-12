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
 *  ======== dskt2_qos.h ========
 *  DSKT2 QOS support library definitions.
 *
 */

#ifndef DSKT2_QOS_H
#define DSKT2_QOS_H

typedef enum DSKT2_QOS_TI_MsgArg1Types {
    DSKT2_QOS_SHARED_SCRATCH_ALLHEAPS = 0x200,   
    DSKT2_QOS_SHARED_SCRATCH_DARAM = 0x201,
    DSKT2_QOS_SHARED_SCRATCH_SARAM = 0x202
} DSKT2_QOS_TI_MsgArg1Types;

typedef enum DSKT2_QOS_TI_GetSharedScratchMsgArg2   {
    /* 
     * If Arg2 is NOT ALL_SCRATCHGROUPS then  the assigned value
     * is Scratch Group/Mutex Id 
     */
    DSKT2_QOS_ALL_SCRATCHGROUPS = 0x100          
} DSKT2_QOS_TI_GetSharedScratchMsgArg2;

extern Void  DSKT2_QOS_getStat(Uns heapType, Uns mutexId, 
        LgUns *sizeSharedScratch, Uns *inHeap);

#endif

/*
 *  @(#) ti.sdo.fc.dskt2; 1, 0, 1,66; 5-10-2007 10:50:09; /db/atree/library/trees/fc-b12x/src/
 */

