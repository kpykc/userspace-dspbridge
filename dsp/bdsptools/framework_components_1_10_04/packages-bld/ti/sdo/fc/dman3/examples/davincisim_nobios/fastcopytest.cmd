/*
 *  Linker-command file for the fcpytest_rts application
 */

-stack 0x1000
-heap 0x2000

--trampolines

/* these libraries are autoincluded since they belong to dependent projects */
/* -l ../../../acpy3/acpy3.l64P   */
/* -l ../../../dman3/dman3.l64P   */

-l api.l64
-lrts64plus.lib        /* C and C++ run-time library support */

MEMORY {
   CACHE_L2    : origin = 0x800000,      len = 0x10000
   CACHE_L1P   : origin = 0x11e08000,    len = 0x8000
   CACHE_L1D   : origin = 0x11f10000,    len = 0x8000
   EXTMEM      : origin = 0x83f00000,    len = 0x0ffe00 
   L1DSRAM     : origin = 0x11f04000,    len = 0xc000
}


_FCPY_IFCPY = _FCPY_TI_IFCPY;
_FCPY_IDMA3 = _FCPY_TI_IDMA3;
_DMAN3_EDMA3BASE = 0x01C00000;

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
