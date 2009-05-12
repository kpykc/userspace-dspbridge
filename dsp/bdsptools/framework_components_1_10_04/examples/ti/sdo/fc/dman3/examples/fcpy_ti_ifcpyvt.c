/*
 *  Copyright 2007 by Texas Instruments Incorporated.
 *
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */

/*
 *  ======== fcpy_ti_ialgvt.c ========
 *  This file contains the function table definitions for the
 *  IALG and IFCPY interfaces implemented by the FCPY_TI module.
 */

#include <std.h>

#include <fcpy_ti.h>
#include <ifcpy.h>
#include <fcpy_ti_priv.h>

#define IALGFXNS \
    &FCPY_TI_IALG,     /* module ID */                         \
    NULL,              /* activate (NULL => no need to initialize buffers */ \
    FCPY_TI_alloc,     /* alloc */                             \
    NULL,              /* control (NULL => no control ops) */  \
    NULL,              /* deactivate (NULL => no need to save data */ \
    FCPY_TI_free,      /* free */                              \
    FCPY_TI_initObj,   /* init */                              \
    FCPY_TI_moved,     /* moved */                             \
    NULL               /* numAlloc (NULL => IALG_MAXMEMRECS) */

/*
 *  ======== FCPY_TI_IFCPY ========
 *  This structure defines TI's implementation of the IFCPY interface
 *  for the FCPY_TI module.
 */
IFCPY_Fxns FCPY_TI_IFCPY = {       /* module_vendor_interface */
    IALGFXNS,                      /* IALG functions */    
    FCPY_TI_control,               /* Control function */
    FCPY_TI_doCopy                 /* The fcpy fxn */
};

/* Overlay v-tables to save data space */
asm("_FCPY_TI_IALG  .set _FCPY_TI_IFCPY");



/*
 *  @(#) ti.sdo.fc.dman3.examples; 1,0,0,36; 5-10-2007 10:49:53; /db/atree/library/trees/fc-b12x/src/
 */

