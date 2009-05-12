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
 *  ======== scale_ti.h ========
 *  Interface for the SCALE_TI module; TI's implementation 
 *  of the ISCALE interface.
 *
 */
#ifndef SCALE_TI_
#define SCALE_TI_

#include "iscale.h"
#include <ialg.h>

/*
 *  ======== SCALE_TI_IALG ========
 *  TI's implementation of the IALG interface for SCALE
 */
extern IALG_Fxns SCALE_TI_IALG;

/*
 *  ======== SCALE_TI_ISCALE ========
 *  TI's implementation of the ISCALE interface
 */
extern ISCALE_Fxns SCALE_TI_ISCALE;

#endif	/* SCALE_TI_ */
