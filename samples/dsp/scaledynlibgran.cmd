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

/* linker command file for dynamic loading */

	-r	/* relocatable output */
	-cr	/* load-time initialization */

SECTIONS
{
  .text:algAlloc:DYN_DARAM:0: { *(.text:algAlloc)}
  .text:algFree:DYN_DARAM:1: { *(.text:algFree)}
  .text:algInit:DYN_SARAM:0: { *(.text:algInit)}
  .text:init:DYN_SARAM:1: { *(.text:init)}
  .text:exit:DYN_EXTERNAL:0: { *(.text:exit)}
}
