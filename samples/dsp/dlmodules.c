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


// Applications wishing to use the DLLview plug-in must reserve a special
// symbol to be used by the dynamic loader and by DLLview.  
//
// The symbol's name is  __DLModules.  It is the address of a 4-word struct
// initialized to all zeros.
//
// This C code example shows how to reserve and initialize __DLModules.
// Note that the C compiler will prepend an underscore to the name of variable
// _DLModules. 

typedef unsigned short uint16_t;

#if defined(__TMS320C55X__) || defined(_TMS320C5XX)
typedef unsigned long uint32_t;
#else
typedef unsigned uint32_t;
#endif

#pragma DATA_SECTION(_DLModules, ".dllview");
const uint32_t _DLModules[] = {0, 0};
