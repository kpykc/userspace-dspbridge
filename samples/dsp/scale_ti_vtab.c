/*
 *  Copyright 2001-2008 Texas Instruments - http://www.ti.com/
 * 
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 *  ======== scale_ti_vtab.c ========
 *  Definition of SCALE_TI_ISCALE.  
 *
 *  Since ISCALE derives from IALG, we also define the symbol SCALE_TI_IALG, 
 *  TI's implementation of the IALG interface.
 *
 *
 */
#include <std.h>
#include "iscale.h"

#include "scale_ti.h"

extern Void SCALE_TI_activate(IALG_Handle);
extern Int SCALE_TI_alloc(const IALG_Params *, struct IALG_Fxns **, 
    IALG_MemRec *);
extern Int SCALE_TI_control(IALG_Handle, IALG_Cmd, IALG_Status *);
extern Void SCALE_TI_deactivate(IALG_Handle);
extern Int SCALE_TI_free(IALG_Handle, IALG_MemRec *);
extern Int SCALE_TI_initObj(IALG_Handle, const IALG_MemRec *, IALG_Handle, 
    const IALG_Params *);
extern Void SCALE_TI_moved(IALG_Handle, const IALG_MemRec *, IALG_Handle, 
    const IALG_Params *);
extern Int SCALE_TI_numAlloc(Void);
extern Void  SCALE_TI_scale(ISCALE_Handle handle, Uns *in, Uns *out, Uns size);

#define IALGFXNS \
    &SCALE_TI_IALG,		    /* module ID */			\
    SCALE_TI_activate,		    /* activate */			\
    SCALE_TI_alloc,		    /* algAlloc */			\
    SCALE_TI_control,		    /* control ops */			\
    SCALE_TI_deactivate,	    /* deactivate */			\
    SCALE_TI_free,		    /* free */				\
    SCALE_TI_initObj,		    /* init */				\
    SCALE_TI_moved,		    /* moved (NULL => not suported) */	\
    NULL			    /* numAlloc (NULL => IALG_DEFMEMRECS) */\

/*
 *  ======== SCALE_TI_ISCALE ========
 *  This structure defines TI's implementation of the ISCALE interface
 *  for the SCALE_TI module.
 */
ISCALE_Fxns SCALE_TI_ISCALE = {	    /* module_vendor_interface */
    IALGFXNS,
    SCALE_TI_scale,
};

/*
 *  ======== SCALE_TI_IALG ========
 *  This structure defines TI's implementation of the IALG interface
 *  for the SCALE_TI module.
 */
asm("_SCALE_TI_IALG .set _SCALE_TI_ISCALE");

