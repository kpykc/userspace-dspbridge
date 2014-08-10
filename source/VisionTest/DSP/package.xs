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

/* all programs in this package use bridge */

/*
 * Typically, we'd want to include the explicit packages we use.  Due to a
 * limitation in the codegen tools (only 7 -i options are allowed to be
 * passed to the linker, Bios uses 3, bridge uses 6), we instead use the
 * bridge_product package.
 */
// Want to do this...
if (0) {
    xdc.loadPackage('ti.dspbridge.dsp.bridge');
    xdc.loadPackage('ti.dspbridge.dsp.acpy');
    xdc.loadPackage('ti.dspbridge.dsp.dman');
    xdc.loadPackage('ti.dspbridge.dsp.usr');
    xdc.loadPackage('ti.dspbridge.dsp.newdev');
    xdc.loadPackage('ti.dspbridge.dsp.tiomap1510');
}

// ... instead do this...
xdc.loadPackage('ti.rtdx');
xdc.loadPackage('ti.dspbridge.dsp.bridge_product');

/* and finally, the BIOS package */
//xdc.loadPackage('ti.bios');

/*
 *  ======== getSects ========
 *  This method is called during generation (after a program's
 *  configuration script completes) and is responsible for returning a
 *  template file name that is used to generate package-specific
 *  linker command file statements.
 *
 *  Context:
 *      this    the current package's xdc.IPackage.Module object
 *          after the program's configuration script has completed.
 *
 *      prog    Configuration model program object (xdc.Program) after
 *          program's configuration script has completed.
 *
 *  Returns
 *      string  path name of template file.  The path name is relative
 *          to the package path.
 *
 *          If null, no data is to be generated.
 *
 *          the template is evaluated in a context where the this
 *          pointer is the package object.
 */
function getSects()
{
    if (prog.name == "ddspbaseMisalign_tiomap1510.x55L") {
        print("*** Setting up customized template ***");
        return("misalign.xdt");
    }
    else {
        return("inst2cmd.xdt");
        return null;
    }
}
