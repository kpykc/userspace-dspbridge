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
 *  ======== iscale.c ========
 *  This file defines the default parameter structure for iscale.h
 *
 */

#include <std.h>
#include "iscale.h"

/*
 *  ======== SCALE_PARAMS ========
 *  This constant structure defines the default parameters for SCALE objects.
 */
ISCALE_Params ISCALE_PARAMS = {
    sizeof(ISCALE_Params),
    0, /* Uns scaleFactor; */

};

