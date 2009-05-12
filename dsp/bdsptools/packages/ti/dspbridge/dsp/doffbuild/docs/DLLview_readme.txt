
DLLview v1.2

Dynamically Loaded code debugging plug-in for Code Composer Studio.

Note: DLLview v1.2 will only install on CCS installations that rely on v3.0
of the CCS SDK.  This includes CCS 3.2.  Users with CCS 2.x are advised to
use DLLview v1.0.  Users with OMAP3430 Virtio are advised to use DLLview v1.1.

** Installation:

To install DLLview, copy the DLLview.ocx plug-in to a preferred location
and run the "register_DLLview.bat" batch file.  This batch file invokes
"regsvr32" to register the plug-in with the system.  When you start CCS,
you should be presented with a dialog informing you that a new component
has been detected.

** Usage:
Please refer to "dynload.hlp" for information on how to use DLLview.

Updates from v1.1:

- DLLview will bypass cache for C6x targets

In previous versions on OMAP2430, if the DLLview module header was
located in a cached memory space, cache coherence issues would result
whenever the ARM side dynamic loader adjusted the debug list of
dynamically loaded modules.  The workaround was to place the module
header (indicated by _DLModules symbol) in an uncached region of memory
or to disable cache altogether.  Now, DLLview will always bypass cache
when reading memory, so no special action needs to be taken.

The API used to bypass cache is the SDKv3.0 API "SetMemoryAccessLevel".
(Note: Using this API causes problems for OMAP3430 Virtio, which does
 not support cache bypass).

Updates from v1.0:

** Bug fixes:

- Built against SDKv3.0 for CCS 3.2.
Plug-in is build against the latest SDK to support CCS 3.2.

- Workaround for CCDspUser::GetSymbolValue()

The CCDspUser::GetSymbolValue() API fails to find the _DLModules symbol
when a query is performed.  Work around uses the CCSymbols::GetSymbolByName()
and CCSymbol::GetValue() APIs to query the symbol, as other plug-ins do.
