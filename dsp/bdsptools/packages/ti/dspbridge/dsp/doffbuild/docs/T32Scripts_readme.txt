Lauterbach debug script usage procedure

Notes:

Hardware Platform:
- These scripts have only been tested with OMAP2420

Software Version:
- These scripts were tested with the April 6, 2005 build of
  Trace32 (t32m320.exe)

*** Debug Setup

1.) Unzip the DSPBridge-T32Scripts.zip archive into the Trace32 installation
directory (e.g. "c:\t32").  Doing this should create a subdirectory
named "DSPBridge" in which the scripts will be placed.

2.) Create a shortcut to the Trace32 application that also loads
the DSPBridge "DSPBridge\init.cmm" script when loaded.  If the
Trace32 working directory is "c:\t32", then the shortcut would be
configured as shown here:

   - Target:
     c:\t32\t32m320.exe -c c:\t32\config.t32,c:\t32\DSPBridge\init.cmm

   - Working Directory:
     c:\t32\DSPBridge

3.) Configure the "DSPBridge\config.cmm" script:

   - Set the searchpath for DSP Bridge Source files:
     e.g. (y.spath.srd "H:\dsp\ti\dspbridge\dsp")

---

*** Debug Process

* Debugging a Static Base Image

1.) Invoke the Trace32-DSP shortcut.  When Trace32 starts, the DSPBridge
"init.cmm" script will run, and a menu named "TI-DSP" will be added to the
T32 menus.

2.) When your DSP has been loaded with code and pulled out of reset, go
into the "TI-DSP" menu and select "Attach to target". This will attach Trace32
to the DSP non-intrusively.  (Note: the DSP is still running after this
has been executed.)

3.) Select the "Load base image symbols" option from the "TI-DSP" menu.
You will be prompted by a dialog to indicate the name and path of the base
image.  Upon doing this, T32 will load the base image symbols (but not
the code, that's loaded from the ARM side of DSP/BIOS Bridge) from the COFF
base image (&imagepath) you have indicated. This image must of course
correspond to the DOFF image loaded by the ARM side of DSP/BIOS Bridge.
(Note: that the DSP is still running after this has been executed.)

4.) Debugging of static nodes in a static image
Now you can stop the DSP, inspect symbols, and single step through the
nodes in your static image, as well as the DSP/BIOS Bridge code itself.

* Debugging Dynamically Loaded Non-"Split-Phase" Bridge Nodes

5.) To debug dynamic nodes, you will need a different baseimage to do
dynamic loading. When you know your DSP is loaded with code and out
of reset, attach using the "TI-DSP" menu (see above) and follow steps
1-3 above using your new base image.

6.) When you know your node is running, you can halt the target and select
"Load dynamic node symbols" from the "TI-DSP" menu. This will read the
dynamic loader entry table on the target and determine the name of the
COFF file it should load to get the symbols. The script will then ask you
to select this file somewhere on your file system. When you do, it will
load the DLL symbols and you can single step debug the DLL on your DSP.

* Debugging Dynamically Loaded "Split-Phase" Bridge Nodes

7.) To debug split-phase dynamic nodes, know that for each DSP Bridge
phase, different libraries may be loaded/unloaded each time.  Because of
this, the T32 symbol table must be refreshed.  When you have stopped in a
new phase, first unload the previously loaded symbols from the symbol
table by selecting "Delete Symbols" from the "TI-DSP" menu.  After that,
reload the base image and Dynamic Node symbols as needed (Step 3,6 above).
