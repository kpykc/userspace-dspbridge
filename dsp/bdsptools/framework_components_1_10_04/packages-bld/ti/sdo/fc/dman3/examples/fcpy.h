/*
 *  Copyright 2007 by Texas Instruments Incorporated.
 *
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */

/*
 *  ======== fcpy.h ========
 *  This header defines all types, constants, and functions used by 
 *  clients of the FCPY algorithm.
 */
#ifndef FCPY_
#define FCPY_

#include <ti/sdo/fc/utils/api/alg.h>
#include <ifcpy.h>
#include <ti/xdais/ialg.h>
#include <ti/xdais/xdas.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*
 *  ======== FCPY_Cmd ========
 *  FCPY instance control commands.
 */
typedef IFCPY_Cmd FCPY_Cmd;

/* control commands */
#define FCPY_GETSTATUS IFCPY_GETSTATUS
#define FCPY_SETSTATUS IFCPY_SETSTATUS

/*
 *  ======== FCPY_Handle ========
 *  FCPY algorithm instance handle
 */
typedef struct IFCPY_Obj *FCPY_Handle;

/*
 *  ======== FCPY_Params ========
 *  FCPY algorithm instance creation parameters
 */
typedef IFCPY_Params FCPY_Params;

/*
 *  ======== FCPY_PARAMS ========
 *  Default interface parameters
 */
#define FCPY_PARAMS IFCPY_PARAMS

/*
 *  ======== FCPY_Status ========
 *  FCPY algorithm status parameters
 */
typedef IFCPY_Status FCPY_Status;

/*
 *  ======== FCPY_apply ========
 *  Apply a FCPY function to the input array and place results in the
 *  output array.
 */
extern Void FCPY_apply(FCPY_Handle handle, Void * in, Void * out);

/*
 *  ======== FCPY_create ========
 *  Create an instance of a FCPY object.
 */
static inline FCPY_Handle FCPY_create(const IFCPY_Fxns *fxns, const FCPY_Params *prms)
{
    return((FCPY_Handle)ALG_create((IALG_Fxns *)fxns, NULL, (IALG_Params *)prms));
}

/*
 *  ======== FCPY_control ========
 *  Used to either write to the read/write parameters in the status
 *  structure or to read all the parameters in the status structure.
 */
extern XDAS_Bool FCPY_control(FCPY_Handle handle, FCPY_Cmd cmd, FCPY_Status *status);

/*
 *  ======== FCPY_delete ========
 *  Delete a FCPY instance object
 */
static inline Void FCPY_delete(FCPY_Handle handle)
{
    ALG_delete((ALG_Handle)handle);
}

/*
 *  ======== FCPY_exit ========
 *  Module finalization
 */
extern Void FCPY_exit(Void);

/*
 *  ======== FCPY_init ========
 *  Module initialization
 */
extern Void FCPY_init(Void);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif  /* FCPY_ */

/*
 *  @(#) ti.sdo.fc.dman3.examples; 1,0,0,36; 5-10-2007 10:49:53; /db/atree/library/trees/fc-b12x/src/
 */

