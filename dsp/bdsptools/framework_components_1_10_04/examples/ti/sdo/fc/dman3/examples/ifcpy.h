/*
 *  Copyright 2007 by Texas Instruments Incorporated.
 *
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */

/*
 *  ======== ifcpy.h ========
 *  This header defines all types, constants, and functions shared by all
 *  implementations of the FCPY algorithm.
 */
#ifndef IFCPY_
#define IFCPY_

#include <ti/xdais/ialg.h>
#include <ti/xdais/xdas.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*
 *  ======== IFCPY_Obj ========
 *  This structure must be the first field of all FCPY instance objects.
 */
typedef struct IFCPY_Obj {
    struct IFCPY_Fxns *fxns;
} IFCPY_Obj;

/*
 *  ======== IFCPY_Handle ========
 *  This handle is used to reference all FCPY instance objects.
 */
typedef struct IFCPY_Obj *IFCPY_Handle;

/*
 *  ======== IFCPY_Cmd ========
 *  This structure defines the control commands for the FCPY module. 
 */
typedef enum IFCPY_Cmd {
    IFCPY_GETSTATUS,
    IFCPY_SETSTATUS
} IFCPY_Cmd;

/*
 *  ======== IFCPY_Params ========
 *  This structure defines the creation parameters for all FCPY instance
 *  objects.
 */
typedef struct IFCPY_Params {
    Int  size;        /* Size of this structure */ 

    /* The following two parameters are read-only */    
    Int  srcLineLen;  /* Source line length (# of 8-bit elements) */
    Int  srcNumLines; /* Number of lines for source */
    
    /* The following parameters are read/write */    
    Int  srcStride;   /* Stride between lines for source */
    Int  dstLineLen;  /* Destination line length (# of 8-bit elements) */
    Int  dstNumLines; /* Number of lines for destination */
    Int  dstStride;   /* Stride between lines for destination */
} IFCPY_Params;

extern const IFCPY_Params IFCPY_PARAMS; /* default params */

/*
 *  ======== IFCPY_Status ========
 *  This structure defines the parameters that can be changed at runtime
 *  (read/write), and the instance status parameters (read-only).
 */
typedef struct IFCPY_Status {
    Int  size;            /* Size of this structure */ 
    
    /* The following two parameters are read-only */
    Int  srcLineLen;      /* Source line length (# of 8-bit elements) */
    Int  srcNumLines;     /* Number of lines for source */
    
    /* The following parameters are read/write */
    Int  srcStride;       /* Stride between lines for source */
    Int  dstLineLen;      /* Destination line length (# of 8-bit elements) */
    Int  dstNumLines;     /* Number of lines for destination */
    Int  dstStride;       /* Stride between lines for destination */
} IFCPY_Status;

/*
 *  ======== IFCPY_Fxns ========
 *  This structure defines all of the operations on FCPY objects.
 */
typedef struct IFCPY_Fxns {
    IALG_Fxns   ialg;    /* IFCPY extends IALG */
    XDAS_Bool  (*control)(IFCPY_Handle handle, IFCPY_Cmd cmd, 
        IFCPY_Status *status);
    Void       (*doCopy)(IFCPY_Handle handle, Void * in, Void * out);
} IFCPY_Fxns;

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif  /* IFCPY_ */





/*
 *  @(#) ti.sdo.fc.dman3.examples; 1,0,0,36; 5-10-2007 10:49:54; /db/atree/library/trees/fc-b12x/src/
 */

