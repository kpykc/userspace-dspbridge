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
 *  ======== iscale.h ========
 *  ISCALE Interface Header
 *
 */
#ifndef ISCALE_
#define ISCALE_

#include <ialg.h>

/*
 *  ======== ISCALE_Handle ========
 *  This handle is used to reference all SCALE instance objects
 */
typedef struct ISCALE_Obj *ISCALE_Handle;

/*
 *  ======== ISCALE_Obj ========
 *  This structure must be the first field of all SCALE instance objects
 */
typedef struct ISCALE_Obj {
    struct ISCALE_Fxns *fxns;
} ISCALE_Obj;

/*
 *  ======== ISCALE_Status ========
 *  Status structure defines the parameters that can be changed or read
 *  during real-time operation of the alogrithm.
 */
typedef struct ISCALE_Status {
    Int size;	/* must be first field of all status structures */
    Uns scaleFactor;
} ISCALE_Status;
/*
 *  ======== ISCALE_Cmd ========
 *  The Cmd enumeration defines the control commands for the SCALE
 *  control method.
 */
typedef enum ISCALE_Cmd {
	ISCALE_GETSCALE,
	ISCALE_SETSCALE
} ISCALE_Cmd;

/*
 *  ======== ISCALE_Params ========
 *  This structure defines the creation parameters for all SCALE objects
 */
typedef struct ISCALE_Params {
    Int size;	/* must be first field of all params structures */
    Uns scaleFactor;
} ISCALE_Params;

/*
 *  ======== ISCALE_PARAMS ========
 *  Default parameter values for SCALE instance objects
 */
extern ISCALE_Params ISCALE_PARAMS;

/*
 *  ======== ISCALE_Fxns ========
 *  This structure defines all of the operations on SCALE objects
 */
typedef struct ISCALE_Fxns {
    IALG_Fxns	ialg;    /* ISCALE extends IALG */
    Void  (*scale)(ISCALE_Handle handle, Uns *in, Uns *out, Uns size);

} ISCALE_Fxns;

#endif	/* ISCALE_ */
