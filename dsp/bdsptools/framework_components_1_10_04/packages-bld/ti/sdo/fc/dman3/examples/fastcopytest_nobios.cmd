/* this is needed so that linker will find all objects */
-x

_FCPY_IFCPY = _FCPY_TI_IFCPY;
_FCPY_IDMA3 = _FCPY_TI_IDMA3;
_DMAN3_EDMA3BASE = 0x01C00000;

-stack 0x1000
-heap 0x2000

--trampolines

-lrts64plus.lib        /* C and C++ run-time library support */

-l../../../../../ti/sdo/fc/acpy3/acpy3d.a64P
-l../../../../../ti/sdo/fc/dman3/dman3d.a64P
-l../../../../../ti/sdo/fc/utils/api/api.a64P
-lfcpy_ti.a64P

MEMORY {
   CACHE_L2    : origin = 0x800000,      len = 0x10000
   CACHE_L1P   : origin = 0x11e08000,    len = 0x8000
   CACHE_L1D   : origin = 0x11f10000,    len = 0x8000
   EXTMEM      : origin = 0x83f00000,    len = 0x0ffe00 
   L1DSRAM     : origin = 0x11f04000,    len = 0xc000
}

SECTIONS
{    
    .image       > EXTMEM
    .sysmem      > L1DSRAM
    .far:     {} > EXTMEM
    .text:    {} > EXTMEM
    .const:   {} > EXTMEM
    .cinit:   {} > EXTMEM
    .stack:   {} > EXTMEM
    .switch:  {} > EXTMEM
    .cio:     {} > EXTMEM
}
