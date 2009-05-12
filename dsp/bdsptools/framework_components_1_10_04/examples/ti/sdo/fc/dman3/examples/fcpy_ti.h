/*
 *  Copyright 2007 by Texas Instruments Incorporated.
 *
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */

/*
 *  ======== fcpy_ti.h ========
 *  Interface header for the FCPY_TI module.
 */
#ifndef FCPY_TI_
#define FCPY_TI_

#ifndef IDMA3_USEFULLPACKAGEPATH
#define IDMA3_USEFULLPACKAGEPATH
#endif

#include <ti/xdais/ialg.h>
#include <ifcpy.h>
#include <ti/xdais/idma3.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*
 *  ======== FCPY_TI_Handle ========
 *  FCPY algorithm instance handle
 */
typedef struct FCPY_TI_Obj *FCPY_TI_Handle;

/*
 *  ======== FCPY_TI_exit ========
 *  Required module finalization function.
 */
extern Void FCPY_TI_exit(Void);

/*
 *  ======== FCPY_TI_init ========
 *  Required module initialization function.
 */
extern Void FCPY_TI_init(Void);

/*
 *  ======== FCPY_TI_IALG ========
 *  TI's implementation of FCPY's IALG interface
 */
extern IALG_Fxns FCPY_TI_IALG; 

/*
 *  ======== FCPY_TI_IFCPY ========
 *  TI's implementation of FCPY's IFCPY interface
 */
extern IFCPY_Fxns FCPY_TI_IFCPY; 

/*
 *  ======== FCPY_TI_IDMA3 ========
 *  TI's implementation of FCPY's IDMA2 interface
 */
extern IDMA3_Fxns FCPY_TI_IDMA3; 

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* FCPY_TI_ */


/*
 *  @(#) ti.sdo.fc.dman3.examples; 1,0,0,36; 5-10-2007 10:49:53; /db/atree/library/trees/fc-b12x/src/
 */

