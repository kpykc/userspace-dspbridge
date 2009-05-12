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
 *  ======== dpdma.h ========
 *  DMA Pipe Driver Interface.
 *
 */

#ifndef DPDMA_
#define DPDMA_

#include <dev.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DPDMA_Params 
{
    Uns    dummy;
} DPDMA_Params;

extern	Void	    DPDMA_init();

extern	DEV_Fxns    DPDMA_FXNS;

extern DPDMA_Params DPDMA_PARAMS;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif	/* DPDMA_ */
