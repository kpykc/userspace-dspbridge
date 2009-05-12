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
 *  ======== dman3_impl.h ========
 *
 *  DMAN3 internal implementation declarations.
 */

#ifndef _DMAN3_IMPL
#define _DMAN3_IMPL

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#include <rmm.h>


/* Maximum number of TCCs available */
#define _DMAN3_MAXNUMTCCS    64


typedef struct DMAN3_ContextObj *DMAN3_Context;


/*
 *  ======== _DMAN_initParams ========
 *  Initialize DMAN3_PARAMS with satic configuration parameters
 */
extern Void _DMAN3_initParams(Void);
    
/*
 *  ======== _DMAN_initImpl   ========
 *  Implementation of DMAN3_init function which expects DMAN3_PARAMS
 *  to be initialized independent of RTSC or C configuration
 */
extern Void _DMAN3_initImpl(Void);

extern far SmUns         _DMAN3_tccAllocationTable[_DMAN3_MAXNUMTCCS];
extern far Int           _DMAN3_numAssignedTccs;

extern far SmUns         _DMAN3_numTccGroup[DMAN3_MAXGROUPS];
extern far MdUns         _DMAN3_numPaRamGroup[DMAN3_MAXGROUPS];
extern far DMAN3_Context _DMAN3_groupContext[DMAN3_MAXGROUPS];
extern far DMAN3_Context _DMAN3_groupContext[DMAN3_MAXGROUPS];

extern far RMM_Handle _DMAN3_rmmTccs;
extern far RMM_Handle _DMAN3_rmmPaRams;

extern far Int _DMAN3_heapInt;
extern far Int _DMAN3_heapExt;
extern far Int _DMAN3_heapIdma3;

extern far DMAN3_ScratchAllocFxn _DMAN3_allocScratch;
extern far DMAN3_ScratchFreeFxn _DMAN3_freeScratch;

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_DMAN3_*/

/*
 *  @(#) ti.sdo.fc.dman3; 1, 0, 1,66; 5-10-2007 10:49:43; /db/atree/library/trees/fc-b12x/src/
 */

