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
 *  ======== misalign.cmd ========
 *
 *  Linker command template for testing EXTMEM loader
 *  Deliberately misalign text and data sections to ensure that loader
 *  adjusts during load.
 *
 *  Note: Generated linker command file is automatically linked in via
 *  the default linkcmd.xdt template.
 *
 */

/* Move code section by renaming section and loading in EXTMEM at odd addr */
SECTIONS {
    .textExt:    { *(.text) }, load = 0x1C5423
}
